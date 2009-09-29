#include <stdlib.h>
#include <stdint.h>
#include <stdio.h> /* for printfs and stderrs */
#include "output.h"
#include "tracer.h"
#include "stream.h"
#include "etmproto.h"

DECL_PKTTYPE(0x01, 0x01, branch_address, 3);
DECL_PKTTYPE(0xff, 0x00, alignment_sync, 3);
DECL_PKTTYPE(0xff, 0x04, cycle_count,    3);
DECL_PKTTYPE(0xff, 0x08, isync,          3);
DECL_PKTTYPE(0xff, 0x0c, trigger,        3);
DECL_PKTTYPE(0x93, 0x00, ooo_data,       3);
DECL_PKTTYPE(0xff, 0x50, store_failed,   3);
DECL_PKTTYPE(0xff, 0x70, isync_cycle,    3);
DECL_PKTTYPE(0xd3, 0x50, ooo_placehldr,  3);
DECL_PKTTYPE(0xd3, 0x12, reserved0,      3);
DECL_PKTTYPE(0xf3, 0x10, reserved1,      3);
DECL_PKTTYPE(0xf3, 0x30, reserved2,      3);
DECL_PKTTYPE(0xd3, 0x02, normal_data,    3);
DECL_PKTTYPE(0xe3, 0x42, reserved3,      3);
DECL_PKTTYPE(0xff, 0x62, data_suppressd, 3);
DECL_PKTTYPE(0xff, 0x66, ignore,         3);
DECL_PKTTYPE(0xef, 0x6a, val_not_traced, 3);
DECL_PKTTYPE(0xff, 0x6e, context_id,     3);
DECL_PKTTYPE(0xff, 0x76, exception_exit, 3);
DECL_PKTTYPE(0xff, 0x7e, exception_ent,  3);
DECL_PKTTYPE(0xf3, 0x72, reserved4,      3);
DECL_PKTTYPE(0x81, 0x80, p_header,       3);

static struct pkttype *pkttypes_v3[] = {
	&PKTTYPE_NAME(branch_address, 3),
	&PKTTYPE_NAME(alignment_sync, 3),
	&PKTTYPE_NAME(cycle_count,    3),
	&PKTTYPE_NAME(isync,          3),
	&PKTTYPE_NAME(trigger,        3),
	&PKTTYPE_NAME(ooo_data,       3),
	&PKTTYPE_NAME(store_failed,   3),
	&PKTTYPE_NAME(isync_cycle,    3),
	&PKTTYPE_NAME(ooo_placehldr,  3),
	&PKTTYPE_NAME(reserved0,      3),
	&PKTTYPE_NAME(reserved1,      3),
	&PKTTYPE_NAME(reserved2,      3),
	&PKTTYPE_NAME(normal_data,    3),
	&PKTTYPE_NAME(reserved3,      3),
	&PKTTYPE_NAME(data_suppressd, 3),
	&PKTTYPE_NAME(ignore,         3),
	&PKTTYPE_NAME(val_not_traced, 3),
	&PKTTYPE_NAME(context_id,     3),
	&PKTTYPE_NAME(exception_exit, 3),
	&PKTTYPE_NAME(exception_ent,  3),
	&PKTTYPE_NAME(reserved4,      3),
	&PKTTYPE_NAME(p_header,       3),
	NULL,
};

DECL_DECODE_FN(branch_address)
{
	int idx, i, bsz = 0;
	uint32_t addr = 0;

	for (idx = 0; idx < 4; idx++) {
		addr |= (stream[idx] & 0x7f) << (7 * idx + 1);
		bsz += 7;

		if (!(stream[idx] & 0x80))
			break;
	}

	if (idx == 4) {
		addr |= (stream[idx] & 0x07) << 29;
		bsz = 31;
	}

	addr &= 0xfffffffc;
	pdbg("----- got branch address: %x\n\tpacket: ", addr);

	tracer_branch(s->tracer, addr, bsz);
	for (i = 0; i <= idx; i++)
		pdbg("%02x", stream[i]);
	pdbg("\n");

	return idx + 1;
}

