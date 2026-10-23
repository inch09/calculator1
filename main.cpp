#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "operations.h"
#include "stack.h"
//----------------------------------------------------------------------------------
#define MAX_LENGTH_OF_ARR_CODE 500
#define SIZE_OF_RAM 41*41
#define FILE_WITH_CODE "code.txt"
#define PROCESSOR_LOG_FILE "processor.log"
//----------------------------------------------------------------------------------
enum ActionWithProcess{
    DEFAULT_ACT,
    CONTINUATION,
    COMPLETION,
    JUMP
};
//----------------------------------------------------------------------------------
struct Processor{
    int registers[4];
    int* code;
    size_t countOfCommands;
    int indOfOper;
    Stack_t stackOfRetTags;
    int* ram;
};

#define DUMP stackDump(LOG_FILE, &stack);
//----------------------------------------------------------------------------------
void processorInit(Processor* processor);
int fillCodeArr(Processor* processor, const char* fileWithCode);
void processorDestroy(Processor* processor);
//----------------------------------------------------------------------------------
void processorDump(Processor* processor, const char* processorLogFile);
bool isCommandHaveArgument(int typeOfOperation, TypeOfOperation* oper);
void getStringEnum(TypeOfOperation* typeOfOperation, const char** strOp);
void printValuesOfRegisters(Processor* processor, FILE* prLogFile);
void printRam(Processor* processor, FILE* prLogFile);
void printCode(Processor* processor, FILE* prLogFile);
void dumpRam(Processor* processor);
//----------------------------------------------------------------------------------
void binaryOperation(Stack_t* stack, int operation);
void unaryOperation (Stack_t* stack, int operation);
//----------------------------------------------------------------------------------
ActionWithProcess processOperation(Processor* processor, Stack_t* stack);
ActionWithProcess processUnOp(Processor* processor, Stack_t* stack, int typeOfOperation);
ActionWithProcess processBinOp(Processor* processor, Stack_t* stack, int typeOfOperation);
ActionWithProcess processHlt(int typeofOperation);
ActionWithProcess processDraw(Processor* processor, int typeOfOperation);
ActionWithProcess processCallAndRet(Processor* processor, int typeOfOperation);
ActionWithProcess processTag(Processor* processor, int typeOfOperation);
ActionWithProcess processInOutFunc(Processor* processor, Stack_t* stack, int typeOfOperation);
ActionWithProcess processJump(int typeOfOperation, Stack_t* stack, Processor* processor);
ActionWithProcess processPoprsAndPushes(Processor* processor, Stack_t* stack, int typeOfOperation);
//----------------------------------------------------------------------------------
void processPshr(int argument, Processor* processor, Stack_t* stack);
void processPopr(int argument, Processor* processor, Stack_t* stack);
void processPshm(int argument, Processor* processor, Stack_t* stack);
void processPopm(int argument, Processor* processor, Stack_t* stack);
//----------------------------------------------------------------------------------
void drawSquareRam(Processor* processor);
void deleteLastEnter(char* str);
//----------------------------------------------------------------------------------

int main(){
    
    Processor processor = {};
    processorInit(&processor);
    fillCodeArr(&processor, FILE_WITH_CODE);

    Stack_t stack = {};
    stackInit(&stack, 10);

    FILE* filePtr = fopen(FILE_WITH_CODE , "r");
    assert(filePtr);

    while(true){
        processorDump(&processor, PROCESSOR_LOG_FILE);
        //stackDump("stack.log", &stack);
        getchar();
        ActionWithProcess action = processOperation(&processor, &stack);
        if(action == COMPLETION){
            DUMP;
            processor.indOfOper = 0;
            processorDump(&processor, PROCESSOR_LOG_FILE);
            //dumpRam(&processor);
            printf("end");
            break;
        }
        
    }

    processorDestroy(&processor);
    stackDestroy(&stack);
    fclose(filePtr);
    
    return 0;
}
//----------------------------------------------------------------------------------
void binaryOperation(Stack_t* stk, int operation){
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

    default:
        return;    
    }
}
//----------------------------------------------------------------------------------
void unaryOperation(Stack_t* stack, int operation){
    assert(stack);

    Stack_elem_t a = stackPop(stack); 

    switch (operation){
    case SIN:
        stackPush(stack, sin(a));
        return;

    case SQRT:
        stackPush(stack, sqrt(a));
        return;
    
    default:
        return;    
    }
    
}
//----------------------------------------------------------------------------------

