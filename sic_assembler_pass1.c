/*
 *
 * This program assumes that a preprocessor has processed the original source file, removed the comments,
 * removed the empty lines, and replaced the blank columns (labels or operands) with a hyphen (-).
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
    unsigned int loc;
} symtab[MAX_LABELS];                                               // symbols table
int optabLength = 0;                                                // current length of opcodes table
int symtabLength = 0;                                               // current length of symbols table

int searchOptab(char mnemonic[6]){                                  // search opcodes table for an opcode (returns 1 if found, 0 otherwise)
    int i;
    for(i = 0; i < optabLength; i++){
        if (!strcmp(mnemonic, opcodes[i].mnemonic))
            return 1;
    }
    return 0;
}
int searchSymtab(char symbol[21]) {                                 // search symbols table for a label (returns 1 if found, 0 otherwise)
    int i;
    for(i = 0; i < symtabLength; i++){
        if (!strcmp(symbol, symtab[i].label))
            return 1;
    }
    return 0;
}

int main(int argc, char const *argv[]) {
    int returnVal = 0;                                              // return value for PASS-1 (0 if it completes without any errors)
    int len;                                                        // to store length of operand
    int lineNum = 0, locCtr = 0, startLoc = 0;                      // to store line number, location counter and starting location
    char label[21], opcode[6], operand[21];                         // to store the extracted label, opcode and operand from each line of the input file
    FILE *input, *optab;                                            // file pointers

    optab = fopen(argv[2], "r");                                    // open the optab file
    while (fscanf(optab, "%s %d", opcodes[optabLength].mnemonic, &opcodes[optabLength].value) == 2)
        optabLength++;                                              // read the optab file and store in into the opcodes array

    input = fopen(argv[1], "r");                                    // open the input file
    while (++lineNum && fscanf(input, "%s %s %s", label, opcode, operand) == 3){        // read each line of the input file
        if (!strcmp(opcode, "START")){                              // if the opcode is START
            locCtr = atoi(operand);                                 // set location counter
            startLoc = atoi(operand);                               // set starting location
            if (strcmp(label, "-")!=0){                             // if label is not empty, store the label into the symtab array
                strcpy(symtab[symtabLength].label, label);
                symtab[symtabLength++].loc = locCtr;
            }
            printf("%d\t-\t%s\t%s\t%s\n", lineNum, label, opcode, operand);
        }
        else if (strcmp(opcode, "END")!=0){                         // if the opcode is not END
            printf("%d\t%d\t%s\t%s\t%s\n", lineNum, locCtr, label, opcode, operand);
            if(strcmp(label, "-")!=0){                              // if label is not empty
                if(!searchSymtab(label)){                           // if label is not in symtab, add to symtab
                    strcpy(symtab[symtabLength].label, label);
                    symtab[symtabLength++].loc = locCtr;
                }
                else{                                               // if label is already in symtab, report the error
                    printf("Error on line %d. Duplicate label found.\n", lineNum);
                    returnVal--;
                    continue;
                }
            }
            if (!strcmp(opcode, "RESW")){                           // if opcode is RESW
                locCtr += (atoi(operand) * 3);                      // increment location counter by the number of bytes required
            }
            else if (!strcmp(opcode, "RESB")){                      // if opcode is RESB
                locCtr += atoi(operand);                            // increment location counter by the number of bytes required
            }
            else if (!strcmp(opcode, "WORD")){                      // if opcode is WORD
                locCtr += 3;                                        // increment location counter by 3 bytes
            }
            else if (!strcmp(opcode, "BYTE")){                      // if opcode is BYTE
                len = strlen(operand);
                if (len > 3 && operand[0] == 'C' && operand[1] == '\'' && operand[len-1] == '\'')
                    locCtr += (len - 3);                            // increment location counter by length of string
                else if (len > 3 && operand[0] == 'X' && operand[1] == '\'' && operand[len-1] == '\'')
                    locCtr += 1;                                    // increment location counter by 1
                else{                                               // if the operand is neither of the form string nor hexadecimal value, report the error
                    printf("Warning: Unrecognized operand in BYTE declaration (line %d).\n", lineNum);
                    returnVal--;
                    locCtr += 1;                                    // assume only one byte is required and increment the location counter
                }
            }
            else if(searchOptab(opcode)){                           // search the optab for the opcode
                locCtr += 3;                                        // if found, increment location counter by 3 bytes
            }
            else{                                                   // if opcode is not found in optab, report error
                printf("Error on line %d.\n", lineNum);
                returnVal--;
                continue;
            }
        }
        else{                                                       // if opcode is END
            printf("%d\t%d\t%s\t%s\t%s\n", lineNum, locCtr, label, opcode, operand);
            if(strcmp(operand, "-")!=0){                            // if operand is not empty
                if(searchSymtab(operand))                           // if operand is found in symtab, set the operand as the starting location
                    startLoc = atoi(operand);
                else{                                               // if operand is not found in symtab, report error
                    printf("Error on line %d.\n", lineNum);
                    returnVal--;
                }
            }
            break;                                                  // stop reading the input file when the END opcode is encountered
        }
    }
    if(!feof(input)) {                                              // if loop was terminated before reaching the end-of-file, report error
        printf("Error on line %d. Invalid syntax.\n", lineNum);
        returnVal--;
    }
    fclose(optab);                                                  // close the open files
    fclose(input);
    printf("Length of the program: %d bytes\n", locCtr - startLoc); // write the length of input in bytes
    int i;
    for (i = 0; i < symtabLength; i++)
        printf("%s\t%d\n", symtab[i].label, symtab[i].loc);
    for (i = 0; i < optabLength; i++)
        printf("%s\t%d\n", opcodes[i].mnemonic , opcodes[i].value);
    return returnVal;
}
