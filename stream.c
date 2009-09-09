#include <stdlib.h>
#include <stdio.h>
#include "output.h"

const char *pkttypenames_v2[] = {
	"normal data",
	/* skip 4 */
	"value not traced",
	"context ID",
	/* skip 1 */
	"load miss",
	"load miss data",
};

const char *pkttypenames_v3[] = {
	"normal data",
	/* skip 4 */
	"value not traced",
	"context ID",
	/* skip 1 */
	"load miss",
	"load miss data",
	/* XXX */
	"P-header",
};

struct pkttype {
	unsigned char mask;
	unsigned char val;
	int type;
};

struct pkttype pkttypes_v2[] = {
	{ .mask = 0x53, .val = 0x2,  .type = 0 },
	{ .mask = 0x7f, .val = 0x6a, .type = 1 },
	{ .mask = 0x7f, .val = 0x6e, .type = 2 },
	{ .mask = 0x53, .val = 0x50, .type = 3 },
	{ .mask = 0x13, .val = 0x0,  .type = 4 },
};

struct pkttype pkttypes_v3[] = {
	{ .mask = 0x53, .val = 0x2,  .type = 0 },
	{ .mask = 0x7f, .val = 0x6a, .type = 1 },
	{ .mask = 0x7f, .val = 0x6e, .type = 2 },
	{ .mask = 0x53, .val = 0x50, .type = 3 },
	{ .mask = 0x13, .val = 0x0,  .type = 4 },
	/* XXX */
	{ .mask = 0x80, .val = 0x80,  .type = 5 },
};

#define arrsz(x) (sizeof(x)/sizeof(x[0]))
#define pkttypes pkttypes_v3
#define pkttypenames pkttypenames_v3

void stream_decode(const unsigned char *buffer, size_t bufsz)
{
	int cur, i;
	int percycle = 0;

	for (cur = 0; cur < bufsz; cur++) {
		int lastincycle;

		DBG("# Got %02x:\n", buffer[cur]);

		lastincycle = !(buffer[cur] & 0x80);
		percycle += !lastincycle;

		for (i = 0; i < arrsz(pkttypes); i++) {
			if ((buffer[cur] & pkttypes[i].mask) == pkttypes[i].val) {
				DBG("## type: %s\n",
						pkttypenames[pkttypes[i].type]
				   );
				break;
			}
		}
		if (i == arrsz(pkttypes)) {
			DBG("## assuming branch address\n");
		}

		if (lastincycle) {
			DBG("--- packets in this cycle: %d\n", percycle);
			percycle = 0;
		}
	}

	DBG("\n");
}

