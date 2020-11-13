#include <stdio.h>                                                  // I/O operations
#include <string.h>                                                 // string operations
#include <stdlib.h>                                                 // for atoi()

struct opcodeTable{
    char mnemonic[5];
    char value[3];
};                                                                  // opcodes table

struct symbolsTable{
    char label[11];
    char loc[5];
};                                                                  // symbols table

struct TRecord{
    int start;
    int size;
    char record[71];
    int count;
};                                                  // a text-record struct that temporarily stores the machine code of upto 10 instructions

int optabLength = 0;                                                // current length of opcodes table
int symtabLength = 0;                                               // current length of symbols table
int fwdRefLength = 0;

char * getOpcode(struct opcodeTable optab[], char mnemonic[5]){                                  // search opcodes table for an opcode and return its value
    int i;
    for(i = 0; i < optabLength; i++)
        if (!strcmp(mnemonic, optab[i].mnemonic))
            return optab[i].value;
}
char * getSymcode(struct symbolsTable symtab[], char symbol[11]) {                                 // search symbols table for a label and return its location
    int i;
    for(i = 0; i < symtabLength; i++)
        if (!strcmp(symbol, symtab[i].label) && strcmp(symtab[i].loc, "*")!=0)
            return symtab[i].loc;
}

int checkOptab(struct opcodeTable optab[], char mnemonic[5]){                                  // search opcodes table for an opcode and return its value
    int i;
    for(i = 0; i < optabLength; i++)
        if (!strcmp(mnemonic, optab[i].mnemonic))
            return 1;
    return 0;
}
int checkSymtab(struct symbolsTable symtab[], char symbol[11]) {                                 // search symbols table for a label and return its location
    int i;
    for(i = 0; i < symtabLength; i++)
        if (!strcmp(symbol, symtab[i].label) && strcmp(symtab[i].label, "*")!=0)
            return 1;
    return 0;
}

