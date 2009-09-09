#include <stdlib.h>
#include <stdio.h>
#include "output.h"
#include "etmproto.h"

void stream_decode(const unsigned char *buffer, size_t bufsz)
{
	int cur, i;
	int percycle = 0;

	for (cur = 0; cur < bufsz; cur++) {
		int lastincycle;

		DBG("# Got %02x:\n", buffer[cur]);

		lastincycle = !(buffer[cur] & 0x80);
		percycle += !lastincycle;

		for (i = 0; pkttypes[i]; i++) {
			if ((buffer[cur] & pkttypes[i]->mask) == pkttypes[i]->val) {
				DBG("## type: %s\n", pkttypes[i]->name);
				break;
			}
		}
		if (!pkttypes[i]) {
			DBG("## assuming branch address\n");
		}

		if (lastincycle) {
			DBG("--- packets in this cycle: %d\n", percycle);
			percycle = 0;
		}
	}

	DBG("\n");
}

