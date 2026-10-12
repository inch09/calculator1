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


enum ActionWithProcess{
    CONTINUATION,
    COMPLETION,
    JUMP
};

struct Processor{
    double registers[4];
    Operation* operations;
    int indOfOper;
    int tags[20];
};

#define DUMP stackDump(LOG_FILE, &stack);

void processorInit(Processor* processor, Operation* operations);

void binaryOperation(Stack_t* stk, TypeOfOperation operation);
void unaryOperation(Stack_t* stack, TypeOfOperation operation);
ActionWithProcess processOperation(Processor* processor, Stack_t* stack);

int main(){
    Operation* operations = translate(FILE_TO_BE_TRANSLATION, FILE_WITH_TRANSLATION_RESULT);
    assert(operations);
    disAssembling(FILE_TO_BE_DISASSEMBLING, FILE_WITH_DISASSEMBLING_RESULT);

    Processor processor = {};
    processorInit(&processor, operations);

    Stack_t stack = {};
    stackInit(&stack, 10);

    FILE* filePtr = fopen(FILE_WITH_TRANSLATION_RESULT, "r");
    assert(filePtr);

    // while(true){
    //     char str[20] = {};
    //     fgets(str, sizeof(str), filePtr);
    //     deleteLastEnter(str);

    //int indOfOper = 0;
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
    
    while(true){
        ActionWithProcess action = processOperation(&processor, &stack);
        // DUMP;
        // getchar();
        if(action == COMPLETION){
            DUMP;
            break;
        }
        if(action != JUMP){
            processor.indOfOper++;
        }
    }    

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

ActionWithProcess processOperation(Processor* processor, Stack_t* stack){
    
    assert(stack);
    assert(processor);

    Operation oper = processor->operations[processor->indOfOper];

    int typeOfOperation = oper.type;
    Stack_elem_t elem = 0;

    if(isnan(oper.argument1) && isnan(oper.argument2)){

        switch (typeOfOperation){
        case ADD:
            binaryOperation(stack, ADD);
            return CONTINUATION;
        
        case SUB:
            binaryOperation(stack, SUB);
            return CONTINUATION; 
        
        case MUL:
            binaryOperation(stack, MUL);
            return CONTINUATION;
        
        case DIV:
            binaryOperation(stack, DIV);
            return CONTINUATION;    
        
        case SIN:
            unaryOperation(stack, SIN);
            return CONTINUATION; 
                
        case SQRT:
            unaryOperation(stack, SQRT);
            return CONTINUATION;

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

    }
    else if(!isnan(oper.argument1) && isnan(oper.argument2)){
        double a = NAN;
        double b = NAN;
        switch (typeOfOperation){
            case PUSH:
                stackPush(stack, oper.argument1);
                return CONTINUATION;
        
            case POPR:
                if(isEqual(oper.argument1, 1)){
                    processor->registers[0] = stackPop(stack);
                }

                if(isEqual(oper.argument1, 2)){
                    processor->registers[1] = stackPop(stack);
                }

                if(isEqual(oper.argument1, 3)){
                    processor->registers[2] = stackPop(stack);
                }

                if(isEqual(oper.argument1, 4)){
                    processor->registers[3] = stackPop(stack);
                }
                return CONTINUATION;    
            
            case PSHR:

            if(isEqual(oper.argument1, 1)){
                    stackPush(stack, processor->registers[0]);
                }

                if(isEqual(oper.argument1, 2)){
                    stackPush(stack, processor->registers[1]);
                }

                if(isEqual(oper.argument1, 3)){
                    stackPush(stack, processor->registers[2]);
                }

                if(isEqual(oper.argument1, 4)){
                    stackPush(stack, processor->registers[3]);
                }
                return CONTINUATION; 

            case JMP:
                processor->indOfOper = processor->tags[(int) oper.argument1 - 1];
                return JUMP;
            case JA:
                a = stackPop(stack);
                b = stackPop(stack);
                if(a < b){
                    processor->indOfOper = processor->tags[(int) oper.argument1 - 1];
                    return JUMP;
                }
                else{
                    return CONTINUATION;
                }
            case JAE:
                a = stackPop(stack);
                b = stackPop(stack);
                if(a < b || isEqual(a, b)){
                    processor->indOfOper = processor->tags[(int) oper.argument1 - 1];
                    return JUMP;
                }
                else{
                    return CONTINUATION;
                }
            case JB:
                a = stackPop(stack);
                b = stackPop(stack);
                if(a > b){
                    processor->indOfOper = processor->tags[(int) oper.argument1 - 1];
                    return JUMP;
                }
                else{
                    return CONTINUATION;
                }
            case JBE:
                a = stackPop(stack);
                b = stackPop(stack);
                if(a > b || isEqual(a, b)){
                    processor->indOfOper = processor->tags[(int) oper.argument1 - 1];
                    return JUMP;
                }
                else{
                    return CONTINUATION;
                }
            case JE:
                a = stackPop(stack);
                b = stackPop(stack);
                if(isEqual(a, b)){
                    processor->indOfOper = processor->tags[(int) oper.argument1 - 1];
                    return JUMP;
                }
                else{
                    return CONTINUATION;
                }
            case JNE:
                a = stackPop(stack);
                b = stackPop(stack);
                if(!isEqual(a, b)){
                    processor->indOfOper = processor->tags[(int) oper.argument1 - 1];
                    return JUMP;
                }
                else{
                    return CONTINUATION;
                }
            case TAG:{
                processor->tags[((int) oper.argument1) - 1] = processor->indOfOper + 1;
                return CONTINUATION;
            }             
            default:
                return CONTINUATION;      
        }
    }
    return COMPLETION;
}

void processorInit(Processor* processor, Operation* operations){
    assert(processor);
    assert(operations);
    for(int i = 0; i < 4; i++){
        processor->registers[i] = 0;
    }
    processor->operations = operations;
    processor->indOfOper = 0;
    for(int i = 0; i < 20; i++){
        processor->tags[i] = -1;
    }
}


