/*
 * 0000 cannot be used stored in the hash table, as it is the default state of each slot.
 * Labels should have a maximum size of only 4 characters.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SIZE 5                                                      // size of the hash table
#define ENTRY struct Entry
#define HASHTB struct HashTable

struct Entry{                                                       // to store each entry in the hash table buckets (slots)
    char label[5];
    int address;
    struct Entry * next;
};
struct HashTable{                                                   // hash table structure that stores a pointer to the first entry in each bucket (slot)
    struct Entry * slot;
};

void lookup(HASHTB symbolTable[], char label[]){                    // search the symbol table for the given label
    int i;
    unsigned int key = (int)label[0];
    ENTRY * pointer;
    for(i = 1; i < strlen(label); i++){                                                 // calculate the key
        key <<= 7;
        key |= (int)label[i];
    }
    key %= SIZE;
    pointer = symbolTable[key].slot;
    while(pointer != NULL && strcmp(pointer->label, label) != 0)                        // search the hash bucket (slot) for the required label
        pointer = pointer->next;
    if (pointer == NULL)                                                                // when label is not found
        printf("\n Symbol not found in the symbol table.\n");
    else                                                                                // when label is found
        printf("\n Key: %d\n Label: %s\n Address: %d\n", key, label, pointer->address);
}

void insert(HASHTB symbolTable[], char label[], int address) {      // insert a symbol into the symbol table
    int i;
    unsigned int key = (int) label[0];
    ENTRY *pointer;
    for (i = 1; i < strlen(label); i++) {                                               // calculate the key
        key <<= 7;
        key |= (int) label[i];
    }
    key %= SIZE;
    if (symbolTable[key].slot == NULL){                                                 // if the hash bucket (slot) is empty
        symbolTable[key].slot = (ENTRY *) malloc(sizeof(ENTRY));
        if (symbolTable[key].slot == NULL){
            printf("\n[!] Error in memory allocation detected. Could not update symbol table.\n");
            return;
        }
        symbolTable[key].slot->address = address;
        symbolTable[key].slot->next = NULL;
        strcpy(symbolTable[key].slot->label, label);
        printf("\n Inserted \'%s\' with key <%d> \n", label, key);
        return;
    }
    pointer = symbolTable[key].slot;
    while (pointer->next != NULL && strcmp(pointer->label, label) != 0)                 // go to the end of the linked list in the bucket
        pointer = pointer->next;
    if (!strcmp(pointer->label, label)){                                                // if the symbol is encountered while going to the end of the linked list
        printf("\n[!] Symbol already exists in symbol table.\n");
        return;
    }
    pointer->next = (ENTRY *) malloc(sizeof(ENTRY));                                    // insert the new symbol at the end of the linked list
    if (pointer->next == NULL){
        printf("\n[!] Error in memory allocation detected. Could not update symbol table.\n");
        return;
    }
    pointer->next->address = address;
    pointer->next->next = NULL;
    strcpy(pointer->next->label, label);
    printf("\n Inserted \'%s\' with key <%d> \n", label, key);
}


void display(HASHTB symbolTable[]){                                 // display the symbols in the symbol table
    int i;
    ENTRY * pointer;
    printf("\n------- Hash Table Dump -------");
    for(i = 0; i < SIZE; i++){
        if(symbolTable[i].slot != NULL) {                                               // if a bucket (slot) is not empty
            printf("\n < Key:%d > : %s(%d)", i, symbolTable[i].slot->label, symbolTable[i].slot->address);
            pointer = symbolTable[i].slot->next;
            while (pointer != NULL) {                                                   // print any remaining elements of the linked list
                printf(" - %s(%d)", pointer->label, pointer->address);
                pointer = pointer->next;
            }
        }
      }
    printf("\n\n");
}

int main(){
    struct HashTable symbolTable[SIZE] = {0};                       // the symbol table
    int choice = 1, address;
    char label[5];
    printf("\n----------- MENU -----------");
    printf("\n 1. Insert into symbol table");
    printf("\n 2. Search in symbol table");
    printf("\n 0. Exit\n");
    while ( choice ){
        printf("\nEnter choice: ");
        scanf("%d", &choice);
        if ( choice == 1 ){                                         // insert
            printf("\nEnter the address: ");
            scanf("%d", &address);
            printf("Enter the label: ");
            scanf("%s", label);
            if (strlen(label) > 4){
                printf("\n[!] Label size should be less than or equal to 4.\n");
                continue;
            }
            insert(symbolTable, label, address);
        }
        else if ( choice == 2 ){                                    // lookup
            printf("\nEnter the label: ");
            scanf("%s", label);
            if (strlen(label) > 4){
                printf("\n[!] Label size should be less than or equal to 4.\n");
                continue;
            }
            lookup(symbolTable, label);
        }
        else if ( choice )                                          // invalid choice
            printf("\nInvalid choice. (Available options: 1, 2 and 0)\n");
    }
    display(symbolTable);                                           // dump the values in the hash table
}
