#ifndef __STREAM_H__
#define __STREAM_H__

enum {
	SST_NONE = 0,
	SST_READ,
	SST_SYNCING,
	SST_INSYNC,
	SST_DECODING,
	SST_DECODED,
};

struct stream {
	struct tracer *tracer;
	const unsigned char *buffer;
	int buffer_len;
	int cycle_accurate:1;
	int reverse;
	int context_sz;
	int state;
};

#endif /* __STREAM_H__ */

