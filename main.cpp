#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "operations.h"
#include "stack.h"
#include "translator.cpp"
#include "disassembler.cpp"

#define FILE_WITH_CODE "code.txt"

enum ActionWithProcess{
    DEFAULT_ACT,
    CONTINUATION,
    COMPLETION,
    JUMP
};

struct Processor{
    double registers[4];
    Operation* operations;
    int* code;
    size_t countOfCommands;
    int indOfOper;
    int tags[20];
};

#define DUMP stackDump(LOG_FILE, &stack);

void processorInit(Processor* processor, Operation* operations);
int fillCodeArr(Processor* processor, const char* fileWithCode);

void binaryOperation(Stack_t* stk, TypeOfOperation operation);
void unaryOperation(Stack_t* stack, TypeOfOperation operation);
ActionWithProcess processOperation(Processor* processor, Stack_t* stack);


ActionWithProcess processJump(int typeOfOperation, Stack_t* stack, Processor* processor);
void processPshr(Operation* oper, Processor* processor, Stack_t* stack);
void processPopr(Operation* oper, Processor* processor, Stack_t* stack);

void deleteLastEnter(char* str);

int main(){
    // Operation* operations = translate(FILE_TO_BE_TRANSLATION, FILE_WITH_TRANSLATION_RESULT);
    // assert(operations);
    // disAssembling(FILE_TO_BE_DISASSEMBLING, FILE_WITH_DISASSEMBLING_RESULT);

    Processor processor = {};
    processorInit(&processor, operations);
    fillCodeArr(&processor, FILE_WITH_CODE);

    Stack_t stack = {};
    stackInit(&stack, 10);

    FILE* filePtr = fopen(FILE_WITH_CODE , "r");
    assert(filePtr);

    while(true){
        ActionWithProcess action = processOperation(&processor, &stack);
        // DUMP;
        // getchar();
        if(action == COMPLETION){
            DUMP;
            processor.indOfOper = 0;
            break;
        }
        if(action != JUMP){
            processor.indOfOper++;
        }
    }
    
    free(processor.code);
    free(operations);
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
        return CONTINUATION;

#define processUnOp(typeOfOp)\
    case typeOfOp:\
        unaryOperation(stack, typeOfOp);\
        return CONTINUATION;

#define commandWithoutArg(command)\
    command == ADD || command ==
    

ActionWithProcess processOperation(Processor* processor, Stack_t* stack){
    
    assert(stack);
    assert(processor);

    int a = 0;
    int b = 0;

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
            stackPush(stack, processor->code[indOfOper + 1]);
            return CONTINUATION;
    
        case POPR:
            
            processPopr(processor->code[indOfOper + 1], processor, stack);
            return CONTINUATION;    
        
        case PSHR:
            
            processPshr(processor->code[indOfOper + 1], processor, stack);
            return CONTINUATION; 

        case TAG:{
            processor->tags[processor->code[indOfOper + 1] - 1] = processor->indOfOper + 1;
            return CONTINUATION;
        }

        case OUT:
            elem = stackPop(stack);
            printf(SPECIFIER, elem);
            printf("\n");
            stackPush(stack, elem);
            return CONTINUATION;
        
        case HLT:
            return COMPLETION;
        default:
            return CONTINUATION;     
    }
    return COMPLETION;
}

#undef processBinOp
#undef processUnOp

void processorInit(Processor* processor, Operation* operations){
    assert(processor);
    assert(operations);
    for(size_t i = 0; i < 4; i++){
        processor->registers[i] = 0;
    }

    processor->operations = operations;
    processor->indOfOper = 0;

    processor->code = (int*) calloc(200, sizeof(code[0]));
    assert(processor->code);
    processor->countOfCommands = 0;
    
    for(size_t i = 0; i < 20; i++){
        processor->tags[i] = -1;
    }
}

void processPshr(int argument, Processor* processor, Stack_t* stack){
    assert(oper);
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

    int count = sscanf(fileCodePtr, "%d %d", &command, &argument);

    //sdelat rashirenie massiva v sluchae chego

    if(count == 1){
        processor->code[processor->indOfOper++] = command;
    }
    else if(count == 2){
        processor->code[processor->indOfOper++] = command;
        processor->code[processor->indOfOper++] = argument;
    }
    else{
        fclose(fileCodePtr);
        processor->countOfCommands = processor->indOfOper + 1;
        return -1;
    }

    }

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
        processor->indOfOper = processor->tags[processor->code[indOfOper + 1] - 1];
        return JUMP;
    }
    if(cont == true){
        return CONTINUATION;
    }
    return DEFAULT_ACT;
}
