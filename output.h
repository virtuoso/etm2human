#ifndef __OUTPUT_H__
#define __OUTPUT_H__

#define SAY(f, args...) \
	fprintf(stdout, f, ## args)
#define DBG(f, args...) \
	fprintf(stderr, f, ## args)
#define ERR(f, args...) \
	fprintf(stderr, "[!!!] " f, ## args)

#endif /* __OUTPUT_H__ */

