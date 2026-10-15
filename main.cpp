#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "operations.h"
#include "stack.h"

#define FILE_WITH_CODE "code.txt"
#define PROCESSOR_LOG_FILE "processor.log"

enum ActionWithProcess{
    DEFAULT_ACT,
    CONTINUATION,
    COMPLETION,
    JUMP
};

struct Processor{
    double registers[4];
    int* code;
    size_t countOfCommands;
    int indOfOper;
    int tags[20];
};

#define DUMP stackDump(LOG_FILE, &stack);

void processorInit(Processor* processor);
int fillCodeArr(Processor* processor, const char* fileWithCode);
void processorDump(Processor* processor, const char* processorLogFile);

void binaryOperation(Stack_t* stk, TypeOfOperation operation);
void unaryOperation(Stack_t* stack, TypeOfOperation operation);
ActionWithProcess processOperation(Processor* processor, Stack_t* stack);


ActionWithProcess processJump(int typeOfOperation, Stack_t* stack, Processor* processor);
void processPshr(int argument, Processor* processor, Stack_t* stack);
void processPopr(int argument, Processor* processor, Stack_t* stack);

void deleteLastEnter(char* str);

int main(){
    // Operation* operations = translate(FILE_TO_BE_TRANSLATION, FILE_WITH_TRANSLATION_RESULT);
    // assert(operations);
    // disAssembling(FILE_TO_BE_DISASSEMBLING, FILE_WITH_DISASSEMBLING_RESULT);

    Processor processor = {};
    processorInit(&processor);
    fillCodeArr(&processor, FILE_WITH_CODE);
    printf("count com = %d\n", processor.countOfCommands);

    Stack_t stack = {};
    stackInit(&stack, 10);

    FILE* filePtr = fopen(FILE_WITH_CODE , "r");
    assert(filePtr);

    // for(int i = 0; i < processor.countOfCommands; i++){
    //     printf("%d ", processor.code[i]);
    // }

    while(true){
        ActionWithProcess action = processOperation(&processor, &stack);
        if(action == COMPLETION){
            DUMP;
            processor.indOfOper = 0;
            break;
        }
        
    }
    
    free(processor.code);
    fclose(filePtr);
    
    return 0;
}

void binaryOperation(Stack_t* stk, TypeOfOperation operation){
    assert(stk);

    Stack_elem_t a = stackPop(stk);
    Stack_elem_t b = stackPop(stk);
    
    switch (operation){
    case ADD:
        stackPush(stk, a + b);
        return;

    case SUB:
        stackPush(stk, b - a);
        return;

    case MUL:
        stackPush(stk, a * b);
        return;

    case DIV:
        assert(a != 0);
        stackPush(stk, b / a);
        return;

    case DEFAULT:
        return;
    }
}

void unaryOperation(Stack_t* stack, TypeOfOperation operation){
    assert(stack);

    Stack_elem_t a = stackPop(stack); 

    switch (operation){
    case SIN:
        stackPush(stack, sin(a));
        return;

    case SQRT:
        stackPush(stack, sqrt(a));
        return;
    }

}

#define processBinOp(typeOfOp)\
    case typeOfOp:\
        binaryOperation(stack, typeOfOp);\
        processor->indOfOper += 1;\
        return CONTINUATION;

#define processUnOp(typeOfOp)\
    case typeOfOp:\
        unaryOperation(stack, typeOfOp);\
        processor->indOfOper += 1;\
        return CONTINUATION;


ActionWithProcess processOperation(Processor* processor, Stack_t* stack){
    
    assert(stack);
    assert(processor);


    int typeOfOperation = processor->code[processor->indOfOper];
    Stack_elem_t elem = 0;

        ActionWithProcess act = processJump(typeOfOperation, stack, processor);
        if(act == DEFAULT_ACT){
        }
        else{
            return act;
        }

        switch (typeOfOperation){

        processBinOp(ADD);
        processBinOp(SUB);
        processBinOp(MUL);
        processBinOp(DIV);

        processUnOp(SIN);
        processUnOp(SQRT);

        case PUSH:
            //printf("push\n");
            //printf("ind of oper = %d\n", processor->indOfOper);
            stackPush(stack, processor->code[processor->indOfOper + 1]);
            processor->indOfOper += 2;
            return CONTINUATION;
    
        case POPR:
            //printf("popr\n");
            processPopr(processor->code[processor->indOfOper + 1], processor, stack);
            processor->indOfOper += 2;
            return CONTINUATION;    
        
        case PSHR:
            //printf("pshr\n");
            processPshr(processor->code[processor->indOfOper + 1], processor, stack);
            processor->indOfOper += 2;
            return CONTINUATION; 

        case TAG:{
            //printf("tag\n");
            processor->tags[processor->code[processor->indOfOper + 1] - 1] = processor->indOfOper + 2;
            processor->indOfOper += 2;
            return CONTINUATION;
        }

        case IN:{
            int argument = 0;
            int c = scanf("%d", &argument);
            if(c == 1){
                stackPush(stack, argument);
                processor->indOfOper += 1;
                return CONTINUATION;
            }
            else{
                printf("error enter\n");
                return COMPLETION;
            }
        }

        case OUT:
            //printf("out\n");
            elem = stackPop(stack);
            printf(SPECIFIER, elem);
            printf("\n");
            stackPush(stack, elem);
            processor->indOfOper += 1;
            return CONTINUATION;
        
        case HLT:
            return COMPLETION;
        default:
            processor->indOfOper += 1;
            return CONTINUATION;     
    }
    return COMPLETION;
}

