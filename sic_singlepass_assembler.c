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
int fwdRefLength = 0;                                               // length of forward references table

char * getOpcode(struct opcodeTable optab[], char mnemonic[5]){                         // return opcode from optab
    int i;
    for(i = 0; i < optabLength; i++)
        if (!strcmp(mnemonic, optab[i].mnemonic))
            return optab[i].value;
}
char * getSymcode(struct symbolsTable symtab[], char symbol[11]) {                      // return symbol value from symtab
    int i;
    for(i = 0; i < symtabLength; i++)
        if (!strcmp(symbol, symtab[i].label) && strcmp(symtab[i].loc, "*")!=0)
            return symtab[i].loc;
}

int checkOptab(struct opcodeTable optab[], char mnemonic[5]){                           // check whether an operation is present in optab
    int i;
    for(i = 0; i < optabLength; i++)
        if (!strcmp(mnemonic, optab[i].mnemonic))
            return 1;
    return 0;
}
int checkSymtab(struct symbolsTable symtab[], char symbol[11]) {                        // check whether a symbol is present in symtab
    int i;
    for(i = 0; i < symtabLength; i++)
        if (!strcmp(symbol, symtab[i].label) && strcmp(symtab[i].label, "*")!=0)
            return 1;
    return 0;
}

int main(){
    struct opcodeTable optab[10];                                   // to store opcodes temporarily
    struct symbolsTable symtab[10], forwardReferences[10];          // to store symbols and forward references temporarily
    struct TRecord textrec;                                         // to store each text record temporarily
    int locCtr = 0, startLoc = 0;
    char label[11], opcode[6], operand[11], temp[11];
    FILE *inputFile, *outputFile, *symtabFile, *optabFile, *resultFile;
    int i;
    inputFile = fopen("input.txt", "r");                            // open input file
    if(!inputFile){
        printf("[!] Could not open input.txt\n\n");
        exit(1);
    }
    optabFile = fopen("optab.txt", "r");                            // open optab file
    if(!optabFile){
        printf("[!] Could not open optab.txt\n\n");
        exit(1);
    }
    outputFile = fopen("output.txt", "w");                          // open output file
    symtabFile = fopen("symtab.txt", "w");                          // open symtab file
    resultFile = fopen("result.txt", "w");                          // open result file for object code

    while (fscanf(optabFile, "%s %s", optab[optabLength].mnemonic, optab[optabLength].value) == 2){ // read optab into memory
        optabLength++;
    }
    if (fscanf(inputFile, "%s %s %s", label, opcode, operand) != 3){    // read first line of input
        printf("\n[.] Incorrect format.\n\n");
        exit(1);
    }
    if(!strcmp(opcode, "START")){                                   // check whether it is a START directive
        locCtr = atoi(operand);
        startLoc = locCtr;
        if (strcmp(label, "-")!=0){                                 // add name of program to symtab, if available
            strcpy(symtab[symtabLength].label, label);
            itoa(locCtr, symtab[symtabLength].loc, 10);
            symtabLength++;
        }
        else                                                        // use NULL if no program name is specified
            strcpy(label, "NULL");
    }
    else{                                                           // if first line was not a START directive, go back to start of the file
        fseek(inputFile, 0, SEEK_SET);
    }
    fprintf(resultFile, "H^%-6.6s^%06.6d^000000", label, atoi(operand));    // print the HEADER record
    textrec.count = textrec.size = 0;
    textrec.start = locCtr;
    strcpy(textrec.record, "");
    while (fscanf(inputFile, "%s %s %s", label, opcode, operand) == 3){     // go through the input lines
        if(strcmp(opcode, "END")!=0){                                       // when the current line is not an END directive
            if (!strcmp(opcode, "WORD")){                                   // when opcode => WORD
                sprintf(temp, "%06.6d", atoi(operand));
                strcat(textrec.record, "^");
                strcat(textrec.record, temp);
                textrec.size += 3;
                textrec.count++;
                strcpy(symtab[symtabLength].label, label);          // add to symtab
                itoa(locCtr, symtab[symtabLength].loc, 10);
                symtabLength++;
                locCtr += 3;
                if(textrec.count == 10){                            // if there are 10 instructions in the record, write TEXT record to file
                    fprintf(resultFile, "\nT^%06.6d^%02x%s", textrec.start, textrec.size, textrec.record);
                    textrec.count = textrec.size = 0;
                    textrec.start = locCtr;
                    strcpy(textrec.record, "");
                }
            }
            else if (!strcmp(opcode, "BYTE")){                              // when opcode => BYTE
                sprintf(temp, "%02.2d", atoi(operand));
                strcat(textrec.record, "^");
                strcat(textrec.record, temp);
                textrec.size += 1;
                textrec.count++;
                strcpy(symtab[symtabLength].label, label);          // add to symtab
                itoa(locCtr, symtab[symtabLength].loc, 10);
                symtabLength++;
                locCtr++;
                if(textrec.count == 10){                            // if there are 10 instructions in the record, write TEXT record to file
                    fprintf(resultFile, "\nT^%06.6d^%02x%s", textrec.start, textrec.size, textrec.record);
                    textrec.count = textrec.size = 0;
                    textrec.start = locCtr;
                    strcpy(textrec.record, "");
                }
            }
            else if (!strcmp(opcode, "RESB")){                              // when opcode => RESB
                textrec.size += atoi(operand);
                textrec.count += atoi(operand);
                strcpy(symtab[symtabLength].label, label);          // add to symtab
                itoa(locCtr, symtab[symtabLength].loc, 10);
                symtabLength++;
                locCtr += atoi(operand);
                if(textrec.count >= 10){                            // if there are 10 instructions in the record, write TEXT record to file
                    fprintf(resultFile, "\nT^%06.6d^%02x%s", textrec.start, textrec.size, textrec.record);
                    textrec.count = textrec.size = 0;
                    textrec.start = locCtr;
                    strcpy(textrec.record, "");
                }
            }
            else if (!strcmp(opcode, "RESW")){                              // when opcode => RESW
                textrec.size += (atoi(operand) * 3);
                textrec.count += atoi(operand);
                strcpy(symtab[symtabLength].label, label);          // add to symtab
                itoa(locCtr, symtab[symtabLength].loc, 10);
                symtabLength++;
                locCtr += (3 * atoi(operand));
                if(textrec.count >= 10){                            // if there are 10 instructions in the record, write TEXT record to file
                    fprintf(resultFile, "\nT^%06.6d^%02x%s", textrec.start, textrec.size, textrec.record);
                    textrec.count = textrec.size = 0;
                    textrec.start = locCtr;
                    strcpy(textrec.record, "");
                }
            }
            else{                                                           // for all other opcodes
                if(strcmp(label, "-") != 0) {                       // if there is a label
                    if (checkSymtab(symtab, label)) {
                        printf("\n[.] Label \"%s\" already exists.\n\n", label);
                        exit(1);
                    }
                    strcpy(symtab[symtabLength].label, label);
                    itoa(locCtr, symtab[symtabLength].loc, 10);
                    symtabLength++;
                }
                if (!checkOptab(optab, opcode)){                    // check whether the operation is valid by checking the optab
                    printf("\n[.] Operation \"%s\" is invalid.\n\n", opcode);
                    exit(1);
                }
                strcat(textrec.record, "^");
                strcat(textrec.record, getOpcode(optab, opcode));   // convert the operation to opcode
                if(checkSymtab(symtab, operand))                    // when the operand is already defined
                    strcat(textrec.record, getSymcode(symtab, operand));
                else{                                               // when there is a forward reference, the location is stored in the forward references table for processing later
                    strcpy(symtab[symtabLength].label, operand);
                    strcpy(symtab[symtabLength].loc, "*");
                    symtabLength++;
                    fprintf(outputFile, "%s\t0\n", getOpcode(optab, opcode));
                    strcat(textrec.record, "0000");                 // "0000" is used in the text record to denote forward references
                    strcpy(forwardReferences[fwdRefLength].label, operand);
                    itoa(locCtr + 1, forwardReferences[fwdRefLength].loc, 10);
                    fwdRefLength++;
                }
                textrec.size += 3;
                locCtr += 3;
                if(textrec.count == 10){                            // if there are 10 instructions in the record, write TEXT record to file
                    fprintf(resultFile, "\nT^%06.6d^%02x%s", textrec.start, textrec.size, textrec.record);
                    textrec.count = textrec.size = 0;
                    textrec.start = locCtr;
                    strcpy(textrec.record, "");
                }
            }
        }
        else{                                                       // when an END directive is encountered
            if(textrec.count > 0)                                   // write the remaining TEXT record, if it exists
                fprintf(resultFile, "\nT^%06.6d^%02x%s", textrec.start, textrec.size, textrec.record);
            for(i = 0; i < fwdRefLength; i++){                      // resolve the forward references by cross-checking the symtab
                if (!checkSymtab(symtab, forwardReferences[i].label)){
                    printf("\n[.] Symbol \"%s\" is not declared.\n\n", forwardReferences[i].label);
                    exit(1);
                }
                fprintf(outputFile, "%s\t%s\n", forwardReferences[i].loc, getSymcode(symtab, forwardReferences[i].label));
                fprintf(resultFile, "\nT^%06.6d^02^%s", atoi(forwardReferences[i].loc), getSymcode(symtab, forwardReferences[i].label));
            }
            fprintf(resultFile, "\nE^%06.6d", startLoc);            // write the END record to file
            break;
        }
    }
    fseek(resultFile, 16, SEEK_SET);                                // move the file cursor to the point where the program size is stored in the HEADER record
    fprintf(resultFile, "%06x", locCtr - startLoc);                 // update the program size in the HEADER record
    for(i = 0; i < symtabLength; i++)                               // write symtab to file
        fprintf(symtabFile, "%s\t%s\n", symtab[i].label, symtab[i].loc);
    fclose(inputFile);
    fclose(outputFile);
    fclose(symtabFile);
    fclose(optabFile);
    fclose(resultFile);
    printf("\n[.] Successful.\n\n");
    return 0;
}
