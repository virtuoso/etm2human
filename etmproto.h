#ifndef __ETMPROTO_H__
#define __ETMPROTO_H__

typedef unsigned char pkt_t;

struct pkttype {
	pkt_t mask;
	pkt_t val;
	int type;
	const char *name;
	int (*skip_bytes)(pkt_t);
	int (*decode)(pkt_t *, int);
};

#define DECODE_FN_NAME(__name) \
	pt_ ## __name ## _decode
#define DECL_DECODE_FN(__name) \
	static int DECODE_FN_NAME(__name) (pkt_t *stream, int npkts)
#define FALLBACK_DECODE_FN(__name) \
	DECL_DECODE_FN(__name) { return 0; }

#define PKTTYPE_NAME(__name, __ver) \
	pkttype_ ## v ## __ver ## _ ## __name

#define DECL_PKTTYPE(__mask, __val, __name, __ver) \
	DECL_DECODE_FN(__name); \
	struct pkttype PKTTYPE_NAME(__name, __ver) = { \
		.mask = (__mask), \
		.val = (__val), \
		.name = # __name, \
		.decode = DECODE_FN_NAME(__name), \
	}

/* etm_v3.c */
extern struct pkttype **pkttypes;

#endif /* __ETMPROTO_H__ */

