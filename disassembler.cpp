#include <stdio.h>
#include <string.h>
#include "operations.h"

#define CONVERT_TO_STR(x) #x
#define FILE_TO_BE_DISASSEMBLING "code.txt"
#define FILE_WITH_DISASSEMBLING_RESULT "disasmcode.txt"

#define GET_OPERATION(operation)    \
    case operation:                 \
        strOperation = #operation;  \
        break;

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
            GET_OPERATION(SIN);
            GET_OPERATION(SQRT);
            GET_OPERATION(POPR);
            GET_OPERATION(PSHR);
            GET_OPERATION(JMP);
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
            if(codeOfOperation == PSHR || codeOfOperation == POPR){
                fprintf(fileOut, "%s ", strOperation);
                if(isEqual(argument, 1)){
                    fprintf(fileOut, "AX");
                }
                else if(isEqual(argument, 2)){
                    fprintf(fileOut, "BX");
                }
                else if(isEqual(argument, 3)){
                    fprintf(fileOut, "CX");
                }
                else if(isEqual(argument, 4)){
                    fprintf(fileOut, "DX");
                }
            }
            else{
                fprintf(fileOut, "%s %lf", strOperation, argument);
            }
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