__FALLBACK_DECODE_FN(alignment_sync);

DECL_DECODE_FN(cycle_count)
{
	int idx, i;
	uint32_t cycle_count = 0;

	for (idx = 1; idx < 5; idx++) {
		cycle_count |= (stream[idx] & 0x7f) << (7 * (idx - 1));

		if (!(stream[idx] & 0x80))
			break;
	}

	if (idx == 5)
		cycle_count |= (stream[idx] & 0x0f) << 28;
	pdbg("----- got cycle count: %d\n\tpacket: ", cycle_count);
	for (i = 0; i <= idx; i++)
		pdbg("%02x", stream[i]);
	pdbg("\n");

	return idx + 1;
}

static const char *isync_reason[] = {
	"periodic i-sync",
	"tracing enabled",
	"overflow",
	"left debug",
};

DECL_DECODE_FN(isync)
{
	int idx, infobyte, i;
	uint32_t ctxid = 0;
	uint32_t addr = 0;

	for (i = 0, idx = 1; i < s->context_sz; i++, idx++)
		ctxid |= stream[idx] << (8 * i);

	pdbg("----- got context id: %x\n", ctxid);

	infobyte = stream[idx++];
	pdbg("----- got infobyte: %x\n", infobyte);

	for (i = 0; i < 4; i++, idx++)
		addr |= stream[idx] << (8 * i);

	pdbg("----- got address: %x\n\t", addr);
	for (i = 0; i < idx; i++)
		pdbg("%02x", stream[i]);
	pdbg("\n");

	return idx;
}

FALLBACK_DECODE_FN(trigger);

DECL_DECODE_FN(ooo_data)
{
	int idx, sz, tag, i;
	uint32_t data = 0;

	sz = (stream[0] & 0xc) >> 2;
	tag = (stream[0] & 0x60) >> 5;
	for (i = 0, idx = 1; i < sz; i++, idx++)
		data |= stream[idx] << (8 * i);

	pdbg("----- got data: %x [size=%d, tag=%x]\n\tpacket: ", data,
			sz, tag);
	for (i = 0; i < idx; i++)
		pdbg("%02x", stream[i]);
	pdbg("\n");

	return idx;
}

FALLBACK_DECODE_FN(store_failed);

DECL_DECODE_FN(isync_cycle)
{
	int idx, infobyte, i;
	uint32_t ctxid = 0;
	uint32_t addr = 0;
	uint32_t cycle_count = 0;

	pdbg("===== trace begins here =====\n");
	for (i = 0, idx = 1; i < 4; i++, idx++) {
		cycle_count |= (stream[idx] & 0x7f) << (7 * i);

		if (!(stream[idx] & 0x80))
			break;
	}

	if (idx == 5)
		cycle_count |= (stream[idx] & 0x0f) << 28;
	pdbg("----- got cycle count: %d\n", cycle_count);

	for (i = 0, idx++; i < s->context_sz; i++, idx++)
		ctxid |= stream[idx] << (8 * i);

	pdbg("----- got context id: %x\n", ctxid);

	infobyte = stream[idx++];
	pdbg("----- got infobyte: %x\n", infobyte);

	if ((infobyte & 0x9f) != 0x9) {
		pdbg("invalid infobyte, skipping\n");
		return 0;
	}

	for (i = 0; i < 4; i++, idx++)
		addr |= stream[idx] << (8 * i);

	pdbg("----- got address: %x\n\t", addr);
	for (i = 0; i < idx; i++)
		pdbg("%02x", stream[i]);
	pdbg("\n");

	if (s->state == SST_DECODING)
		tracer_sync(s->tracer, addr, cycle_count, ctxid);

	return idx;
}

