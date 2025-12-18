// Implements a spell-checker in C++ using C++ I/O (cout/cerr)

#include <cctype>      // For isalpha, isdigit, isalnum
#include <cstdio>      // For FILE*, fopen, fclose, ferror, fread (C file I/O must remain)
#include <cstdlib>     // For EXIT_SUCCESS, EXIT_FAILURE
#include <ctime>       // For clock_t, clock, CLOCKS_PER_SEC
#include <iostream>    // For cout, cerr, endl

// Assuming dictionary.h defines these functions and the LENGTH constant
#include "dictionary.h"

// Bring the entire std namespace into scope
using namespace std;

// Default dictionary
#define DICTIONARY "dictionaries/large"

// Function prototype
double calculate(clock_t start, clock_t end);

// The 'main' function is the entry point
int main(int argc, char *argv[])
{
    // Check for correct number of arguments
    if (argc != 2 && argc != 3)
    {
        // Using cerr for error message
        cerr << "Usage: ./speller [DICTIONARY] text" << endl;
        return EXIT_FAILURE;
    }

    // Timing variables
    clock_t start, end;
    double time_load = 0.0, time_check = 0.0, time_size = 0.0, time_unload = 0.0;

    // Determine dictionary to use
    const char *dictionary = (argc == 3) ? argv[1] : DICTIONARY;

    // Load dictionary
    start = clock();
    bool loaded = load(dictionary); 
    end = clock();
    time_load = calculate(start, end);

    if (!loaded)
    {
        cerr << "Could not load " << dictionary << "." << endl;
        return EXIT_FAILURE;
    }

    // Try to open text file
    const char *text = (argc == 3) ? argv[2] : argv[1];
    FILE *file = fopen(text, "r");
    if (file == NULL)
    {
        cerr << "Could not open " << text << "." << endl;
        unload();
        return EXIT_FAILURE;
    }

    // Prepare to report misspellings
    cout << "\nMISSPELLED WORDS\n" << endl;

    // Prepare to spell-check
    int index = 0, misspellings = 0, words = 0;
    char word[LENGTH + 1]; 
    char c;

    // Spell-check each word in text
    while (fread(&c, sizeof(char), 1, file))
    {
        // Handle alphabetic characters and apostrophes
        if (isalpha(c) || (c == '\'' && index > 0))
        {
            word[index++] = c;
            
            // Check for word overflow
            if (index > LENGTH)
            {
                // Consume the rest of the too-long word
                while (fread(&c, sizeof(char), 1, file) && isalpha(c));
                index = 0;
            }
        }
        // Handle numeric characters (to skip them)
        else if (isdigit(c))
        {
            // Consume the rest of the number (alphanumeric)
            while (fread(&c, sizeof(char), 1, file) && isalnum(c));
            index = 0;
        }
        // Encountered a separator (space, punctuation, etc.) and have a word buffered
        else if (index > 0)
        {
            // Null-terminate the word
            word[index] = '\0';
            words++;

            // Time the check function
            start = clock();
            bool misspelled = !check(word);
            end = clock();
            time_check += calculate(start, end);

            // Report misspelling
            if (misspelled)
            {
                cout << word << endl;
                misspellings++;
            }

            // Reset index for the next word
            index = 0;
        }
    }

    // Check for file read error
    if (ferror(file))
    {
        fclose(file);
        cerr << "Error reading " << text << "." << endl;
        unload();
        return EXIT_FAILURE;
    }

    // Close the text file
    fclose(file);

    // Determine dictionary size
    start = clock();
    unsigned int n = size();
    end = clock();
    time_size = calculate(start, end);

    // Unload dictionary
    start = clock();
    bool unloaded = unload();
    end = clock();
    time_unload = calculate(start, end);

    if (!unloaded)
    {
        cerr << "Could not unload " << dictionary << "." << endl;
        return EXIT_FAILURE;
    }

    // Report benchmarks
    cout << "\nWORDS MISSPELLED:     " << misspellings << endl;
    cout << "WORDS IN DICTIONARY:  " << n << endl;
    cout << "WORDS IN TEXT:        " << words << endl;
    cout << "TIME IN load:         " << fixed << setprecision(2) << time_load << endl;
    cout << "TIME IN check:        " << fixed << setprecision(2) << time_check << endl;
    cout << "TIME IN size:         " << fixed << setprecision(2) << time_size << endl;
    cout << "TIME IN unload:       " << fixed << setprecision(2) << time_unload << endl;
    cout << "TIME IN TOTAL:        " << fixed << setprecision(2)
         << time_load + time_check + time_size + time_unload << "\n" << endl;

    return EXIT_SUCCESS;
}

/**
 * @brief Returns the number of seconds between start and end.
 * @param start The starting time measured by clock().
 * @param end The ending time measured by clock().
 * @return double The duration in seconds.
 */
double calculate(clock_t start, clock_t end)
{
    // C++: Explicit casting to 'double'
    return static_cast<double>(end - start) / static_cast<double>(CLOCKS_PER_SEC);
}
