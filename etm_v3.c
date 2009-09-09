#include <stdlib.h>
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
	return 1;
}

FALLBACK_DECODE_FN(alignment_sync);
FALLBACK_DECODE_FN(cycle_count);
FALLBACK_DECODE_FN(insn_flow_sync);
FALLBACK_DECODE_FN(trigger);
FALLBACK_DECODE_FN(ooo_data);
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
FALLBACK_DECODE_FN(p_header);

struct pkttype **pkttypes = pkttypes_v3;

