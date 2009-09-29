#ifndef __ETMPROTO_H__
#define __ETMPROTO_H__

typedef unsigned char pkt_t;

struct pkttype {
	pkt_t mask;
	pkt_t val;
	int type;
	const char *name;
	int (*skip_bytes)(pkt_t);
	int (*decode)(const pkt_t *, struct stream *);
};

#define DECODE_FN_NAME(__name) \
	pt_ ## __name ## _decode
#define DECL_DECODE_FN(__name) \
	static int DECODE_FN_NAME(__name) (const pkt_t *stream, struct stream *s)
#define __FALLBACK_DECODE_FN(__name) \
	DECL_DECODE_FN(__name) { return 1; }
#define FALLBACK_DECODE_FN(__name) \
	DECL_DECODE_FN(__name) { DBG("IMPLEMENT ME! %s\n", __FUNCTION__); return 1; }

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

#define pdbg(f, args...)							\
	do {									\
		switch (s->state) {						\
			case SST_SYNCING:					\
				dbg(DBG_PROTO | DBG_STREAM, f, ## args);	\
				break;						\
			case SST_DECODING:					\
				dbg(DBG_PROTO, f, ## args);			\
				break;						\
			default:						\
				ERR("%s called in wrong state: %d\n",		\
						__FUNCTION__, s->state);	\
				dbg(DBG_PROTO, f, ## args);			\
		}								\
	} while (0)

/* etm_v3.c */
extern struct pkttype **pkttypes;
int find_beginning(struct stream *s);

#endif /* __ETMPROTO_H__ */

