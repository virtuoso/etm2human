#include "etmproto.h"

const char *pkttypenames_v2[] = {
	"normal data",
	/* skip 4 */
	"value not traced",
	"context ID",
	/* skip 1 */
	"load miss",
	"load miss data",
};

struct pkttype pkttypes_v2[] = {
	{ .mask = 0x53, .val = 0x2,  .type = 0 },
	{ .mask = 0x7f, .val = 0x6a, .type = 1 },
	{ .mask = 0x7f, .val = 0x6e, .type = 2 },
	{ .mask = 0x53, .val = 0x50, .type = 3 },
	{ .mask = 0x13, .val = 0x0,  .type = 4 },
};

