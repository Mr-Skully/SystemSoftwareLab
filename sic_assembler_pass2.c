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
} textRecord[10];                                                   // a text-record struct that temporarily stores the machine code of upto 10 instructions

int optabLength = 0;                                                // current length of opcodes table
int symtabLength = 0;                                               // current length of symbols table

int searchOptab(char mnemonic[6]){                                  // search opcodes table for an opcode and return its value
    int i;
    for(i = 0; i < optabLength; i++){
        if (!strcmp(mnemonic, opcodes[i].mnemonic))
            return opcodes[i].value;
    }
    printf("\n[!] Undefined opcode: %s\n", mnemonic);
    return 0;
}
int searchSymtab(char symbol[21]) {                                 // search symbols table for a label and return its location
    int i;
    for(i = 0; i < symtabLength; i++){
        if (!strcmp(symbol, symtab[i].label))
            return symtab[i].loc;
    }
    printf("\n[!] Undefined symbol: %s\n", symbol);
    return 0;
}

int main(int argc, char const *argv[]) {
    int i, pgmLength, currentRecordSize = 0, recordBytes = 0;       // to store total program size, number of instructions in a text record, total bytes in that record, etc.
    char locCtr[5], label[21], opcode[6], operand[21];              // to store the extracted location, label, opcode and operand from each line of the input file
    char pgmStartLoc[5] = "0", recordStartLoc[5] = "0";             // default starting address of program and execution
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

    if(fscanf(input, "%s %s %s %s", locCtr, label, opcode, operand) != 4){              // read first line of input file and check whether it is formatted correctly
        printf("[!] Could not parse the input file. Make sure that it is the output file from a successful PASS1 execution.\n\n");
        exit(6);
    }
    if (!strcmp(opcode, "START")){                                  // if the first line is a START statement
        if(!strcmp(label, "-"))                                     // check whether a program name exists, use NULL if no name is present
            strcpy(label, "NULL");
        strcpy(pgmStartLoc, operand);                               // program starting location, to be used with the END record
        fprintf(objfile, "H^%-6.6s^%06.6d^%06.6d", label, atoi(operand), pgmLength);    // write the HEADER record
    }
    else{                                                           // when the first statement read was not a START directive
        fseek(input, 0, SEEK_SET);                                  // reset file pointer to the beginning of the file
        fprintf(objfile, "H^%-6.6s^%06.6d^%06.6d", "NULL", 0, pgmLength);               // write HEADER record using default values
    }
    while (!feof(input)){                                           // read the input file till EOF
        fscanf(input, "%s %s %s %s", locCtr, label, opcode, operand);
        if (strcmp(opcode, "END")){                                 // if the statement is not an END statement
            if (currentRecordSize == 0)
                strcpy(recordStartLoc, locCtr);                     // note the location of the first statement of a text record
            if(!strcmp(opcode, "BYTE")){                            // if the opcode is BYTE
                recordBytes += 1;                                   // increment size of the current text record
                textRecord[currentRecordSize].opcode = 0;
                if(operand[0] == 'X'){                              // convert hex operand (Eg. X'6A') to base10 (Eg. 106)
                    for(i = 0; i < strlen(operand) - 3; i++)        // using strlen() - 3, to account for the characters 'X' and the two single quotes
                        operand[i] = operand[i+2];
                    operand[i] = '\0';
                    textRecord[currentRecordSize].operand = (int)strtol(operand, NULL, 16);;
                }
                else{                                               // convert ascii character into a decimal number
                    textRecord[currentRecordSize].operand = (int)(operand[2]);
                }
            }
            else if(!strcmp(opcode, "WORD")){                       // if the opcode is WORD
                recordBytes += 3;                                   // increment size of the current text record
                textRecord[currentRecordSize].opcode = 0;
                textRecord[currentRecordSize].operand = atoi(operand);
            }
            else if(!strcmp(opcode, "RESB")){                       // no entry in text record for RESB, but the memory required is considered while calculating the record size
                recordBytes += atoi(operand);
                continue;
            }
            else if(!strcmp(opcode, "RESW")){                       // no entry in text record for RESW, but the memory required is considered while calculating the record size
                recordBytes += 3 * atoi(operand);
                continue;
            }
            else{                                                   // if the opcode is not any of the ones specified above
                recordBytes += 3;
                textRecord[currentRecordSize].opcode = searchOptab(opcode);             // search the optable for the machine code corresponidng to the opcode
                textRecord[currentRecordSize].operand = searchSymtab(operand);          // search the symbols table for the location of a symbol/label
            }
            currentRecordSize++;                                    // increment the number of instructions in a text record
            if (currentRecordSize == 10){                           // when the number of instructions in the record reaches 10, write the record to file, and prepare for a new record
                fprintf(objfile, "\nT^%06.6d^%02.2d", atoi(recordStartLoc), recordBytes);
                for(i = 0; i < currentRecordSize; i++)
                    fprintf(objfile, "^%02.2d%04.4d", textRecord[i].opcode, textRecord[i].operand);
                currentRecordSize = 0;
                recordBytes = 0;
            }
        }
        else{                                                       // when the opcode is END
            if (strcmp(operand, "-")){                              // if an operand exists
                i = searchSymtab(operand);                          // check for its value in the symbols table
                if(i)                                               // if it is found in symtab, set is as the program start location
                    itoa(i, pgmStartLoc, 10);
            }
            break;                                                  // stop processing when an END statement is encountered
        }
    }
    if (currentRecordSize > 0){                                     // write the remaining TEXT record to file
        fprintf(objfile, "\nT^%06.6d^%02.2d", atoi(recordStartLoc), recordBytes);
        for(i = 0; i < currentRecordSize; i++)
            fprintf(objfile, "^%02.2d%04.4d", textRecord[i].opcode, textRecord[i].operand);
    }
    fprintf(objfile, "\nE^%06.6d", atoi(pgmStartLoc));              // write the END record to file
    fclose(input);
    fclose(objfile);
    printf("\n[.] Wrote object code to output.obj\n\n");
    return 0;
}