ActionWithProcess processOperation(Processor* processor, Stack_t* stack){
    
    assert(stack);
    assert(processor);


    int typeOfOperation = processor->code[processor->indOfOper];

        ActionWithProcess act = DEFAULT_ACT;
        act = processJump(typeOfOperation, stack, processor);
        if(act != DEFAULT_ACT) return act;

        act = processPoprsAndPushes(processor, stack, typeOfOperation);
        if(act != DEFAULT_ACT) return act;

        act = processInOutFunc(processor, stack, typeOfOperation);
        if(act != DEFAULT_ACT) return act;

        act = processTag(processor, typeOfOperation);
        if(act != DEFAULT_ACT) return act;

        act = processCallAndRet(processor, typeOfOperation);
        if(act != DEFAULT_ACT) return act;

        act = processDraw(processor, typeOfOperation);
        if(act != DEFAULT_ACT) return act;

        act = processHlt(typeOfOperation);
        if(act != DEFAULT_ACT) return act;

        act = processBinOp(processor, stack, typeOfOperation);
        if(act != DEFAULT_ACT) return act;

        act = processUnOp(processor, stack, typeOfOperation);
        if(act != DEFAULT_ACT) return act;
        
    return COMPLETION;
}

//----------------------------------------------------------------------------------
void processorInit(Processor* processor){
    assert(processor);

    for(size_t i = 0; i < 4; i++){
        processor->registers[i] = 0;
    }

    processor->indOfOper = 0;

    processor->code = (int*) calloc(MAX_LENGTH_OF_ARR_CODE, sizeof(processor->code[0]));
    assert(processor->code);

    processor->ram = (int*) calloc(SIZE_OF_RAM, sizeof(processor->ram[0]));
    assert(processor->ram);

    processor->countOfCommands = 0;

    processor->stackOfRetTags = {};
    stackInit(&processor->stackOfRetTags, 10);
    
}
//--------------------------------------------------------------------------------------------------
void processorDestroy(Processor* processor){
    assert(processor);

    free(processor->code);
    free(processor->ram);
    stackDestroy(&processor->stackOfRetTags);
    
    processor = NULL;
}
//----------------------------------------------------------------------------------
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
//----------------------------------------------------------------------------------
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
//----------------------------------------------------------------------------------
void processPshm(int argument, Processor* processor, Stack_t* stack){
    assert(processor);
    assert(stack);

    if(isEqual(argument, 1)){
        stackPush(stack, processor->ram[processor->registers[0]]);
    }

    else if(isEqual(argument, 2)){
        stackPush(stack, processor->ram[processor->registers[1]]);
    }

    else if(isEqual(argument, 3)){
        stackPush(stack, processor->ram[processor->registers[2]]);
    }

    else if(isEqual(argument, 4)){
        stackPush(stack, processor->ram[processor->registers[3]]);
    }
}
//----------------------------------------------------------------------------------
void processPopm(int argument, Processor* processor, Stack_t* stack){

    assert(processor);
    assert(stack);

    if(isEqual(argument, 1)){
        processor->ram[processor->registers[0]] = stackPop(stack);
    }

    else if(isEqual(argument, 2)){
        processor->ram[processor->registers[1]] = stackPop(stack);
    }

    else if(isEqual(argument, 3)){
        processor->ram[processor->registers[2]] = stackPop(stack);
    }

    else if(isEqual(argument, 4)){
        processor->ram[processor->registers[3]] = stackPop(stack);
    }
}
//----------------------------------------------------------------------------------
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
//----------------------------------------------------------------------------------
ActionWithProcess processJump(int typeOfOperation, Stack_t* stack, Processor* processor){

    assert(stack);
    assert(processor);

    bool cont = false;
    bool jmp = false;

    if(typeOfOperation != JMP && typeOfOperation != JA &&  typeOfOperation != JAE &&
       typeOfOperation != JB  && typeOfOperation != JBE && typeOfOperation != JE &&
       typeOfOperation != JNE) return DEFAULT_ACT;

    if(typeOfOperation == JMP){
        processor->indOfOper = processor->code[processor->indOfOper + 1];
        return JUMP;
    }

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
        processor->indOfOper = processor->code[processor->indOfOper + 1];
        return JUMP;
    }
    if(cont == true){
        processor->indOfOper += 2;
        return CONTINUATION;
    }
    return DEFAULT_ACT;
}
//----------------------------------------------------------------------------------
void dumpRam(Processor* processor){
    assert(processor);

    for(size_t i = 0; i < SIZE_OF_RAM; i++){
        printf("%d ", processor->ram[i]);
    }
}
//----------------------------------------------------------------------------------
void drawSquareRam(Processor* processor){
    assert(processor);

    for(size_t y = 1; y <= (int) sqrt(SIZE_OF_RAM); y++){
        for(size_t x = 1; x <= (int) sqrt(SIZE_OF_RAM); x++){
            if(processor->ram[(int) sqrt(SIZE_OF_RAM) * (y - 1) + x - 1] == 1){
                printf("# ");
            }
            else{
                printf(". ");
            }
        }
        printf("\n");
    }
}
//----------------------------------------------------------------------------------
ActionWithProcess processPoprsAndPushes(Processor* processor, Stack_t* stack, int typeOfOperation){
    assert(processor);
    assert(stack);

    switch(typeOfOperation){
        case PUSH: stackPush(stack, processor->code[processor->indOfOper + 1]);
            break;
        case POPR: processPopr(processor->code[processor->indOfOper + 1], processor, stack);
            break;
        case PSHR: processPshr(processor->code[processor->indOfOper + 1], processor, stack);
            break;
        case POPM: processPopm(processor->code[processor->indOfOper + 1], processor, stack);
            break;
        case PSHM: processPshm(processor->code[processor->indOfOper + 1], processor, stack);
            break;
        default: return DEFAULT_ACT;
    }

    processor->indOfOper += 2;
    return CONTINUATION;
}
//-----------------------------------------------------------------------------------------
ActionWithProcess processInOutFunc(Processor* processor, Stack_t* stack, int typeOfOperation){
    assert(processor);
    assert(stack);

    if(typeOfOperation == IN){
        int argument = 0;
        int isOkArgument = scanf("%d", &argument);
        if(isOkArgument == 1){
            stackPush(stack, argument);
            processor->indOfOper += 1;
            return CONTINUATION;
        }
        else{
            printf("error enter\n");
            return COMPLETION;
        }
    }
    else if(typeOfOperation == OUT){
        Stack_elem_t elem = stackPop(stack);
        printf(SPECIFIER, elem);
        printf("\n");
        stackPush(stack, elem);
        processor->indOfOper += 1;
        return CONTINUATION;
    }   
    else{
        return DEFAULT_ACT;
    } 
}
//------------------------------------------------------------------------------------------------
ActionWithProcess processTag(Processor* processor, int typeOfOperation){
    assert(processor);
    if(typeOfOperation == TAG){
        processor->indOfOper += 2;
        return CONTINUATION;
    }
    else{
        return DEFAULT_ACT;
    }
}
//------------------------------------------------------------------------------------------------
ActionWithProcess processCallAndRet(Processor* processor, int typeOfOperation){
    assert(processor);
    if(typeOfOperation == CALL){
        stackPush(&processor->stackOfRetTags, processor->indOfOper + 2);
        processor->indOfOper = processor->code[processor->indOfOper + 1];
        return JUMP;
    }
    else if(typeOfOperation == RET){
        processor->indOfOper = stackPop(&processor->stackOfRetTags);
        return JUMP;
    }
    else{
        return DEFAULT_ACT;
    }  
}
//------------------------------------------------------------------------------------------------
ActionWithProcess processDraw(Processor* processor, int typeOfOperation){
    assert(processor);
    if(typeOfOperation == DRAW){
        drawSquareRam(processor);
        processor->indOfOper += 1;
        return CONTINUATION;
    }
    else{
        return DEFAULT_ACT;
    }    
}
//------------------------------------------------------------------------------------------------
ActionWithProcess processHlt(int typeofOperation){
    if(typeofOperation == HLT){
        return COMPLETION;
    }
    else{
        return DEFAULT_ACT;
    }
}
//------------------------------------------------------------------------------------------------
ActionWithProcess processBinOp(Processor* processor, Stack_t* stack, int typeOfOperation){
    assert(processor);
    assert(stack);
    if(typeOfOperation == ADD || typeOfOperation == SUB ||
       typeOfOperation == MUL || typeOfOperation == DIV){
        binaryOperation(stack, typeOfOperation);
        processor->indOfOper += 1;
        return CONTINUATION;
    }
    else{
        return DEFAULT_ACT;
    }
}
//------------------------------------------------------------------------------------------------
ActionWithProcess processUnOp(Processor* processor, Stack_t* stack, int typeOfOperation){
    assert(processor);
    assert(stack);
    if(typeOfOperation == SIN || typeOfOperation == SQRT){
        unaryOperation(stack, typeOfOperation);
        processor->indOfOper += 1;
        return CONTINUATION;
    }
    else{
        return DEFAULT_ACT;
    }
}
//------------------------------------------------------------------------------------------------
void processorDump(Processor* processor, const char* processorLogFile){
    assert(processor);
    assert(processorLogFile);

    FILE* prLogFile = fopen(processorLogFile, "w");
    assert(prLogFile);

    printCode(processor, prLogFile);
    printValuesOfRegisters(processor, prLogFile);
    printRam(processor, prLogFile);

    fclose(prLogFile);
}
//--------------------------------------------------------------------------------------------------------------
void printValuesOfRegisters(Processor* processor, FILE* prLogFile){
    assert(processor);
    assert(prLogFile);

    fprintf(prLogFile, "------------------------------------------------------------------------------------------------------------------------------\n");
    fprintf(prLogFile, "                  Registers of processor\n");
    fprintf(prLogFile, "AX = %d\n", processor->registers[0]);
    fprintf(prLogFile, "BX = %d\n", processor->registers[1]);
    fprintf(prLogFile, "CX = %d\n", processor->registers[2]);
    fprintf(prLogFile, "DX = %d\n", processor->registers[3]);
}
//-----------------------------------------------------------------------------------------------
void printRam(Processor* processor, FILE* prLogFile){
    assert(processor);
    assert(prLogFile);

    fprintf(prLogFile, "----------------------------------------------------------------------------------------------------------------\n");
    fprintf(prLogFile, "                  Information about RAM\n");
    size_t string = 0;
    for(size_t i = 0; i < SIZE_OF_RAM; i++){
        string++;
        fprintf(prLogFile, "%4d.[%d] ", (int) i, processor->ram[i]);
        if(string >= SIZE_OF_RAM / 10){
            fprintf(prLogFile, "\n");
            string = 0;
        }
        else{
            string++;
        }
    }
}
//-----------------------------------------------------------------------------------------------
void printCode(Processor* processor, FILE* prLogFile){
    assert(processor);
    assert(prLogFile);
        
    size_t numOfCommand = 1;
    TypeOfOperation oper = DEFAULT;

    fprintf(prLogFile, "                  IT IS INFORMATION ABOUT YOUR PROCESSOR\n");
    for(size_t i = 0; i < processor->countOfCommands; i++){
        int increase = 0;
        const char* strOperation = "";
        fprintf(prLogFile, "%d. ", numOfCommand);
        if(isCommandHaveArgument(processor->code[i], &oper)){
            fprintf(prLogFile, "%d ", processor->code[i]);
            fprintf(prLogFile, "%d", processor->code[i + 1]);
            increase = 1;
        }
        else{
            fprintf(prLogFile, "%d", processor->code[i]);
        }

        getStringEnum(&oper, &strOperation);

        if(i == processor->indOfOper){
            fprintf(prLogFile, "  <----- ");
            fprintf(prLogFile, "%s", strOperation);
            if(isCommandHaveArgument(processor->code[i], &oper)){
                fprintf(prLogFile, " %d", processor->code[i + 1]);
            }
        }
        
        fprintf(prLogFile, "\n");
        numOfCommand++;
        i += increase;
    }

}
//-----------------------------------------------------------------------------------------------
bool isCommandHaveArgument(int typeOfOperation, TypeOfOperation* oper){
    assert(oper);

    TypeOfOperation arrOfCommandsWithoutArg[] = {ADD, SUB, DIV, MUL, SIN, SQRT, 
                                                 IN, RET, DRAW, OUT, HLT, DEFAULT, ERROR}; 
    
    TypeOfOperation arrOfCommandsWithArg[] =    {PUSH, POPR, PSHR, JMP, JA, JAE, JB,
                                                 JBE, JE, JNE, TAG, CALL, PSHM, POPM};                                         

    size_t sizeOfArrOfComWithoutArg  = sizeof(arrOfCommandsWithoutArg) / sizeof(arrOfCommandsWithoutArg[0]);
    size_t sizeOfArrOfComWithArg  =    sizeof(arrOfCommandsWithArg) / sizeof(arrOfCommandsWithArg[0]);

    for(size_t i = 0; i < sizeOfArrOfComWithoutArg; i++){
        if(typeOfOperation == arrOfCommandsWithoutArg[i]){
            *oper = arrOfCommandsWithoutArg[i];
            return false;
        }
    }

    for(size_t i = 0; i < sizeOfArrOfComWithArg; i++){
        if(typeOfOperation == arrOfCommandsWithArg[i]){
            *oper = arrOfCommandsWithArg[i];
            return true;
        }
    }

    return false;    
}
//---------------------------------------------------------------------------------------------------------
#define GET_OPERATION(operation)    \
    case operation:                 \
        *strOp = #operation;  \
        break;

void getStringEnum(TypeOfOperation* typeOfOperation, const char** strOp){
    assert(typeOfOperation);
    assert(strOp);
    assert(*strOp);

     switch(*typeOfOperation){        
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
            GET_OPERATION(JA);
            GET_OPERATION(JAE);
            GET_OPERATION(JB);
            GET_OPERATION(JBE);
            GET_OPERATION(JE);
            GET_OPERATION(JNE);
            GET_OPERATION(TAG);
            GET_OPERATION(IN);
            GET_OPERATION(CALL);
            GET_OPERATION(DRAW);
            GET_OPERATION(RET);
            GET_OPERATION(OUT);
            GET_OPERATION(HLT);
            GET_OPERATION(DEFAULT);
            GET_OPERATION(ERROR);
            default:
                break;
        }
}

#undef GET_OPERATION
