#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "operations.h"

struct Tag{
    char nameOfTag[10]; 
    size_t indOfOper;
}

#define FILE_TO_BE_TRANSLATION "test13.txt"//comand string
#define FILE_WITH_TRANSLATION_RESULT "code.txt"

void firstAssemPass(FILE* fileIn, int* arrOfTags);
void secondAssemPass(FILE* fileIn, FILE* fileOut, int* arrOfTags);

void deleteLastEnter(char* str);

size_t countOfWords(const char* str);
size_t countOfStringsInFile(FILE* file);
bool strIsNumber(const char* str);
void strCpyWithSameLength(char* strCopyTo, char* strCopyFrom);

int getNumOfTag(const char* operationWithTag);
void assemblingJumps(const char* operation, int* argument, TypeOfOperation* typeOp, int* arrOfTags);

int main(){
    Tag arrOfTags[20] = {};

    FILE* fileIn = fopen(FILE_TO_BE_TRANSLATION, "r");
    FILE* fileOut = fopen(FILE_WITH_TRANSLATION_RESULT, "w");

    assert(fileIn);
    assert(fileOut);

    firstAssemPass(fileIn, arrOfTags);
    rewind(fileIn);

    // for(size_t i = 0; i < 20; i++){
    //     printf("%d ", arrOfTags[i]);
    // }

    secondAssemPass(fileIn, fileOut, arrOfTags);
        
    fclose(fileIn);
    fclose(fileOut);

    // for(size_t i = 0; i < 20; i++){
    //     printf("%d ", arrOfTags[i]);
    // }
}

void deleteLastEnter(char* str){
    size_t lenOfStr = strlen(str);
    if (str[lenOfStr - 1] == '\n'){
        str[lenOfStr- 1] = '\0';
    }
}

size_t countOfWords(const char* str){
    size_t count = 1;
    size_t lenOfStr = strlen(str);

    for(size_t i = 0; i < lenOfStr; i++){
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
        if(str[1] == '\0'){
            return false;
        }
    }

    size_t lenOfStr = strlen(str);
    for(;ind < lenOfStr; ind++){
        if(str[ind] >= '0' && str[ind] <= '9'){
            continue;
        }
        else{
            return false;
        }
    }

    return true;
}

//strlen fix
void strCpyWithSameLength(char* strCopyTo, char* strCopyFrom){
    assert(strCopyTo);
    assert(strCopyFrom);

    size_t strCopyFromLen = strlen(strCopyFrom);
    size_t strCopyToLen = strlen(strCopyTo);

    for(size_t i = 0; i < strCopyFromLen; i++){
        strCopyTo[i] = strCopyFrom[i];
    }
    assert(strCopyFromLen == strCopyToLen);
}

int getNumOfTag(const char* operationWithTag){
    int numOfTag = -1;

    char* strNumOfTag = (char*) calloc(strlen(operationWithTag), sizeof(char));
    assert(strNumOfTag);

    size_t operationWithTagLen = strlen(operationWithTag);

    for(size_t i = 0; i < operationWithTagLen - 1; i++){
        assert(i + 1 < operationWithTagLen);
        assert(operationWithTag[i + 1] >= '0' && operationWithTag[i + 1] <= '9');

        strNumOfTag[i] = operationWithTag[i + 1];
    }

    int c = sscanf(strNumOfTag, "%d", &numOfTag);
    free(strNumOfTag);
    assert(c);

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

void firstAssemPass(FILE* fileIn, int* arrOfTags){
    assert(fileIn);

    int indOfOperInCode = 0;

    while(true){
    char str[20] = {};
        char* strptr = fgets(str, sizeof(str), fileIn);
        if(strptr == NULL){
            indOfOperInCode = 0;
            break;
        }
        deleteLastEnter(str);

        int argument1 = 0;

        if(str[0] == ':'){//func with tag
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
}

void secondAssemPass(FILE* fileIn, FILE* fileOut, int* arrOfTags){
    int indOfOper = 0;
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
}