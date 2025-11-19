// Implements a dictionary's functionality

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include "dictionary.h"
#include <stdio.h>
#include <string.h>
#include <strings.h> // Added: Required in C++ for strcasecmp (POSIX)

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
    int index = hash(word);
    
    // Note: The variable 'n' below is shadowed by the 'n' in the loop.
    // Kept to preserve original logic, but strictly unnecessary.
    node* n = table[index]; 
    
    for (node *n = table[index]; n != NULL; n = n->next)
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
        printf("Sorry! the program could not open the file due to some error!!");
        return false;
    }
    
    for (int i = 0; i < 26; i++) 
    {
        table[i] = NULL; // sets each linked list head to NULL
    }

    char word[LENGTH+1];
    while(fscanf(file, "%s", word) != EOF){
        
        // CHANGE: Added (node*) cast. C++ requires explicit casting for malloc.
        node* n = (node*)malloc(sizeof(node));
        
        if(n == NULL){
            printf("Uhmm, Sorry the program couldn't allocate enough memory for your words!");
            return false;
        }
        int index = hash(word);
        strcpy(n->word, word);
        n->next = table[index];
        table[index] = n;

        word_count++; 
    }
    
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
    node* n;
    for(int i = 0 ; i < 26 ; i++){
        n = table[i];
        while (n != NULL)
        {
            node *tmp = n;
            n = n->next;
            free(tmp);
        }
    }
    return true;
}