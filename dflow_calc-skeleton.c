/* 046267 Computer Architecture - Winter 20/21 - HW #3               */
/* Implementation (skeleton)  for the dataflow statistics calculator */

#include <minmax.h>
#include "dflow_calc.h"

#define NUM_REGS 32
#define UNDEFINED -1
#define ZERO 0
#define SUCCESS ZERO

typedef struct {
	int dependency_1;
	int dependency_2;
	int running_time;
	int waiting_time;
}commandAns;

typedef struct {
	int regs[NUM_REGS];
	commandAns* command_answers;
	int total_time;
	unsigned int num_insts;
}ProgCtx_t;

static inline int getWaitingTime(ProgCtx_t* new_prog,int i) {
    return max(new_prog->command_answers[i].dependency_1 == UNDEFINED ? ZERO :
               new_prog->command_answers[new_prog->command_answers[i].dependency_1].waiting_time +
               new_prog->command_answers[new_prog->command_answers[i].dependency_1].running_time,
               new_prog->command_answers[i].dependency_2 == UNDEFINED ? ZERO :
               new_prog->command_answers[new_prog->command_answers[i].dependency_2].waiting_time +
               new_prog->command_answers[new_prog->command_answers[i].dependency_2].running_time);
}

ProgCtx analyzeProg(const unsigned int opsLatency[], const InstInfo progTrace[], unsigned int numOfInsts) {
    ///init
    ProgCtx_t* new_prog = malloc(sizeof(*new_prog));
	if(!new_prog){
		return PROG_CTX_NULL;
	}
	new_prog->command_answers = malloc(sizeof(*(new_prog->command_answers))*numOfInsts);
	if(!new_prog->command_answers){
		free(new_prog);
		return PROG_CTX_NULL;
	}	
	for(int i=0;i<numOfInsts;i++){
		new_prog->command_answers[i].dependency_1=UNDEFINED;
		new_prog->command_answers[i].dependency_2=UNDEFINED;
		new_prog->command_answers[i].running_time=opsLatency[progTrace[i].opcode];
		new_prog->command_answers[i].waiting_time=ZERO;
	}
	for(int i=0;i<NUM_REGS;i++)
	    new_prog->regs[i]=UNDEFINED;
    new_prog->total_time=ZERO;
    new_prog->num_insts=numOfInsts;

	///calc
    for(int i=0;i<numOfInsts;i++) {
        new_prog->command_answers[i].dependency_1 = new_prog->regs[progTrace[i].src1Idx];
        new_prog->command_answers[i].dependency_2 = new_prog->regs[progTrace[i].src2Idx];
        new_prog->regs[progTrace[i].dstIdx] = i;
        new_prog->command_answers[i].waiting_time = getWaitingTime(new_prog, i);
        new_prog->total_time = max(new_prog->total_time, new_prog->command_answers[i].waiting_time + new_prog->command_answers[i].running_time);
    }
    return new_prog;
}

void freeProgCtx(ProgCtx ctx) {
    free(((ProgCtx_t*)ctx)->command_answers);
    free(ctx);
}

int getInstDepth(ProgCtx ctx, unsigned int theInst) {
    if(!ctx||theInst>=((ProgCtx_t*)ctx)->num_insts)
        return UNDEFINED;
    return ((ProgCtx_t*)ctx)->command_answers[theInst].waiting_time;
}

int getInstDeps(ProgCtx ctx, unsigned int theInst, int *src1DepInst, int *src2DepInst) {
    if(!ctx||theInst>=((ProgCtx_t*)ctx)->num_insts)
        return UNDEFINED;
    ProgCtx_t* our_ctx=(ProgCtx_t*)ctx;
    *src1DepInst=our_ctx->command_answers[theInst].dependency_1;
    *src2DepInst=our_ctx->command_answers[theInst].dependency_2;
    return SUCCESS;
}

int getProgDepth(ProgCtx ctx) {
    if(!ctx)
        return UNDEFINED;
    return ((ProgCtx_t*)ctx)->total_time;
}