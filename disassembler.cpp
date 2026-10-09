#include <stdio.h>
#include <string.h>

#define CONVERT_TO_STR(x) #x
#define FILE_TO_BE_DISASSEMBLING "code.txt"
#define FILE_WITH_DISASSEMBLING_RESULT "disasmcode.txt"

#define GET_OPERATION(operation)    \
    case operation:                 \
        strOperation = #operation;  \
        break;

// void deleteLastEnter(char* str);
// size_t countOfWords(const char* str);
void disAssembling(const char* file_In, const char* file_Out);

void disAssembling(const char* file_In, const char* file_Out){
    FILE* fileIn = fopen(file_In, "r");
    FILE* fileOut = fopen(file_Out, "w");
    assert(fileIn);
    assert(fileOut);
    while(true){
        char str[20] = {};
        fgets(str, sizeof(str), fileIn);
        deleteLastEnter(str);

        int codeOfOperation = -10;
        const char* strOperation = "";
        double argument = 0;

        size_t countWord = countOfWords((const char*) str);

        if(countWord == 1){
            sscanf(str, "%d", &codeOfOperation);
        }
        else if(countWord == 2){
            sscanf(str, "%d %lf", &codeOfOperation, &argument);
        }
        else{
            return;
        }

        switch(codeOfOperation){
            
            GET_OPERATION(PUSH);
            GET_OPERATION(ADD);
            GET_OPERATION(SUB);
            GET_OPERATION(DIV);
            GET_OPERATION(MUL);
            GET_OPERATION(OUT);
            GET_OPERATION(HLT);
            GET_OPERATION(DEFAULT);
            GET_OPERATION(ERROR);
            default:
                break;
        }

        if(countWord == 1){
            fprintf(fileOut, "%s", strOperation);
        }
        else if(countWord == 2){
            fprintf(fileOut, "%s %lf", strOperation, argument);
        }

        if(codeOfOperation == HLT){
            return;
        }
        fprintf(fileOut, "\n");
    }
    fclose(fileIn);
    fclose(fileOut);
}

#undef GET_OPERATION

// void deleteLastEnter(char* str){
//     if(str[strlen(str) - 1] == '\n'){
//         str[strlen(str)- 1] = '\0';
//     }
// }

// size_t countOfWords(const char* str){
//     size_t count = 1;
//     for(size_t i = 0; i < strlen(str); i++){
//         if(str[i] == ' '){
//             count++;
//         }
//     }
//     return count;
// }