#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "operations.h"


#define FILE_TO_BE_TRANSLATION "test13.txt"
#define FILE_WITH_TRANSLATION_RESULT "code.txt"

void deleteLastEnter(char* str);
size_t countOfWords(const char* str);
size_t countOfStringsInFile(FILE* file);
bool strIsNumber(const char* str);
void strCpyWithSameLength(char* strCopyTo, char* strCopyFrom);
int getNumOfTag(const char* operationWithTag);
void assemblingJumps(const char* operation, int* argument, TypeOfOperation* typeOp, int* arrOfTags);

int main(){
    int arrOfTags[20] = {};

    int indOfOper = 0;
    int indOfOperInCode = 0;
    FILE* fileIn = fopen(FILE_TO_BE_TRANSLATION, "r");
    FILE* fileOut = fopen(FILE_WITH_TRANSLATION_RESULT, "w");

    assert(fileIn);
    assert(fileOut);

    while(true){
        char str[20] = {};
        char* strptr = fgets(str, sizeof(str), fileIn);
        if(strptr == NULL){
            indOfOperInCode = 0;
            break;
        }
        deleteLastEnter(str);

        int argument1 = 0;

        if(str[0] == ':'){
            argument1 = getNumOfTag(str);
            arrOfTags[argument1 - 1] = indOfOperInCode + 2;  
            indOfOperInCode += 2;
            continue;      
        }

        size_t countWord = countOfWords((const char*) str);
        if(countWord == 1){
            indOfOperInCode++;
        }
        else if(countWord == 2){
            indOfOperInCode += 2;
        }
    }

        for(size_t i = 0; i < 20; i++){
        printf("%d ", arrOfTags[i]);
    }

        rewind(fileIn);

        while(true){


        char str[20] = {};
        char* strptr = fgets(str, sizeof(str), fileIn);
        if(strptr == NULL){
            indOfOper = 0;
            break;
        }
        deleteLastEnter(str);

        char strArg1[15] = {}; 
        
        char operation[5] = {};
        TypeOfOperation codeOfOperation = DEFAULT;
        int argument1 = 0;
        size_t countWord = countOfWords((const char*) str);

        if(countWord == 1){
            sscanf(str, "%s", operation);
        }
        else if(countWord == 2){
            sscanf(str, "%s %s", operation, strArg1);

            if(!strIsNumber(strArg1)){  

                char* strArgument1 = (char*) calloc(strlen(strArg1) + 1, sizeof(char));
                assert(strArgument1); 
                strCpyWithSameLength(strArgument1, strArg1);
                //printf("str arg1 = %s, strlen1 = %d\n", strArgument1, strlen(strArgument1));
                if(strcmp(strArgument1, "AX") == 0){
                    argument1 = 1;
                }
                else if(strcmp(strArgument1, "BX") == 0){
                    argument1 = 2;
                }
                else if(strcmp(strArgument1, "CX") == 0){
                    argument1 = 3;
                }
                else if(strcmp(strArgument1, "DX") == 0){
                    argument1 = 4;
                }
                // if(strcmp(operation, "POPR")==0 || strcmp(operation, "PSHR") == 0){
                // printf("%s", operation);
                // }
                free(strArgument1);
            }

            else{
                sscanf(str, "%s %d", operation, &argument1);
            }

        }

        assemblingJumps(operation, &argument1, &codeOfOperation, arrOfTags);

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
            // printf("POPR");
            // printf("%lf", argument1);
            codeOfOperation = POPR;
        }
        else if(strcmp(operation, "PSHR") == 0){
            codeOfOperation = PSHR;
        }
        else if(strcmp(operation, "IN") == 0){
            codeOfOperation = IN;
        }
        else if(strcmp(operation, "OUT") == 0){
            codeOfOperation = OUT;
        }
        else if(strcmp(operation, "HLT") == 0){
            codeOfOperation = HLT;
            // indOfOper = 0;
            // fprintf(fileOut, "%d", codeOfOperation);
            // break;
        }
        else if(operation[0] == ':'){
            //printf("oper = %s\n", operation);
            codeOfOperation = TAG;
            argument1 = getNumOfTag(str);
            //printf("getN = %d\n", getNumOfTag(operation));
            //argument1 = arrOfTags[getNumOfTag(operation) - 1];        
        }
        else if(strcmp(operation, "CALL") == 0){
            codeOfOperation = CALL;
            argument1 = arrOfTags[argument1 - 1];
        }
        else if(strcmp(operation, "RET") == 0){
            codeOfOperation = RET;
        }


        fprintf(fileOut, "%d", codeOfOperation);

        if(countWord == 2 || codeOfOperation == TAG){
            fprintf(fileOut, " %d", argument1);
        }
        
        fprintf(fileOut, "\n");
        indOfOper++;
    }

    fclose(fileIn);
    fclose(fileOut);

    for(size_t i = 0; i < 20; i++){
        printf("%d ", arrOfTags[i]);
    }
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

void strCpyWithSameLength(char* strCopyTo, char* strCopyFrom){
    for(size_t i = 0; i < strlen(strCopyFrom); i++){
        strCopyTo[i] = strCopyFrom[i];
    }
    assert(strlen(strCopyFrom) == strlen(strCopyTo));
}

int getNumOfTag(const char* operationWithTag){
    int numOfTag = -1;
    // printf("%d, %s\n", strlen(operationWithTag) ,operationWithTag);
    char* strNumOfTag = (char*) calloc(strlen(operationWithTag), sizeof(char));
    //printf("strlen  = %d\n", strlen(strNumOfTag));
    assert(strNumOfTag);
    for(size_t i = 0; i < strlen(operationWithTag) - 1; i++){
        //printf("uraaa\n");
        assert(i + 1 < strlen(operationWithTag));
        assert(operationWithTag[i + 1] >= '0' && operationWithTag[i + 1] <= '9');
        strNumOfTag[i] = operationWithTag[i + 1];
    }
    int c = sscanf(strNumOfTag, "%d", &numOfTag);
    free(strNumOfTag);
    assert(c);
    //printf("numoftg = %d\n", numOfTag);
    return numOfTag;
}

void assemblingJumps(const char* operation, int* argument, TypeOfOperation* typeOp, int* arrOfTags){
    assert(operation);
    assert(argument);
    assert(typeOp);
    assert(arrOfTags);    
    
    if(strcmp(operation, "JMP") == 0){
        *typeOp = JMP;
    }
    else if(strcmp(operation, "JA") == 0){
        *typeOp = JA;
    }
    else if(strcmp(operation, "JAE") == 0){
        *typeOp = JAE;
    }
    else if(strcmp(operation, "JB") == 0){
        *typeOp = JB;
    }
    else if(strcmp(operation, "JBE") == 0){
        *typeOp = JBE;
    }
    else if(strcmp(operation, "JE") == 0){
        *typeOp = JE;
    }
    else if(strcmp(operation, "JNE") == 0){
        *typeOp = JNE;
    }
    else{
        return;
    }

    *argument = arrOfTags[*argument - 1];

}