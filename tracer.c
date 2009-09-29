#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "output.h"
#include "tracer.h"

void insn_init(struct insn *insn)
{
	insn->flags = INSN_NOOPCODE;
	insn->cycle = 0;
}

void insn_print(struct insn *insn)
{
	SAY("insn at %08x: ", insn->addr);

	if (insn->flags & INSN_NOOPCODE)
		SAY("X ");
	else
		SAY("%08x ", insn->opcode);

	SAY("cycle: %d cond: %s\n", insn->cycle,
			(insn->flags & INSN_COND_OK) ? "PASS" : "FAIL");
}

struct tracer *tracer_init(void)
{
	struct tracer *t;

	t = malloc(sizeof(struct tracer));
	if (!t) {
		ERR("Out of memory: %m\n");
		return NULL;
	}

	memset(t, 0, sizeof(struct tracer));
	t->state = TST_INIT;
	t->cur_insn = 0;

	return t;
}

void tracer_done(struct tracer *t)
{
	free(t);
}

void tracer_flush(struct tracer *t)
{
	int i;

	SAY("trace flow started at %08x, cycle %d, ctxid %x\n",
			t->addr, t->cycle, t->ctxid);
	for (i = 0; i < t->cur_insn; i++)
		insn_print(&t->insns[i]);
}

void tracer_sync(struct tracer *t, uint32_t addr, int cycles, uint32_t ctxid)
{
	dbg(DBG_DECODE, "trace sync: addr=%x, cycle=%d, ctxid=%x\n",
			addr, cycles, ctxid);
	if (t->state != TST_INIT)
		tracer_flush(t);
	t->cur_insn = 0;
	t->addr = addr;
	t->next_addr = addr;
	t->cycle = cycles;
	t->ctxid = ctxid;
	t->state = TST_INSYNC;
}

void tracer_next_cycle(struct tracer *t, int n)
{
	dbg(DBG_DECODE, "--->>> adding cycle gap %d\n", n);
	t->cycle += n;
}

void tracer_add_insn(struct tracer *t, int cond, int cycles)
{
	dbg(DBG_DECODE, "--->>> adding insn: at %d, cond %s\n",
			t->cycle, cond ? "PASS" : "FAIL");

	insn_init(&t->insns[t->cur_insn]);
	t->insns[t->cur_insn].cycle = t->cycle;
	t->insns[t->cur_insn].addr = t->next_addr;
	t->next_addr += 4;

	if (cond)
		t->insns[t->cur_insn].flags |= INSN_COND_OK;
	else
		t->insns[t->cur_insn].flags &= ~INSN_COND_OK;

	t->cur_insn++;
	t->cycle += cycles;
}

void tracer_branch(struct tracer *t, uint32_t addr, int bsz)
{
	t->next_addr &= ~((1 << (bsz + 1)) - 1);
	t->next_addr |= addr;
	dbg(DBG_DECODE, "--->>> branching to %x\n", t->next_addr);
}

