#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include "output.h"
#include "stream.h"

unsigned int verbosity;

static const char *file_in;
static unsigned char *buffer_in;
static int buffer_len;
static int hex_input;
static struct stream stream;

static const struct option options[] = {
	{ "input",			1, 0, 'i' },
	{ "hex-input",			1, 0, 'H' },
	{ "context",			1, 0, 'c' },
	{ "force-cycle-accurate",	0, 0, 'C' },
	{ "debug",			1, 0, 'D' },
	{ "reverse",			0, 0, 'R' },
	{ "help",			0, 0, 'h' },
	{ NULL,				0, 0, 0   },
};

static const char *optdesc[] = {
	"specify binary trace data input file",
	"specify hex trace data input file",
	"context ID width in the trace",
	"trace is cycle-accurate",
	"turn on debugging of certain parts of etm2human",
	"for a hex input, reverse the byte order",
	"display this help text"
};

void usage(void)
{
	int i;

	for (i = 0; options[i].name; i++)
		fprintf(stderr, "-%c, --%s\n\t%s\n",
				options[i].val, options[i].name, optdesc[i]);
}

static const char *optstr = "i:H:c:CRD:h";

int read_hex(void)
{
	FILE *f;
	unsigned char *p = buffer_in;
	int i = 0;

	f = fopen(file_in, "r");
	while (!feof(f)) {
		unsigned int c;
		int r;

		r = fscanf(f, "%02x", &c);
		if (r != 1) {
			if (feof(f)) break;

			ERR("Can't read from %s: %m\n", file_in);
			return -1;
		}

#if 0
		/* fix formatter's stuff */
		if (i & 1)
			p[i] = (unsigned char)c;
		else {
			if (c & 1) {
				ERR("Got trace source ID, i=%d, c=%02x\n", i, c);
				exit(EXIT_FAILURE);
			}

			p[i] = (unsigned char)c | (p[ (i & ~0xf) + 0xf ] & (i >> 2));
		}

		DBG("%02x", p[i]);
#endif
		/* reverse bytes in an incorrectly acquired trace */
		if (stream.reverse)
			p[(i & ~3) + 3 - (i & 3)] = (unsigned char)c;
		else
			p[i] = (unsigned char)c;
		i++;
	}
	fclose(f);

	return 0;
}

int read_raw(void)
{
	int fd;

	fd = open(file_in, O_RDONLY);
	if (fd == -1) {
		ERR("Can't read from %s: %m\n", file_in);
		return -1;
	}

	if (read(fd, buffer_in, buffer_len) != buffer_len) {
		ERR("Problem reading from %s: %m\n", file_in);
		return -1;
	}

	close(fd);

	return 0;
}

int main(int argc, char *const argv[])
{
	struct stat sb;
	int r, loptidx, c;

	for (;;) {
		c = getopt_long(argc, argv, optstr, options, &loptidx);
		if (c == -1)
			break;

		switch (c) {
			case 'H':
				hex_input++;
			case 'i':
				file_in = strdup(optarg);
				break;
			case 'c':
				stream.context_sz = atoi(optarg);

				/* TODO: validate context_sz */
				DBG("Context size: %d\n", stream.context_sz);
				break;
			case 'C':
				stream.cycle_accurate = 1;
				DBG("Forcing cycle-accurate trace mode\n");
				break;
			case 'R':
				stream.reverse = 1;
				break;
			case 'D':
				verbosity = atoi(optarg);
				verbosity &= DBG_MASK;
				DBG("Debug verbosity set to %d\n", verbosity);
				break;
			case 'h':
				usage();
				exit(EXIT_SUCCESS);
			default:
				ERR("Unknown argument: %c\n", c);
		}
	}

	if (argc != optind || !file_in) {
		ERR("Invalid arguments or missing input file.\n");
		usage();
		exit(EXIT_FAILURE);
	}

	DBG("Reading %s\n", file_in);

	r = stat(file_in, &sb);
	if (r == -1) {
		ERR("Can't stat %s: %m\n", file_in);
		exit(EXIT_FAILURE);
	}

	buffer_len = sb.st_size / 2 + 1;
	buffer_in = malloc(buffer_len);
	memset(buffer_in, 0, buffer_len);

	if (hex_input)
		read_hex();
	else
		read_raw();

	stream.buffer = buffer_in;
	stream.buffer_len = buffer_len;
	stream.state = SST_READ; /* -> SST_READ */

	stream_decode(&stream);
	if (stream.state == SST_DECODED)
		SAY("Trace stream decoding commenced.\n");

	return 0;
}

