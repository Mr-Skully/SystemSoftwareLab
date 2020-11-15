/*
 * 0000 cannot be used stored in the hash table, as it is the default state of each slot.
 * Labels should have a maximum size of only 4 characters.
*/

#include <stdio.h>
#include <string.h>
#define SIZE 10

void lookup(int hashTable[], char label[]){
    int i, key = (int)label[0];
    for(i = 1; i < strlen(label); i++){
        key <<= 7;
        key |= (int)label[i];
    }
    key %= SIZE;
    if (hashTable[key])
        printf("\n Key: %d\n Label: %s\n Address: %d\n", key, label, hashTable[key]);
    else
        printf("\n Symbol not found in the symbol table.\n");
}

void insert(int hashTable[], char label[], int address) {
    int i, key = (int) label[0];
    for (i = 1; i < strlen(label); i++) {
        key <<= 7;
        key |= (int) label[i];
    }
    key %= SIZE;
    if (hashTable[key])
        printf("\n[!] Hash Collision Detected. Could not update symbol table.\n");
    else {
        hashTable[key] = address;
        printf("\n Inserted \'%s\' with key <%d> \n", label, key);
    }
}


void display(int hashTable[]){
    int i;
    printf("\n--- Hash Table Dump ---\n");
    for(i = 0; i < SIZE; i++){
        if(hashTable[i])
              printf("  < Key:%d > : %d\n", i, hashTable[i]);
      }
    printf("\n");
}

int main(){
    int hashTable[SIZE] = {0};
    int choice = 1, address;
    char label[5];
    printf("\n----------- MENU -----------");
    printf("\n 1. Insert into symbol table");
    printf("\n 2. Search in symbol table");
    printf("\n 0. Exit\n");
    while ( choice ){
        printf("\nEnter choice: ");
        scanf("%d", &choice);
        if ( choice == 1 ){
            printf("\nEnter the address: ");
            scanf("%d", &address);
            printf("Enter the label: ");
            scanf("%s", label);
            if (strlen(label) > 4){
                printf("\n[!] Label size should be less than or equal to 4.\n");
                continue;
            }
            insert(hashTable, label, address);
        }
        else if ( choice == 2 ){
            printf("\nEnter the label: ");
            scanf("%s", label);
            if (strlen(label) > 4){
                printf("\n[!] Label size should be less than or equal to 4.\n");
                continue;
            }
            lookup(hashTable, label);
        }
        else if ( choice )
            printf("\nInvalid choice. (Available options: 1, 2 and 0)\n");
    }
    display(hashTable);
}


