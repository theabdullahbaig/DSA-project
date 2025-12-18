// Implements a dictionary's functionality

#include <cctype>       // For isalpha, tolower
#include <cstdio>       // For FILE*, fopen, fclose, fscanf
#include <cstdlib>      // For EXIT_FAILURE, etc.
#include <cstring>      // For strcpy
#include <iostream>     // For cout, cerr, endl
#include <stdexcept>    // For bad_alloc
#include <strings.h>    // For strcasecmp (POSIX)

#include "dictionary.h"

using namespace std;

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
} node;

// TODO: Choose number of buckets in hash table
#define N 26

node *table[N];
unsigned int word_count = 0;

// Returns true if word is in dictionary, else false
bool check(const char *word)
{
    unsigned int index = hash(word);

    // Note: The variable 'n' below is shadowed by the 'n' in the loop.
    // Kept to preserve original logic, but strictly unnecessary.
    node* n = table[index];

    for (node *n = table[index]; n != nullptr; n = n->next)
    {
        if(strcasecmp(word, n->word) == 0)
        {
            return true;
        }
    }
    return false;
}

// Hashes word to a number
unsigned int hash(const char *word)
{
    if (isalpha(word[0]))
    {
        return tolower(word[0]) - 'a';
    }
    return 0; // fallback for non-alphabetical inputs
}

// Loads dictionary into memory, returning true if successful, else false
bool load(const char *dictionary)
{
    FILE* file = fopen(dictionary,"r");
    if(file == NULL){
        cerr << "Sorry! The program could not open the file due to some error!!" << endl;
        return false;
    }

    for (int i = 0; i < N; i++)
    {
        table[i] = nullptr; // sets each linked list head to NULL (using nullptr)
    }

    char word[LENGTH+1];
    while(fscanf(file, "%s", word) != EOF){

        // C++: Using 'new' operator for allocation
        node* new_node;
        try
        {
            new_node = new node;
        }
        catch (const bad_alloc& e)
        {
            cerr << "Uhmm, Sorry the program couldn't allocate enough memory for your words!" << endl;
            fclose(file);
            return false;
        }

        unsigned int index = hash(word);
        strcpy(new_node->word, word);
        new_node->next = table[index];
        table[index] = new_node;

        word_count++;
    }

    fclose(file);

    return true;
}

// Returns number of words in dictionary if loaded, else 0 if not yet loaded
unsigned int size()
{
    return word_count;
}

// Unloads dictionary from memory, returning true if successful, else false
bool unload(void)
{
    node* cursor;
    for(int i = 0 ; i < N ; i++){
        cursor = table[i];
        while (cursor != nullptr)
        {
            node *tmp = cursor;
            cursor = cursor->next;
            // C++: Using 'delete' for deallocation
            delete tmp;
        }
    }
    return true;
}
