/*
 *
 * This program assumes that:
 *     1) A preprocessor has processed the original source file, removed the comments,
 *         removed the empty lines, and replaced the blank columns (labels or operands) with a hyphen (-).
 *     2) The source file has been processed by PASS1 successfully (without any errors) and output.txt has been generated.
 * This program takes the output of PASS1 as the input file for generating object code, as the locations are already resolved.
 *
 */

#include <stdio.h>                                                  // I/O operations
#include <string.h>                                                 // string operations
#include <stdlib.h>                                                 // for atoi()

#define MAX_LABELS 10                                               // to set size of symbols table

struct opcodeTable{
    char mnemonic[6];
    int value;
} opcodes[30];                                                      // opcodes table

struct symbolsTable{
    char label[21];
    int loc;
} symtab[MAX_LABELS];                                               // symbols table

struct textRecordSections{
    int opcode;
    int operand;
} textRecord[10];

int optabLength = 0;                                                // current length of opcodes table
int symtabLength = 0;                                               // current length of symbols table

int searchOptab(char mnemonic[6]){                                  // search opcodes table for an opcode (returns 1 if found, 0 otherwise)
    int i;
    for(i = 0; i < optabLength; i++){
        if (!strcmp(mnemonic, opcodes[i].mnemonic))
            return opcodes[i].value;
    }
    printf("\n[!] Undefined opcode: %s\n", mnemonic);
    return 0;
}
int searchSymtab(char symbol[21]) {                                 // search symbols table for a label (returns 1 if found, 0 otherwise)
    int i;
    for(i = 0; i < symtabLength; i++){
        if (!strcmp(symbol, symtab[i].label))
            return symtab[i].loc;
    }
    printf("\n[!] Undefined symbol: %s\n", symbol);
    return 0;
}

int main(int argc, char const *argv[]) {
    int i, pgmLength, currentRecordSize = 0, recordBytes = 0;
    char locCtr[5], label[21], opcode[6], operand[21];             // to store the extracted location, label, opcode and operand from each line of the input file
    char pgmStartLoc[5] = "0", recordStartLoc[5] = "0";
    FILE *input, *optab, *objfile, *symfile, *lenfile;              // file pointers

    if(argc != 2){                                                  // provide usage hint when incorrect number of arguments is provided
        printf("\n[!] Incorrect usage.\n[!]   USAGE: ./pass2 PASS1_OUTPUT.txt\n\n");
        exit(1);
    }

    optab = fopen("optab.txt", "r");                                // open the optab file
    if(!optab){                                                     // if optab file cannot be opened
        printf("[!] Could not open optab.txt\n\n");
        exit(2);
    }
    while (fscanf(optab, "%s %d", opcodes[optabLength].mnemonic, &opcodes[optabLength].value) == 2)
        optabLength++;                                              // read the optab file and store in into the opcodes array
    fclose(optab);
    symfile = fopen("symtab.txt", "r");                             // open the symtab file
    if(!symfile){                                                   // if symtab file cannot be opened
        printf("[!] Could not open symtab.txt\n\n");
        exit(3);
    }
    while (fscanf(symfile, "%s %d", symtab[symtabLength].label, &symtab[symtabLength].loc) == 2)
        symtabLength++;                                              // read the symtab file and store in into the symtab array
    fclose(symfile);
    lenfile = fopen("length.txt", "r");                             // open the symtab file
    if(!lenfile){                                                   // if symtab file cannot be opened
        printf("[!] Could not open length.txt\n\n");
        exit(4);
    }
    fscanf(lenfile, "%d", &pgmLength) ;                             // read the length file and store in into the pgmLength variable
    fclose(lenfile);
    input = fopen(argv[1], "r");                                    // open the input file
    if(!input){                                                     // if input file cannot be opened
        printf("[!] Could not open %s\n\n", argv[1]);
        exit(5);
    }
    objfile = fopen("output.obj", "w");                             // open the output file

    if(fscanf(input, "%s %s %s %s", locCtr, label, opcode, operand) != 4){
        printf("[!] Could not parse the input file. Make sure that it is the output file from a successful PASS1 execution.\n\n");
        exit(6);
    }
    if (!strcmp(opcode, "START")){
        if(!strcmp(label, "-"))
            strcpy(label, "NULL");
        strcpy(pgmStartLoc, operand);
        fprintf(objfile, "H^%-6.6s^%06.6d^%06.6d", label, atoi(operand), pgmLength);
    }
    else{
        fseek(input, 0, SEEK_SET);
        fprintf(objfile, "H^%-6.6s^%06.6d^%06.6d", "NULL", 0, pgmLength);
    }
    while (!feof(input)){
        fscanf(input, "%s %s %s %s", locCtr, label, opcode, operand);
        if (strcmp(opcode, "END")){
            if (currentRecordSize == 0)
                strcpy(recordStartLoc, locCtr);
            if(!strcmp(opcode, "BYTE")){
                recordBytes += 1;
                textRecord[currentRecordSize].opcode = 0;
                if(operand[0] == 'X'){
                    for(i = 0; i < strlen(operand) - 3; i++)
                        operand[i] = operand[i+2];
                    operand[i] = '\0';
                    textRecord[currentRecordSize].operand = atoi(operand);
                }
                else{
                    textRecord[currentRecordSize].operand = (int)(operand[2]);
                }
            }
            else if(!strcmp(opcode, "WORD")){
                recordBytes += 3;
                textRecord[currentRecordSize].opcode = 0;
                textRecord[currentRecordSize].operand = atoi(operand);
            }
            else if(!strcmp(opcode, "RESB")){
                recordBytes += atoi(operand);
                continue;
            }
            else if(!strcmp(opcode, "RESW")){
                recordBytes += 3 * atoi(operand);
                continue;
            }
            else{
                recordBytes += 3;
                textRecord[currentRecordSize].opcode = searchOptab(opcode);
                textRecord[currentRecordSize].operand = searchSymtab(operand);
            }
            currentRecordSize++;
            if (currentRecordSize == 10){
                fprintf(objfile, "\nT^%06.6d^%02.2d", atoi(recordStartLoc), recordBytes);
                for(i = 0; i < currentRecordSize; i++)
                    fprintf(objfile, "^%02.2d%04.4d", textRecord[i].opcode, textRecord[i].operand);
                currentRecordSize = 0;
                recordBytes = 0;
            }
        }
        else{
            if (strcmp(operand, "-")){
                i = searchSymtab(operand);
                if(i)
                    itoa(i, pgmStartLoc, 10);
            }
            break;
        }
    }
    if (currentRecordSize > 0){
        fprintf(objfile, "\nT^%06.6d^%02.2d", atoi(recordStartLoc), recordBytes);
        for(i = 0; i < currentRecordSize; i++)
            fprintf(objfile, "^%02.2d%04.4d", textRecord[i].opcode, textRecord[i].operand);
    }
    fprintf(objfile, "\nE^%06.6d", atoi(pgmStartLoc));
    fclose(input);
    fclose(objfile);
    return 0;
}
