#include <stdio.h>
#include <string.h>

enum Operations{
    PUSH,
    ADD,
    SUB,
    DIV,
    MUL,
    OUT,
    DEFAULT,
    ERROR
};

#define FILE_TO_BE_TRANSLATION "test1.txt"
#define FILE_WITH_TRANSLATION_RESULT "code.txt"

void deleteLastEnter(char* str);
size_t countOfWords(const char* str);
void translate(const char* file_In, const char* file_Out);

void translate(const char* file_In, const char* file_Out){
    FILE* fileIn = fopen(file_In, "r");
    FILE* fileOut = fopen(file_Out, "w");

    assert(fileIn);
    assert(fileOut);

    while(true){
        char str[20] = {};
        fgets(str, sizeof(str), fileIn);
        deleteLastEnter(str);

        char operation[5] = {};
        Operations codeOfOperation = DEFAULT;
        double argument = 0;
        size_t countWord = countOfWords((const char*) str);

        if(countWord == 1){
            sscanf(str, "%s", operation);
        }
        else if(countWord == 2){
            sscanf(str, "%s %lf", operation, &argument);
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
        else if(strcmp(operation, "OUT") == 0){
            codeOfOperation = OUT;
            fprintf(fileOut, "%d", codeOfOperation);
            break;
        }

        fprintf(fileOut, "%d", codeOfOperation);

        if(countWord == 2){
            fprintf(fileOut, " %lf", argument);
        }
        
        fprintf(fileOut, "\n");
    }
    fclose(fileIn);
    fclose(fileOut);
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