#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "operations.h"

#define FILE_TO_BE_TRANSLATION "factorial.txt"//comand string
#define FILE_WITH_TRANSLATION_RESULT "code.txt"

#define MAX_COUNT_OF_TAGS 20
#define MAX_LENGTH_OF_TAG 20

struct Tag{
    char name[MAX_LENGTH_OF_TAG + 1]; 
    size_t indOfOper;
};

void firstAssemPass(FILE* fileIn, Tag* arrOfTags);
void secondAssemPass(FILE* fileIn, FILE* fileOut, Tag* arrOfTags);

void processOperWithArg(const char* stringOper, int* argument, char* operation, int* typeOfOper);

void deleteLastEnter(char* str);

size_t countOfWords(const char* str);
size_t countOfStringsInFile(FILE* file);
bool strIsNumber(const char* str);
void strCpyWithSameLength(char* strCopyTo, char* strCopyFrom);
bool strIsWithoutOperations(const char* str);

void getNameOfTag(const char* operationWithTag, char* nameOfTag);
int getIndOfTagByName(Tag* arrOfTags, const char* nameTag);

void assembleCommandWithBranches(const char* stringOper, int* argument, char* operation, int* typeOfOper);
void assembleJumps(const char* operation, int* argument, TypeOfOperation* typeOp, Tag* arrOfTags, char* operStr);
void assembleFuncWithoutJmp(const char* operation, TypeOfOperation* codeOfOp);
void assembleTag(int* argument, TypeOfOperation* codeOfOp, const char* str, Tag* arrOfTags);
void assemblePushAndPop(const char* stringArg, int* typeOfOper, int* argument);
bool isCommandWithBranches(const char* operation);