#undef processBinOp
#undef processUnOp

void processorInit(Processor* processor){
    assert(processor);

    for(size_t i = 0; i < 4; i++){
        processor->registers[i] = 0;
    }

    processor->indOfOper = 0;

    processor->code = (int*) calloc(200, sizeof(processor->code[0]));
    assert(processor->code);
    processor->countOfCommands = 0;
    
    for(size_t i = 0; i < 20; i++){
        processor->tags[i] = -1;
    }
}

void processPshr(int argument, Processor* processor, Stack_t* stack){
    assert(processor);
    assert(stack);

    if(isEqual(argument, 1)){
        stackPush(stack, processor->registers[0]);
    }

    else if(isEqual(argument, 2)){
        stackPush(stack, processor->registers[1]);
    }

    else if(isEqual(argument, 3)){
        stackPush(stack, processor->registers[2]);
    }

    else if(isEqual(argument, 4)){
        stackPush(stack, processor->registers[3]);
    }
}

void processPopr(int argument, Processor* processor, Stack_t* stack){

    assert(processor);
    assert(stack);

    if(isEqual(argument, 1)){
        processor->registers[0] = stackPop(stack);
    }

    else if(isEqual(argument, 2)){
        processor->registers[1] = stackPop(stack);
    }

    else if(isEqual(argument, 3)){
        processor->registers[2] = stackPop(stack);
    }

    else if(isEqual(argument, 4)){
        processor->registers[3] = stackPop(stack);
    }
}

int fillCodeArr(Processor* processor, const char* fileWithCode){
    assert(processor);

    FILE* fileCodePtr = fopen(fileWithCode, "r");
    assert(fileCodePtr);

    int command = -1;
    int argument = -1;

    while(true){

    char strCommand[20] = {};
    fgets(strCommand, sizeof(strCommand), fileCodePtr);
    deleteLastEnter(strCommand);

    int count = sscanf(strCommand, "%d %d", &command, &argument);

    //sdelat rashirenie massiva v sluchae chego

    if(count == 1){
        processor->code[processor->indOfOper++] = command;
        //printf("command = %d, 1\n", command);
    }
    else if(count == 2){
        processor->code[processor->indOfOper++] = command;
        processor->code[processor->indOfOper++] = argument;
        //printf("command = %d, argument = %d, 2\n", command, argument);
    }
    else{
        fclose(fileCodePtr);
        processor->countOfCommands = processor->indOfOper;
        processor->indOfOper = 0;
        //printf("end\n");
        return -1;
    }

    }

    processor->indOfOper = 0;
    
    fclose(fileCodePtr);
}

void deleteLastEnter(char* str){
    if(str[strlen(str) - 1] == '\n'){
        str[strlen(str)- 1] = '\0';
    }
}

ActionWithProcess processJump(int typeOfOperation, Stack_t* stack, Processor* processor){

    assert(stack);
    assert(processor);

    bool cont = false;
    bool jmp = false;

    if(typeOfOperation != JMP && typeOfOperation != JA &&  typeOfOperation != JAE &&
       typeOfOperation != JB  && typeOfOperation != JBE && typeOfOperation != JE &&
       typeOfOperation != JNE) return DEFAULT_ACT;

    int a = stackPop(stack);
    int b = stackPop(stack);

    // printf("pop2\n");

    switch(typeOfOperation){
    case JMP:
        jmp = true;
        break;

    case JA:
        if(a < b) jmp = true;
        else cont = true;
        break;

    case JAE:
        if(a < b || isEqual(a, b)) jmp = true;
        else cont = true;
        break;

    case JB:
        if(a > b) jmp = true;
        else cont = true;
        break;

    case JBE:
        if(a > b || isEqual(a, b)) jmp = true;
        else cont = true;
        break;

    case JE:
        if(isEqual(a, b)) jmp = true;
        else cont = true;
        break;

    case JNE:
        if(!isEqual(a, b)) jmp = true;
        else cont = true;
        break;
    }
    
    if(jmp == true){
        if(processor->tags[processor->code[processor->indOfOper + 1] - 1]  >= 0){
            processor->indOfOper = processor->tags[processor->code[processor->indOfOper + 1] - 1];
            return JUMP;
        }
        else{
            processor->indOfOper += 2;
            return CONTINUATION;
        }
    }
    if(cont == true){
        processor->indOfOper += 2;
        return CONTINUATION;
    }
    return DEFAULT_ACT;
}

void processorDump(Processor* processor, const char* processorLogFile){
    assert(processor);
    FILE* prLogFile = fopen(processorLogFile, "w");
    assert(prLogFile);

    fprintf(prLogFile, "                  IT IS INFORMATION ABOUT YOUR PROCESSOR\n");
    for(size_t i = 0; i < processor->countOfCommands; i++){
        fprintf(prLogFile, "%2d ", processor->code[i]);
    }

    fprintf(prLogFile, "\n");
    for(size_t i = 0; i < processor->indOfOper; i++){
        fprintf(prLogFile, "   ");
    }
    fprintf(prLogFile, "^");

    fprintf(prLogFile, "\n\n---------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\n");
    fprintf(prLogFile, "Current processor instruction: ");
    fclose(prLogFile);

}
