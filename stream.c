#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "output.h"
#include "tracer.h"
#include "stream.h"
#include "etmproto.h"

void stream_decode(struct stream *stream)
{
	int cur, i;

	if (stream->state == SST_READ) {
		stream->state++; /* -> SST_SYNCING */
		cur = find_beginning(stream);
		if (cur < 0) {
			SAY("Failed to find a sync packet, proceeding on guesswork.\n");
			cur = 0;
			stream->state++; /* -> SST_INSYNC */
		}
	} else if (stream->state != SST_INSYNC) {
		ERR("Bad stream state (%d), exiting.\n", stream->state);
		return;
	}

	stream->state++; /* -> SST_DECODING */
	stream->tracer = tracer_init();

	for (; cur < stream->buffer_len;) {
		int res;
		unsigned char c = stream->buffer[cur];

		dbg(DBG_STREAM, "# Got %02x [off=%d]:\n", c, cur);

		for (i = 0; pkttypes[i]; i++) {
			if ((c & pkttypes[i]->mask) == pkttypes[i]->val) {
				dbg(DBG_STREAM, "## type: %s\n", pkttypes[i]->name);
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
	}
	stream->state++; /* SST_DECODED */
	tracer_flush(stream->tracer);
	tracer_done(stream->tracer);

	dbg(DBG_STREAM, "\n");
}