void printComandInFile(int countWord, int argument, int* codeOfOp, FILE* fileOut);
//-------------------------------------------------------------------------
int main(int argc, char* argv[]){
    char fileToTrans[30] = "";
    if(argc > 1){
        strcpy(fileToTrans, argv[1]);
    }
    else{
        strcpy(fileToTrans, FILE_TO_BE_TRANSLATION);
    }


    Tag arrOfTags[MAX_COUNT_OF_TAGS] = {};

    FILE* fileIn = fopen(fileToTrans, "r");
    FILE* fileOut = fopen(FILE_WITH_TRANSLATION_RESULT, "w");

    assert(fileIn);
    assert(fileOut);

    firstAssemPass(fileIn, arrOfTags);
    rewind(fileIn);

    for(size_t i = 0; i < MAX_COUNT_OF_TAGS; i++){
        printf("ind = %d, name = %s\n", arrOfTags[i].indOfOper, arrOfTags[i].name);
    }

    secondAssemPass(fileIn, fileOut, arrOfTags);
        
    fclose(fileIn);
    fclose(fileOut);

    // for(size_t i = 0; i < 20; i++){
    //     printf("%d ", arrOfTags[i]);
    // }
}
//-------------------------------------------------------------------------
void firstAssemPass(FILE* fileIn, Tag* arrOfTags){
    assert(fileIn);
    assert(arrOfTags);

    int indOfOperInCode = 0;
    int indOfTag = 0;

    while(true){

        const int maxLengthOfStrWithOper = 30; 
        char str[maxLengthOfStrWithOper] = {};
        char* strptr = fgets(str, sizeof(str), fileIn);
        if(strptr == NULL){
            indOfOperInCode = 0;
            break;
        }
        deleteLastEnter(str);

        if(strIsWithoutOperations(str)){
            continue;
        }

        if(str[0] == ':'){//func with tag
            getNameOfTag(str, arrOfTags[indOfTag].name);
            arrOfTags[indOfTag].indOfOper = indOfOperInCode + 2;  
            indOfOperInCode += 2;
            indOfTag++;
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
//-------------------------------------------------------------------------
void secondAssemPass(FILE* fileIn, FILE* fileOut, Tag* arrOfTags){
    assert(fileIn);
    assert(fileOut);
    assert(arrOfTags);

    int indOfOper = 0;
       
    while(true){
        const int maxLengthOfStrWithOper = 30; 
        char str[maxLengthOfStrWithOper] = {};
        char* strptr = fgets(str, sizeof(str), fileIn);
        if(strptr == NULL){
            indOfOper = 0;
            break;
        }
        deleteLastEnter(str);

        if(strIsWithoutOperations(str)){
            continue;
        }

        const int maxLengthOfOper = 31;
        char operation[maxLengthOfOper] = {};
        int argument = 0;
        int typeOfOp = 0;
        TypeOfOperation codeOfOperation = DEFAULT;
        
        size_t countWord = countOfWords(str);
        printf("str = %s, countword = %d\n", str, countWord);
        if(countWord == 1){
            sscanf(str, "%s", operation);
        }
        else if(countWord == 2){
            processOperWithArg(str, &argument, operation, &typeOfOp);
        }

        assembleJumps(operation, &argument, &codeOfOperation, arrOfTags, str);
        assembleFuncWithoutJmp(operation, &codeOfOperation);
        assembleTag(&argument, &codeOfOperation, str, arrOfTags);

        if(codeOfOperation != DEFAULT){
            typeOfOp = codeOfOperation;
        }
        printComandInFile(countWord, argument, &typeOfOp, fileOut);

        indOfOper++;
    }
}
//-------------------------------------------------------------------------
void deleteLastEnter(char* str){
    size_t lenOfStr = strlen(str);
    if (str[lenOfStr - 1] == '\n'){
        str[lenOfStr- 1] = '\0';
    }
}
//-------------------------------------------------------------------------
size_t countOfWords(const char* str){
    size_t count = 0;
    if((str[0] != ' ') && (str[0] != '\n') && (str[0] != '\0')){
        count++;
    }
    size_t lenOfStr = strlen(str);

    for(size_t i = 0; i < lenOfStr; i++){
        if(str[i] == ' '){
            size_t t = i;
            while(t < lenOfStr){
                if((str[t] != ' ') && (str[t] != '\n') && (str[t] != '\0')){
                    i = t;
                    count++;
                    break;
                }
                t++;
            }
        }
    }
    return count;
}
//-------------------------------------------------------------------------
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
//-------------------------------------------------------------------------
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
//-------------------------------------------------------------------------
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
//-------------------------------------------------------------------------
void getNameOfTag(const char* operationWithTag, char* nameOfTag){
    assert(operationWithTag);

    size_t operationWithTagLen = strlen(operationWithTag);
    printf("oper withTag = %s, len = %d\n", operationWithTag, operationWithTagLen);
    assert(operationWithTagLen <= 20);

    for(size_t i = 0; i + 1 < operationWithTagLen; i++){
        assert(i + 1 < operationWithTagLen);
        nameOfTag[i] = operationWithTag[i + 1];
    }
}
//-------------------------------------------------------------------------
void assembleJumps(const char* operation, int* argument, TypeOfOperation* typeOp, Tag* arrOfTags, char* operStr){
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
    else if(strcmp(operation, "CALL") == 0){
        *typeOp = CALL;
    }
    else if(strcmp(operation, "RET") == 0){
        *typeOp = RET;
        return;
    }
    else{
        return;
    }

    // #define QUOTE2(var)  #var
    // #define QUOTE(var)   QUOTE2 (var)
    // sscanf(operStr, "%" QUOTE (MAX_LENGTH_OF_TAG) "s %s", helpStr, nameOfTag);

    char helpStr[16] = "";
    char nameOfTag[MAX_LENGTH_OF_TAG + 1] = "";
    sscanf(operStr, "%15s %20s", helpStr, nameOfTag);
    //printf("name of tag = %s\n", nameOfTag);



    *argument = getIndOfTagByName(arrOfTags, nameOfTag);
    //printf("arg = %d, nameTag = %s\n", *argument, nameOfTag);

}
//-------------------------------------------------------------------------
void assembleFuncWithoutJmp(const char* operation, TypeOfOperation* codeOfOp){
    assert(operation);
    assert(codeOfOp);

        if(strcmp(operation, "ADD") == 0){
            *codeOfOp = ADD;
        }
        else if(strcmp(operation, "SUB") == 0){
            *codeOfOp = SUB;
        }
        else if(strcmp(operation, "MUL") == 0){
            *codeOfOp = MUL;
        }
        else if(strcmp(operation, "DIV") == 0){
            *codeOfOp = DIV;
        }
        else if(strcmp(operation, "SIN") == 0){
            *codeOfOp = SIN;
        }
        else if(strcmp(operation, "SQRT") == 0){
            *codeOfOp = SQRT;
        }
        // else if(strcmp(operation, "POPR") == 0){
        //     *codeOfOp = POPR;
        // }
        // // else if(strcmp(operation, "PSHR") == 0){
        // //     *codeOfOp = PSHR;
        // // }
        // else if(strcmp(operation, "POPM") == 0){
        //     *codeOfOp = POPM;
        // }
        // else if(strcmp(operation, "PSHM") == 0){
        //     *codeOfOp = PSHM;
        // }
        else if(strcmp(operation, "DRAW") == 0){
            *codeOfOp = DRAW;
        }
        else if(strcmp(operation, "IN") == 0){
            *codeOfOp = IN;
        }
        else if(strcmp(operation, "OUT") == 0){
            *codeOfOp = OUT;
        }
        else if(strcmp(operation, "HLT") == 0){
            *codeOfOp = HLT;
        }

}
//-------------------------------------------------------------------------
void assembleTag(int* argument, TypeOfOperation* codeOfOp, const char* str, Tag* arrOfTags){
    assert(argument);
    assert(codeOfOp);
    assert(str);

    printf("str = %s\n", str);
    
    if(str[0] == ':'){
        *codeOfOp = TAG;
        char nameOfTag[MAX_LENGTH_OF_TAG] = {};
        getNameOfTag(str, nameOfTag);
        printf("name = %s\n", nameOfTag);
        int ind = getIndOfTagByName(arrOfTags, nameOfTag);
        if(ind >= 0){
            *argument = ind;
            printf("indexindex = %d\n", *argument);
        }
        else{
            printf("ghhgghhghg =  %s\n", nameOfTag);
            assert(strcmp("THIS TAG IS NOT SEARCH" , " ") == 0);
        }

    }
}
//-------------------------------------------------------------------------
void printComandInFile(int countWord, int argument, int* codeOfOp, FILE* fileOut){
    assert(fileOut);
    assert(codeOfOp);
    
    if(countWord == 1){
        if(*codeOfOp == TAG){
            fprintf(fileOut, "%d", *codeOfOp);
            fprintf(fileOut, " %d", argument);
        }
        else{
            fprintf(fileOut, "%d", *codeOfOp);
        }    
    }    
    else if(countWord == 2){
        printf("oper = %d\n", *codeOfOp);
        fprintf(fileOut, "%d", *codeOfOp);
        fprintf(fileOut, " %d", argument);
    }
        
    fprintf(fileOut, "\n");
}
//-------------------------------------------------------------------------
void processOperWithArg(const char* stringOper, int* argument, char* operation, int* typeOfOper){
    assert(stringOper);
    assert(argument);
    assert(operation);

    char stringArg[30] = {}; 

    sscanf(stringOper, "%s %s", operation, stringArg);

    if(isCommandWithBranches(operation)){
        assembleCommandWithBranches(stringOper, argument, operation, typeOfOper);
    }
    else{
        sscanf(stringOper, "%s %d", operation, argument);
    }
}
//-------------------------------------------------------------------------
bool strIsWithoutOperations(const char* str){
    assert(str);
    
    size_t lenOfStr = strlen(str);
    bool without = true; 
    for(size_t i = 0; i < lenOfStr; i++){
        if((str[i] >= '0' && str[i] <= '9') || 
           (str[i] >= 'a' && str[i] <= 'z') ||
           (str[i] >= 'A' && str[i] <= 'Z')){
                without = false;
          }
    }
    return without;
}
//---------------------------------------------------------------------------
int getIndOfTagByName(Tag* arrOfTags, const char* nameTag){
    assert(arrOfTags);
    assert(nameTag);
    int indOfTag = -1;

    for(size_t i = 0; i < MAX_COUNT_OF_TAGS; i++){
        if(strcmp(nameTag, arrOfTags[i].name) == 0){
            //printf("nameTag = %s, really tag = %s\n", nameTag, arrOfTags[i].name);
            indOfTag = arrOfTags[i].indOfOper;
            break;
        }
    }
    //printf("ind = %d\n", indOfTag);
    return indOfTag;
}
//-------------------------------------------------------------------------------
// void processPushes(const char* operation, const char* stringArg){
//     assert(operation);
//     assert(stringArg);
    
//     if(strcmp(stringArg, "AX") == 0 ||
//         strcmp(stringArg, "[AX]") == 0){
//         *argument = 1;
//     }
//     else if(strcmp(stringArg, "BX") == 0 ||
//             strcmp(stringArg, "[BX]") == 0){
//         *argument = 2;
//     }
//     else if(strcmp(stringArg, "CX") == 0 ||
//             strcmp(stringArg, "[CX]") == 0){
//         *argument = 3;
//     }
//     else if(strcmp(stringArg, "DX") == 0 ||
//             strcmp(stringArg, "[DX]") == 0){
//         *argument = 4;
//     }
// }

// void processPops(const char* operation){

// }
//-------------------------------------------------------------------------------
bool isCommandWithBranches(const char* operation){
    assert(operation);

    if(strcmp(operation, "PUSH") == 0) return true;
    if(strcmp(operation,  "POP") == 0) return true;
    
    return false;
}
//-------------------------------------------------------------------------------
void assembleCommandWithBranches(const char* stringOper, int* argument, char* operation, int* typeOfOper){
    assert(stringOper);
    assert(argument);
    assert(operation);

    char stringArg[30] = {};
    sscanf(stringOper, "%s %s", operation, stringArg);

    if(strcmp(operation, "PUSH") == 0){
        *typeOfOper = PUSH;
        assemblePushAndPop(stringArg, typeOfOper, argument);
    }
    else if(strcmp(operation, "POP") == 0){
        *typeOfOper = POP;
        assemblePushAndPop(stringArg, typeOfOper, argument);
    }

    return;

}
//-------------------------------------------------------------------------------
void assemblePushAndPop(const char* stringArg, int* typeOfOper, int* argument){
    assert(stringArg);
    assert(typeOfOper);
    assert(argument);

    char registers[][5] = {"AX", "BX", "CX", "DX"};
    char memoryPush[][10] = {"[AX]", "[BX]", "[CX]", "[DX]"};

    if(strIsNumber(stringArg)){
        sscanf(stringArg, "%d", argument);
        *typeOfOper += 32; // 00100000
        return;
    }

    size_t regSize = sizeof(registers)/sizeof(registers[0]);
    for(size_t i = 0; i < regSize; i++){
        if(strcmp(stringArg, registers[i]) == 0){
            *argument = i + 1;
            *typeOfOper += 64; //01000000
            return;
        }
    }

    size_t memoryPushSize = sizeof(memoryPush)/sizeof(memoryPush[0]);
    for(size_t i = 0; i < memoryPushSize; i++){
        if(strcmp(stringArg, memoryPush[i]) == 0){
            *argument = i + 1;
            *typeOfOper += 128; //10000000
            return;
        }
    }

    return;
}