int main(){
    struct opcodeTable optab[10];
    struct symbolsTable symtab[10], forwardReferences[10];
    struct TRecord textrec;
    int locCtr = 0, startLoc = 0;
    char label[11], opcode[6], operand[11], temp[11];
    FILE *inputFile, *outputFile, *symtabFile, *optabFile, *resultFile;
    int i;
    inputFile = fopen("input.txt", "r");
    if(!inputFile){
        printf("[!] Could not open input.txt\n\n");
        exit(1);
    }
    optabFile = fopen("optab.txt", "r");
    if(!optabFile){
        printf("[!] Could not open optab.txt\n\n");
        exit(1);
    }
    outputFile = fopen("output.txt", "w");
    symtabFile = fopen("symtab.txt", "w");
    resultFile = fopen("result.txt", "w");

    while (fscanf(optabFile, "%s %s", optab[optabLength].mnemonic, optab[optabLength].value) == 2){
        optabLength++;
    }
    if (fscanf(inputFile, "%s %s %s", label, opcode, operand) != 3){
        printf("\n[.] Incorrect format.\n\n");
        exit(1);
    }
    if(!strcmp(opcode, "START")){
        locCtr = atoi(operand);
        startLoc = locCtr;
        if (strcmp(label, "-")!=0){
            strcpy(symtab[symtabLength].label, label);
            itoa(locCtr, symtab[symtabLength].loc, 10);
            symtabLength++;
        }
        else
            strcpy(label, "NULL");
    }
    else{
        fseek(inputFile, 0, SEEK_SET);
    }
    fprintf(resultFile, "H^%-6.6s^%06.6d^000000", label, atoi(operand));
    textrec.count = textrec.size = 0;
    textrec.start = locCtr;
    strcpy(textrec.record, "");
    while (fscanf(inputFile, "%s %s %s", label, opcode, operand) == 3){
        if(strcmp(opcode, "END")!=0){
            if (!strcmp(opcode, "WORD")){
                sprintf(temp, "%06.6d", atoi(operand));
                strcat(textrec.record, "^");
                strcat(textrec.record, temp);
                textrec.size += 3;
                textrec.count++;
                strcpy(symtab[symtabLength].label, label);
                itoa(locCtr, symtab[symtabLength].loc, 10);
                symtabLength++;
                locCtr += 3;
                if(textrec.count == 10){
                    fprintf(resultFile, "\nT^%06.6d^%02x%s", textrec.start, textrec.size, textrec.record);
                    textrec.count = textrec.size = 0;
                    textrec.start = locCtr;
                    strcpy(textrec.record, "");
                }
            }
            else if (!strcmp(opcode, "BYTE")){
                sprintf(temp, "%02.2d", atoi(operand));
                strcat(textrec.record, "^");
                strcat(textrec.record, temp);
                textrec.size += 1;
                textrec.count++;
                strcpy(symtab[symtabLength].label, label);
                itoa(locCtr, symtab[symtabLength].loc, 10);
                symtabLength++;
                locCtr++;
                if(textrec.count == 10){
                    fprintf(resultFile, "\nT^%06.6d^%02x%s", textrec.start, textrec.size, textrec.record);
                    textrec.count = textrec.size = 0;
                    textrec.start = locCtr;
                    strcpy(textrec.record, "");
                }
            }
            else if (!strcmp(opcode, "RESB")){
                textrec.size += atoi(operand);
                textrec.count += atoi(operand);
                strcpy(symtab[symtabLength].label, label);
                itoa(locCtr, symtab[symtabLength].loc, 10);
                symtabLength++;
                locCtr += atoi(operand);
                if(textrec.count >= 10){
                    fprintf(resultFile, "\nT^%06.6d^%02x%s", textrec.start, textrec.size, textrec.record);
                    textrec.count = textrec.size = 0;
                    textrec.start = locCtr;
                    strcpy(textrec.record, "");
                }
            }
            else if (!strcmp(opcode, "RESW")){
                textrec.size += (atoi(operand) * 3);
                textrec.count += atoi(operand);
                strcpy(symtab[symtabLength].label, label);
                itoa(locCtr, symtab[symtabLength].loc, 10);
                symtabLength++;
                locCtr += (3 * atoi(operand));
                if(textrec.count >= 10){
                    fprintf(resultFile, "\nT^%06.6d^%02x%s", textrec.start, textrec.size, textrec.record);
                    textrec.count = textrec.size = 0;
                    textrec.start = locCtr;
                    strcpy(textrec.record, "");
                }
            }
            else{
                if(strcmp(label, "-") != 0) {
                    if (checkSymtab(symtab, label)) {
                        printf("\n[.] Label \"%s\" already exists.\n\n", label);
                        exit(1);
                    }
                    strcpy(symtab[symtabLength].label, label);
                    itoa(locCtr, symtab[symtabLength].loc, 10);
                    symtabLength++;
                }
                if (!checkOptab(optab, opcode)){
                    printf("\n[.] Operation \"%s\" is invalid.\n\n", opcode);
                    exit(1);
                }
                strcat(textrec.record, "^");
                strcat(textrec.record, getOpcode(optab, opcode));
                if(checkSymtab(symtab, operand))
                    strcat(textrec.record, getSymcode(symtab, operand));
                else{
                    strcpy(symtab[symtabLength].label, operand);
                    strcpy(symtab[symtabLength].loc, "*");
                    symtabLength++;
                    fprintf(outputFile, "%s\t0\n", getOpcode(optab, opcode));
                    strcat(textrec.record, "0000");
                    strcpy(forwardReferences[fwdRefLength].label, operand);
                    itoa(locCtr + 1, forwardReferences[fwdRefLength].loc, 10);
                    fwdRefLength++;
                }
                locCtr += 3;
                if(textrec.count == 10){
                    fprintf(resultFile, "\nT^%06.6d^%02x%s", textrec.start, textrec.size, textrec.record);
                    textrec.count = textrec.size = 0;
                    textrec.start = locCtr;
                    strcpy(textrec.record, "");
                }
            }
        }
        else{
            if(textrec.count > 0)
                fprintf(resultFile, "\nT^%06.6d^%02x%s", textrec.start, textrec.size, textrec.record);
            for(i = 0; i < fwdRefLength; i++){
                if (!checkSymtab(symtab, forwardReferences[i].label)){
                    printf("\n[.] Symbol \"%s\" is not declared.\n\n", forwardReferences[i].label);
                    exit(1);
                }
                fprintf(outputFile, "%s\t%s\n", forwardReferences[i].loc, getSymcode(symtab, forwardReferences[i].label));
                fprintf(resultFile, "\nT^%06.6d^02^%s", atoi(forwardReferences[i].loc), getSymcode(symtab, forwardReferences[i].label));
            }
            fprintf(resultFile, "\nE^%06.6d", startLoc);
            break;
        }
    }
    fseek(resultFile, 16, SEEK_SET);
    fprintf(resultFile, "%06x", locCtr - startLoc);
    for(i = 0; i < symtabLength; i++)
        fprintf(symtabFile, "%s\t%s\n", symtab[i].label, symtab[i].loc);
    fclose(inputFile);
    fclose(outputFile);
    fclose(symtabFile);
    fclose(optabFile);
    fclose(resultFile);
    printf("\n[.] Successful.\n\n");
    return 0;
}
