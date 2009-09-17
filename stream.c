#include <stdlib.h>
#include <stdio.h>
#include "output.h"
#include "stream.h"
#include "etmproto.h"

void stream_decode(struct stream *stream)
{
	int cur, i;
	int percycle = 0;

	for (cur = 0; cur < stream->buffer_len;) {
		int lastincycle;
		int res;
		unsigned char c = stream->buffer[cur];

		DBG("# Got %02x [off=%d]:\n", c, cur);

		lastincycle = !(c & 0x80);
		percycle += !lastincycle;

		for (i = 0; pkttypes[i]; i++) {
			if ((c & pkttypes[i]->mask) == pkttypes[i]->val) {
				DBG("## type: %s\n", pkttypes[i]->name);
				break;
			}
		}

		if (!pkttypes[i]) {
			ERR("packet header is not recognized: %x\n", c);
			return;
		}

		res = pkttypes[i]->decode(&stream->buffer[cur], stream);
		if (res <= 0) {
			ERR("failed to decode packet: %x", c);
			for (i = 0; i < -res; i++)
				ERR("%x", stream->buffer[cur + i]);
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

