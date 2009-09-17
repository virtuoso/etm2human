#include <stdlib.h>
#include <stdint.h>
#include <stdio.h> /* for printfs and stderrs */
#include "output.h"
#include "etmproto.h"

DECL_PKTTYPE(0x01, 0x01, branch_address, 3);
DECL_PKTTYPE(0xff, 0x00, alignment_sync, 3);
DECL_PKTTYPE(0xff, 0x04, cycle_count,    3);
DECL_PKTTYPE(0xff, 0x08, insn_flow_sync, 3);
DECL_PKTTYPE(0xff, 0x0c, trigger,        3);
DECL_PKTTYPE(0x93, 0x00, ooo_data,       3);
DECL_PKTTYPE(0xff, 0x50, store_failed,   3);
DECL_PKTTYPE(0xff, 0x70, isync_cycle,    3);
DECL_PKTTYPE(0xd3, 0x50, ooo_placehldr,  3);
DECL_PKTTYPE(0xd1, 0x10, reserved0,      3);
DECL_PKTTYPE(0xd3, 0x02, normal_data,    3);
DECL_PKTTYPE(0xe3, 0x42, reserved1,      3);
DECL_PKTTYPE(0xff, 0x62, data_suppressd, 3);
DECL_PKTTYPE(0xff, 0x66, ignore,         3);
DECL_PKTTYPE(0xef, 0x6a, val_not_traced, 3);
DECL_PKTTYPE(0xff, 0x6e, context_id,     3);
DECL_PKTTYPE(0xf3, 0x72, reserved2,      3);
DECL_PKTTYPE(0x81, 0x80, p_header,       3);

static struct pkttype *pkttypes_v3[] = {
	&PKTTYPE_NAME(branch_address, 3),
	&PKTTYPE_NAME(alignment_sync, 3),
	&PKTTYPE_NAME(cycle_count,    3),
	&PKTTYPE_NAME(insn_flow_sync, 3),
	&PKTTYPE_NAME(trigger,        3),
	&PKTTYPE_NAME(ooo_data,       3),
	&PKTTYPE_NAME(store_failed,   3),
	&PKTTYPE_NAME(isync_cycle,    3),
	&PKTTYPE_NAME(ooo_placehldr,  3),
	&PKTTYPE_NAME(reserved0,      3),
	&PKTTYPE_NAME(normal_data,    3),
	&PKTTYPE_NAME(reserved1,      3),
	&PKTTYPE_NAME(data_suppressd, 3),
	&PKTTYPE_NAME(ignore,         3),
	&PKTTYPE_NAME(val_not_traced, 3),
	&PKTTYPE_NAME(context_id,     3),
	&PKTTYPE_NAME(reserved2,      3),
	&PKTTYPE_NAME(p_header,       3),
	NULL,
};

DECL_DECODE_FN(branch_address)
{
	int idx, i;
	uint32_t addr = 0;

	for (idx = 0; idx < 4; idx++) {
		addr |= (stream[idx] & 0x7f) << (7 * idx + 2);

		if (!(stream[idx] & 0x80))
			break;
	}

	if (idx == 4)
		addr |= (stream[idx] & 0x07) << 29;

	//addr &= 0xfffffffc;
	DBG("----- got branch address: %x\n\tpacket: ", addr);
	for (i = 0; i <= idx; i++)
		DBG("%x", stream[i]);
	DBG("\n");

	return idx + 1;
}

FALLBACK_DECODE_FN(alignment_sync);

DECL_DECODE_FN(cycle_count)
{
	int idx;
	uint32_t cycle_count = 0;

	for (idx = 1; idx < 5; idx++) {
		cycle_count |= (stream[idx] & 0x7f) << (7 * (idx - 1));

		if (!(stream[idx] & 0x80))
			break;
	}

	if (idx == 5)
		cycle_count |= (stream[idx] & 0x0f) << 28;
	DBG("----- got cycle count: %d\n", cycle_count);

	return idx + 1;
}

FALLBACK_DECODE_FN(insn_flow_sync);
FALLBACK_DECODE_FN(trigger);

DECL_DECODE_FN(ooo_data)
{
	int idx, sz, tag, i;
	uint32_t data = 0;

	sz = (stream[0] & 0xc) >> 2;
	tag = (stream[0] & 0x60) >> 5;
	for (idx = 0; idx < sz; idx++)
		data |= stream[idx + 1] << (8 * idx);

	DBG("----- got data: %x [size=%d, tag=%x]\n\tpacket: ", data,
			sz, tag);
	for (i = 0; i < sz + 1; i++)
		DBG("%x", stream[i]);
	DBG("\n");

	return idx + 1;
}

FALLBACK_DECODE_FN(store_failed);
FALLBACK_DECODE_FN(isync_cycle);
FALLBACK_DECODE_FN(ooo_placehldr);
FALLBACK_DECODE_FN(reserved0);
FALLBACK_DECODE_FN(normal_data);
FALLBACK_DECODE_FN(reserved1);
FALLBACK_DECODE_FN(data_suppressd);
FALLBACK_DECODE_FN(ignore);
FALLBACK_DECODE_FN(val_not_traced);
FALLBACK_DECODE_FN(context_id);
FALLBACK_DECODE_FN(reserved2);

DECL_DECODE_FN(p_header)
{
	int n_atoms = 0, e_atoms = 0, w_atoms = 0;

	switch (stream[0] & 0xa3) {
		case 0x80:
			if (stream[0] == 0x80) {
				DBG("\tformat 0 p-header\n");
				break;
			}

			n_atoms = (stream[0] & 0x40) >> 6;
			e_atoms = (stream[0] & 0x3c) >> 2;
			DBG("\tformat 1 p-header [e=%d n=%d]\n", e_atoms, n_atoms);
			break;
		case 0xa0:
			DBG("\tformat 1 or 3 p-header\n");
			break;
		case 0x82:
			DBG("\tformat 2 p-header\n");
			break;
		default:
			DBG("\tWTF\n");
	}

	return 1;
}

struct pkttype **pkttypes = pkttypes_v3;

