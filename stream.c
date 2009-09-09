#include <stdlib.h>
#include <stdio.h>
#include "output.h"
#include "etmproto.h"

void stream_decode(const unsigned char *buffer, size_t bufsz)
{
	int cur, i;
	int percycle = 0;

	for (cur = 0; cur < bufsz;) {
		int lastincycle;
		int res;

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
			ERR("packet header is not recognized: %x\n", buffer[cur]);
			return;
		}

		res = pkttypes[i]->decode(&buffer[cur], 0);
		if (res <= 0) {
			ERR("failed to decode packet: %x", buffer[cur]);
			for (i = 0; i < -res; i++)
				ERR("%x", buffer[cur + i]);
			ERR("\n");
			return;
		}

		cur += res;
		if (lastincycle) {
			DBG("--- packets in this cycle: %d\n", percycle);
			percycle = 0;
		}
	}

	DBG("\n");
}