DECL_DECODE_FN(ooo_placehldr)
{
	int idx = 0, i;
	uint32_t addr = 0;

	if (stream[0] & 0x20) {
		for (idx = 1; idx < 5; idx++) {
			addr |= (stream[idx] & 0x7f) << (7 * (idx - 1));

			if (!(stream[idx] & 0x80))
				break;
		}

		if (idx == 5)
			addr |= (stream[idx] & 0x0f) << 28;
		pdbg("----- got address: %x\n\tpacket: ", addr);
	} else
		pdbg("----- no address given\n\tpacket: ");

	for (i = 0; i <= idx; i++)
		pdbg("%02x", stream[i]);
	pdbg("\n");

	return idx + 1;
}

FALLBACK_DECODE_FN(reserved0);
FALLBACK_DECODE_FN(reserved1);
FALLBACK_DECODE_FN(reserved2);

DECL_DECODE_FN(normal_data) /* XXX: assumes address only tracing */
{
	int idx = 0;
	uint32_t addr = 0;
	uint32_t data = 0;

	/* is data address included? */
	if (stream[0] & 0x20 /* && s->do_data_address */) {
		for (idx = 1; idx < 5; idx++) {
			addr |= (stream[idx] & 0x7f) << (7 * (idx - 1));

			if (!(stream[idx] & 0x80))
				break;
		}

		if (idx == 5)
			addr |= (stream[idx] & 0x0f) << 28;

		pdbg("----- got data address: %x\n", addr);
	} else
		pdbg("----- data address not included\n");

	return idx + 1;
}

FALLBACK_DECODE_FN(reserved3);
FALLBACK_DECODE_FN(data_suppressd);
FALLBACK_DECODE_FN(ignore);
FALLBACK_DECODE_FN(val_not_traced);

DECL_DECODE_FN(context_id)
{
	int idx, i;
	uint32_t ctxid = 0;

	for (idx = 1; idx < s->context_sz + 1; idx++)
		ctxid |= stream[idx] << (8 * (idx - 1));

	pdbg("----- got context id: %x\n\tpacket: ", ctxid);
	for (i = 0; i < idx; i++)
		pdbg("%02x", stream[i]);
	pdbg("\n");

	return idx;
}

FALLBACK_DECODE_FN(exception_exit);
FALLBACK_DECODE_FN(exception_ent);
FALLBACK_DECODE_FN(reserved4);

/*
 * p-headers
 */
struct p_header {
	int n_atoms;
	int e_atoms;
	int w_atoms;
};

/* non cycle-accurate mode */
static int ph_ncycacc_do(const pkt_t *stream, struct stream *s,
		struct p_header *p)
{
	p->n_atoms = p->e_atoms = p->w_atoms = 0;

	switch (stream[0] & 0x83) {
		case 0x80: /* format 1 */
			p->n_atoms = (stream[0] & 0x40) >> 6;
			p->e_atoms = (stream[0] & 0x3c) >> 2;
			pdbg("\tformat 1 p-header [e=%d n=%d]\n",
					p->e_atoms, p->n_atoms);
			break;

		case 0xa0: /* format 2 */
			if (stream[0] & 0x08)
				p->e_atoms++; /* push_e_atom */
			else
				p->n_atoms++; /* push_n_atom */
			if (stream[0] & 0x04)
				p->e_atoms++; /* push_e_atom */
			else
				p->n_atoms++; /* push_n_atom */
			pdbg("\tformat 2 p*-header [e=%d n=%d]\n",
					p->e_atoms, p->n_atoms);
			break;

		default:
			pdbg("\tWTF\n");
	}

	return 1;
}

