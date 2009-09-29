#ifndef __TRACER_H__
#define __TRACER_H__

#define INSN_COND_OK	1
#define INSN_NOOPCODE	2

struct insn {
	uint32_t opcode;
	uint32_t addr;
	unsigned int flags;
	int cycle;
};

#define INSN_QUEUE_MAX 4096

enum {
	TST_NONE = 0,
	TST_INIT,
	TST_INSYNC,
};

struct tracer {
	int state;
	uint32_t addr;
	uint32_t next_addr;
	uint32_t ctxid;
	int cycle;
	struct insn insns[INSN_QUEUE_MAX];
	int cur_insn;
};

struct tracer *tracer_init(void);
void tracer_done(struct tracer *t);
void tracer_sync(struct tracer *t, uint32_t addr, int cycle, uint32_t ctxid);
void tracer_next_cycle(struct tracer *t, int n);
void tracer_add_insn(struct tracer *t, int cond, int cycles);
void tracer_branch(struct tracer *t, uint32_t addr, int bsz);

#endif /* __TRACER_H__ */
