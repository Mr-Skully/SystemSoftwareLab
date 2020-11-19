#include <stdio.h>
#include <string.h>
#include <stdlib.h>

FILE *input, *output, *ntab, *dtab, *atab;

int checkNTab(char name[10]){
    char entry[10];
    fseek(ntab, 0, SEEK_SET);
    while(!feof(ntab)){
        fscanf(ntab, "%s", entry);
        if(!strcmp(entry, name))
            return 1;
    }
    return 0;
}

void expand(char name[], char arguments[]){
    char placeholder[5][10], arg[5][10], opcode[10], operand[10];
    int i = 0, j = 0, args = 0;
    while(arguments[i] != '\0'){
        if (arguments[i] != ',')
            arg[args][j++] = arguments[i];
        else{
            arg[args++][j] = '\0';
            fprintf(atab, "%s\n", arg[args-1]);
            j = 0;
        }
        i++;
    }
    arg[args++][j] = '\0';
    fprintf(atab, "%s\n", arg[args-1]);
    fseek(dtab, 0, SEEK_SET);
    while(!feof(dtab)){
        fscanf(dtab, "%s\t%s", opcode, operand);
        if(!strcmp(opcode, name)){
            fprintf(output, ".\t%s\t%s\n", name, arguments);
            i = j = args = 0;
            while(operand[i] != '\0'){
                if (operand[i] != ',')
                    placeholder[args][j++] = operand[i];
                else{
                    placeholder[args++][j] = '\0';
                    j = 0;
                }
                i++;
            }
            placeholder[args++][j] = '\0';
            break;
        }
    }
    while (fscanf(dtab, "%s\t%s", opcode, operand) == 2 && strcmp(opcode, "MEND") != 0){
        for(i = 0; i < args; i++){
            if(!strcmp(operand, placeholder[i]))
                break;
        }
        if(i == args)
            fprintf(output, "-\t%s\t%s\n", opcode, operand);
        else
            fprintf(output, "-\t%s\t%s\n", opcode, arg[i]);
    }
}

void define(char name[10], char args[10]){
    char label[10], opcode[10], operand[10];
    fprintf(ntab, "%s\n", name);
    fprintf(dtab, "%s\t%s\n", name, args);
    while(fscanf(input, "%s %s %s", label, opcode, operand) == 3){
        fprintf(dtab, "%s\t%s\n", opcode, operand);
        if(!strcmp(opcode, "MEND"))
            break;
    }
    if(strcmp(opcode, "MEND") != 0)
        printf("\n[!] Error while processing: Label = %s, Opcode = %s, Operand = %s\n", label, opcode, operand);
}

void pass1() {
    char label[10], opcode[10], operand[10];
    fseek(input, 0, SEEK_SET);
    while (fscanf(input, "%s %s %s", label, opcode, operand) == 3) {
        if (!strcmp(opcode, "MACRO"))
            define(label, operand);
        else if (!strcmp(opcode, "END"))
            return;
    }
    if(!feof(input)) {
        printf("\n[!] Error while processing: Label = %s, Opcode = %s, Operand = %s\n", label, opcode, operand);
        exit(1);
    }
}

void pass2(){
    char label[10], opcode[10], operand[10];
    fseek(input, 0, SEEK_SET);
    while (fscanf(input, "%s %s %s", label, opcode, operand) == 3) {
        if(!strcmp(opcode, "MACRO"))
            while (fscanf(input, "%s %s %s", label, opcode, operand) == 3 && strcmp(opcode, "MEND") != 0);
        else if(checkNTab(opcode))
            expand(opcode, operand);
        else if(strcmp(opcode, "END") != 0)
            fprintf(output, "%s\t%s\t%s\n", label, opcode, operand);
        else{
            fprintf(output, "%s\t%s\t%s\n", label, opcode, operand);
            return;
        }
    }
}

int main(int argc, char const *argv[]) {
    input = fopen("minp2.txt", "r");
    if(!input){
        printf("\n[!] Couldn't open minp2.txt\n\n");
        exit(1);
    }
    ntab = fopen("ntab2.txt", "w+");
    dtab = fopen("dtab2.txt", "w+");
    atab = fopen("atab2.txt", "w+");
    output = fopen("op2.txt", "w");
    pass1();
    pass2();
    printf("\n[*] Successful. Output written to \'op2.txt\'.\n\n");
    fclose(input);
    fclose(output);
    fclose(ntab);
    fclose(dtab);
    fclose(atab);
}
