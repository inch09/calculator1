#include <stdio.h>
#include <string.h>
#include <math.h>
#include "operations.h"

struct Operation{
    TypeOfOperation type;
    double argument1;
    double argument2;
};


#define FILE_TO_BE_TRANSLATION "test6.txt"
#define FILE_WITH_TRANSLATION_RESULT "code.txt"

void deleteLastEnter(char* str);
size_t countOfWords(const char* str);
size_t countOfStringsInFile(FILE* file);
bool strIsNumber(const char* str);
Operation* translate(const char* file_In, const char* file_Out);

Operation* translate(const char* file_In, const char* file_Out){
    FILE* fileIn = fopen(file_In, "r");
    FILE* fileOut = fopen(file_Out, "w");


    assert(fileIn);
    assert(fileOut);

    size_t countOper = countOfStringsInFile(fileIn);
    Operation* arrOfOperations = (Operation*) calloc(countOper, sizeof(Operation));
    assert(arrOfOperations);
    size_t indOfOper = 0;

    while(true){


        Operation oper = {.type = DEFAULT, .argument1 = NAN, .argument2 = NAN};

        char str[20] = {};
        fgets(str, sizeof(str), fileIn);
        deleteLastEnter(str);

        char strArg1[15] = {}; 
        
        char operation[5] = {};
        TypeOfOperation codeOfOperation = DEFAULT;
        double argument1 = NAN;
        size_t countWord = countOfWords((const char*) str);

        if(countWord == 1){
            sscanf(str, "%s", operation);
        }
        else if(countWord == 2){
            sscanf(str, "%s %s", operation, strArg1);

            if(!strIsNumber(strArg1)){  

                char* strArgument1 = (char*) calloc(strlen(strArg1), sizeof(char));
                assert(strArgument1); 

                if(strcmp(strArgument1, "AX")){
                    argument1 = 1;
                }
                else if(strcmp(strArgument1, "BX")){
                    argument1 = 2;
                }
                else if(strcmp(strArgument1, "CX")){
                    argument1 = 3;
                }
                else if(strcmp(strArgument1, "DX")){
                    argument1 = 4;
                }

                free(strArgument1);
            }

            else{
                sscanf(str, "%s %lf", operation, &argument1);
            }

        }
        if(strcmp(operation, "PUSH") == 0){
            codeOfOperation = PUSH;
        }
        else if(strcmp(operation, "ADD") == 0){
            codeOfOperation = ADD;
        }
        else if(strcmp(operation, "SUB") == 0){
            codeOfOperation = SUB;
        }
        else if(strcmp(operation, "MUL") == 0){
            codeOfOperation = MUL;
        }
        else if(strcmp(operation, "DIV") == 0){
            codeOfOperation = DIV;
        }
        else if(strcmp(operation, "SIN") == 0){
            codeOfOperation = SIN;
        }
        else if(strcmp(operation, "SQRT") == 0){
            codeOfOperation = SQRT;
        }
        else if(strcmp(operation, "POPR") == 0){
            codeOfOperation = POPR;
        }
        else if(strcmp(operation, "PSHR") == 0){
            codeOfOperation = PSHR;
        }
        else if(strcmp(operation, "JMP") == 0){
            codeOfOperation = JMP;
        }
        else if(strcmp(operation, "OUT") == 0){
            codeOfOperation = OUT;
        }
        else if(strcmp(operation, "HLT") == 0){
            codeOfOperation = HLT;
            oper.type = HLT;
            fprintf(fileOut, "%d", codeOfOperation);
            break;
        }

        oper.type = codeOfOperation;
        oper.argument1 = argument1;

        fprintf(fileOut, "%d", codeOfOperation);

        if(countWord == 2){
            fprintf(fileOut, " %lf", argument1);
        }
        
        fprintf(fileOut, "\n");

        if(indOfOper < countOper){
            arrOfOperations[indOfOper] = oper;
            indOfOper++;
            //printf("%lu\n", (unsigned long) indOfOper);
            printf("%lu\n", (unsigned long) countOper);

        }
    }
    fclose(fileIn);
    fclose(fileOut);
    return arrOfOperations;
}

void deleteLastEnter(char* str){
    if(str[strlen(str) - 1] == '\n'){
        str[strlen(str)- 1] = '\0';
    }
}

size_t countOfWords(const char* str){
    size_t count = 1;
    for(size_t i = 0; i < strlen(str); i++){
        if(str[i] == ' '){
            count++;
        }
    }
    return count;
}

size_t countOfStringsInFile(FILE* file){
    assert(file);
    
    size_t count = 1;

    while(true){
        char symbol = fgetc(file);
        if(symbol == EOF){
            rewind(file);
            return count;
        }
        if(symbol == '\n'){
            count++;
        }
    }

    rewind(file);
    return count;
}

bool strIsNumber(const char* str){
    assert(str);

    size_t ind = 0;
    if(str[ind] == '-' /*|| str[ind] == "+"*/){
        ind++;
        if(strlen(str) == 1){
            return false;
        }
    }

    for(;ind < strlen(str); ind++){
        if(str[ind] >= '0' && str[ind] <= '9'){
            continue;
        }
        else{
            return false;
        }
    }

    return true;
}
