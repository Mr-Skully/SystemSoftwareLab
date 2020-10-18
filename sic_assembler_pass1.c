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
    FILE *input, *optab, *output, *symfile, *lenfile;               // file pointers

    if(argc != 3){                                                  // provide usage hint when incorrect number of arguments is provided
        printf("\nIncorrect usage.\n USAGE: ./pass1 INPUT_FILE.txt OPTAB_FILE.txt\n\n");
        exit(3);
    }

    optab = fopen(argv[2], "r");                                    // open the optab file
    if(!optab){                                                     // if optab file cannot be opened
        printf("Could not open %s.", argv[2]);
        exit(2);
    }
    while (fscanf(optab, "%s %d", opcodes[optabLength].mnemonic, &opcodes[optabLength].value) == 2)
        optabLength++;                                              // read the optab file and store in into the opcodes array

    input = fopen(argv[1], "r");                                    // open the input file
    if(!input){                                                     // if input file cannot be opened
        printf("Could not open %s.", argv[1]);
        exit(1);
    }

    output = fopen("output.txt", "w");                              // open the output file

    while (++lineNum && fscanf(input, "%s %s %s", label, opcode, operand) == 3){        // read each line of the input file
        if (!strcmp(opcode, "START")){                              // if the opcode is START
            locCtr = atoi(operand);                                 // set location counter
            startLoc = atoi(operand);                               // set starting location
            if (strcmp(label, "-")!=0){                             // if label is not empty, store the label into the symtab array
                strcpy(symtab[symtabLength].label, label);
                symtab[symtabLength++].loc = locCtr;
            }
            fprintf(output, "%-7s %-10s %-5s %s\n", "-", label, opcode, operand);
        }
        else if (strcmp(opcode, "END")!=0){                         // if the opcode is not END
            fprintf(output, "%-7d %-10s %-5s %s\n", locCtr, label, opcode, operand);
            if(strcmp(label, "-")!=0){                              // if label is not empty
                if(!searchSymtab(label)){                           // if label is not in symtab, add to symtab
                    strcpy(symtab[symtabLength].label, label);
                    symtab[symtabLength++].loc = locCtr;
                }
                else{                                               // if label is already in symtab, report the error
                    printf("[Line %d] Error: Duplicate label found - %s.\n", lineNum, label);
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
                    printf("[Line %d] Warning: Unrecognized operand in BYTE declaration - %s.\n", lineNum, operand);
                    returnVal--;
                    locCtr += 1;                                    // assume only one byte is required and increment the location counter
                }
            }
            else if(searchOptab(opcode)){                           // search the optab for the opcode
                locCtr += 3;                                        // if found, increment location counter by 3 bytes
            }
            else{                                                   // if opcode is not found in optab, report error
                printf("[Line %d] Error: Unrecognized operand - %s.\n", lineNum, operand);
                returnVal--;
                continue;
            }
        }
        else{                                                       // if opcode is END
            fprintf(output, "%-7d %-10s %-5s %s\n", locCtr, label, opcode, operand);
            if(strcmp(operand, "-")!=0){                            // if operand is not empty
                if(searchSymtab(operand))                           // if operand is found in symtab, set the operand as the starting location
                    startLoc = atoi(operand);
                else{                                               // if operand is not found in symtab, report error
                    printf("[Line %d] Error: Unrecognized label - %s\n", lineNum, label);
                    returnVal--;
                }
            }
            break;                                                  // stop reading the input file when the END opcode is encountered
        }
    }
    if(!feof(input)) {                                              // if loop was terminated before reaching the end-of-file, report error
        printf("[Line %d] Error: Invalid syntax. Line could not be processed.\n", lineNum);
        returnVal--;
    }
    fclose(optab);                                                  // close the optab file
    fclose(input);                                                  // close the input file
    fclose(output);                                                 // close the output file
    printf("[.] Wrote output to output.txt.\n");

    symfile = fopen("symtab.txt", "w");                             // open symbols table file
    int i;
    for (i = 0; i < symtabLength; i++)                              // write the symbols table to file
        fprintf(symfile, "%-10s%d\n", symtab[i].label, symtab[i].loc);
    fclose(symfile);                                                // close the symtab file
    printf("[.] Wrote symbols table to symtab.txt.\n");

    lenfile = fopen("length.txt", "w");                             // open the length file
    fprintf(lenfile, "%d", locCtr - startLoc);
    printf("\nLength of the program (also written to length.txt): %d bytes\n", locCtr - startLoc); // write the length of input in bytes
    fclose(lenfile);                                                // close the length file

    return returnVal;
}
