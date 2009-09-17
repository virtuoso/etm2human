#ifndef __STREAM_H__
#define __STREAM_H__

struct stream {
	const unsigned char *buffer;
	int buffer_len;
	int cycle_accurate:1;
	int context_sz;
};

#endif /* __STREAM_H__ */

