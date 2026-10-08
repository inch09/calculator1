#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "stack.h"
#include "translator.cpp"

enum ActionWithProcess{
    CONTINUATION,
    COMPLETION
} ;

#define DUMP stackDump(LOG_FILE, &stack);

void stackAdd(Stack_t* stk);
ActionWithProcess processOperation(const char* str, Stack_t* stack);

int main(){
    translate(FILE_TO_BE_TRANSLATION, FILE_WITH_TRANSLATION_RESULT);

    Stack_t stack = {};
    stackInit(&stack, 10);

    FILE* filePtr = fopen(FILE_WITH_TRANSLATION_RESULT, "r");
    assert(filePtr);

    while(true){
        char str[20] = {};
        fgets(str, sizeof(str), filePtr);
        deleteLastEnter(str);

        ActionWithProcess action = processOperation((const char*) str, &stack);
        if(action == COMPLETION){
            DUMP;
            break;
        }
        
    }
    fclose(filePtr);

    return 0;
}

void stackAdd(Stack_t* stk){
    int a = stackPop(stk);
    int b = stackPop(stk);
    stackPush(stk, a + b);
}

ActionWithProcess processOperation(const char* str, Stack_t* stack){
    
    assert(str);
    assert(stack);

    int operation = -67;
    int number = 0;
    if(countOfWords(str) == 1){
        sscanf(str, "%d", &operation);

        switch (operation){
        case ADD:
            stackAdd(stack);
            return CONTINUATION;
        case OUT:
            return COMPLETION;
        default:
            return CONTINUATION;
        }

    }
    else if(countOfWords(str) == 2){
        sscanf(str, "%d %d", &operation, &number);

        switch (operation){
            case PUSH:
                stackPush(stack, number);
                return CONTINUATION;
            default:
                return CONTINUATION;      
        }
    }
    return COMPLETION;
}



// size_t countOfWords(const char* str){
//     size_t count = 1;
//     for(size_t i = 0; i < strlen(str); i++){
//         if(str[i] == ' '){
//             count++;
//         }
//     }
//     return count;
// }

// void deleteLastEnter(char* str){
//     if(str[strlen(str) - 1] == '\n'){
//         str[strlen(str)- 1] = '\0';
//     }
// }

























    // FILE* fileIn = fopen(FILE_TO_BE_TRANSLATION, "r");
    // FILE* fileOutCreate = fopen(FILE_WITH_TRANSLATION_RESULT, "w");
    // fclose(fileOutCreate);
    // FILE* fileOut = fopen(FILE_WITH_TRANSLATION_RESULT, "w");

    // while(true){
    //     char str[20] = {};
    //     fgets(str, sizeof(str), fileIn);
    //     deleteLastEnter(str);
    //     char operation[5] = {};
    //     Operations codeOfOperation = DEFAULT;
    //     int number = 0;
    //     size_t countWord = countOfWords((const char*) str);
    //     if(countWord == 1){
    //         sscanf(str, "%s", operation);
    //     }
    //     else if(countWord == 2){
    //         sscanf(str, "%s %d", operation, &number);
    //     }
    //     if(strcmp(operation, "PUSH") == 0){
    //         codeOfOperation = PUSH;
    //     }
    //     else if(strcmp(operation, "ADD") == 0){
    //         codeOfOperation = ADD;
    //     }
    //     else if(strcmp(operation, "OUT") == 0){
    //         codeOfOperation = OUT;
    //         fprintf(fileOut, "%d", codeOfOperation);
    //         break;
    //     }
    //     fprintf(fileOut, "%d", codeOfOperation);
    //     if(countWord == 2){
    //         fprintf(fileOut, " %d", number);
    //     }
    //     fprintf(fileOut, "\n");
    // }
    // fclose(fileIn);
    // fclose(fileOut);