/* cycle-accurate mode, p-header formats */
DECL_PKTTYPE(0xff, 0x80, p_header_ca_format0, 3);
DECL_PKTTYPE(0xa3, 0x80, p_header_ca_format1, 3);
DECL_PKTTYPE(0xf3, 0x82, p_header_ca_format2, 3);
DECL_PKTTYPE(0xa3, 0xa0, p_header_ca_format3, 3);
DECL_PKTTYPE(0xfb, 0x92, p_header_ca_format4, 3);
DECL_PKTTYPE(0xfd, 0x9a, p_header_ca_reserved0, 3);
DECL_PKTTYPE(0xe3, 0xa2, p_header_ca_reserved1, 3);
DECL_PKTTYPE(0xc3, 0xc2, p_header_ca_reserved2, 3);

static struct pkttype *pheaders_ca[] = {
	&PKTTYPE_NAME(p_header_ca_format0, 3),
	&PKTTYPE_NAME(p_header_ca_format1, 3),
	&PKTTYPE_NAME(p_header_ca_format2, 3),
	&PKTTYPE_NAME(p_header_ca_format3, 3),
	&PKTTYPE_NAME(p_header_ca_format4, 3),
	&PKTTYPE_NAME(p_header_ca_reserved0, 3),
	&PKTTYPE_NAME(p_header_ca_reserved1, 3),
	&PKTTYPE_NAME(p_header_ca_reserved2, 3),
	NULL,
};

__FALLBACK_DECODE_FN(p_header_ca_format0);

DECL_DECODE_FN(p_header_ca_format1)
{
	int i, _e, _n;

	_e = (stream[0] & 0x1c) >> 2;
	_n = (stream[0] & 0x40) >> 6;

	for (i = 0; i < _e; i++)
		tracer_add_insn(s->tracer, 1, 1);
	for (i = 0; i < _n; i++)
		tracer_add_insn(s->tracer, 0, 1);

	return 1;
}

DECL_DECODE_FN(p_header_ca_format2)
{
	int i;

	tracer_next_cycle(s->tracer, 1);
	for (i = 3; i > 1; i--)
		tracer_add_insn(s->tracer, !!(stream[0] & (1 << i)), 0);

	return 1;
}

DECL_DECODE_FN(p_header_ca_format3)
{
	int _w, _e;

	_w = (stream[0] & 0x1c) >> 2;
	_e = (stream[0] & 0x40) >> 6;

	tracer_next_cycle(s->tracer, _w);
	if (_e)
		tracer_add_insn(s->tracer, 1, 0);

	return 1;
}

__FALLBACK_DECODE_FN(p_header_ca_format4);
__FALLBACK_DECODE_FN(p_header_ca_reserved0);
__FALLBACK_DECODE_FN(p_header_ca_reserved1);
__FALLBACK_DECODE_FN(p_header_ca_reserved2);

static int ph_cycacc_do(const pkt_t *stream, struct stream *s,
		struct p_header *p)
{
	int i;
	pkt_t c = stream[0];

	p->n_atoms = p->e_atoms = p->w_atoms = 0;

	for (i = 0; pheaders_ca[i]; i++)
		if ((c & pheaders_ca[i]->mask) == pheaders_ca[i]->val) {
			pdbg("## type: %s\n", pheaders_ca[i]->name);
			break;
		}

	if (!pheaders_ca[i]) {
		ERR("unrecognized p-header: %x\n", c);
		return 0;
	}

	return pheaders_ca[i]->decode(stream, s);
}

DECL_DECODE_FN(p_header)
{
	struct p_header H;

	if (s->cycle_accurate)
		return ph_cycacc_do(stream, s, &H);
	else
		ph_ncycacc_do(stream, s, &H);

	return 1;
}

struct pkttype **pkttypes = pkttypes_v3;

int find_beginning(struct stream *s)
{
	int i, p;

	for (i = 0; i < s->buffer_len; i++) {
		p = DECODE_FN_NAME(isync_cycle)(&s->buffer[i], s);
		if (p != 0) {
			s->state++; /* SST_INSYNC */
			dbg(DBG_STREAM, "======= trace start found at %d ========\n", i);
			return i;
		}
	}

	return -1;
}

