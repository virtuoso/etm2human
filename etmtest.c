#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <getopt.h>
#include "output.h"
#include "stream.h"

static const char *file_in;
static unsigned char *buffer_in;
static int buffer_len;
static struct stream stream;

static const struct option options[] = {
	{ "input",			1, 0, 'i' },
	{ "context",			1, 0, 'c' },
	{ "force-cycle-accurate",	0, 0, 'C' },
	{ NULL,				0, 0, 0   },
};

static const char *optstr = "i:c:C";

int main(int argc, char *const argv[])
{
	FILE *f;
	struct stat sb;
	int r, loptidx, c;
	unsigned char *p;

	for (;;) {
		c = getopt_long(argc, argv, optstr, options, &loptidx);
		if (c == -1)
			break;

		switch (c) {
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
			default:
				ERR("Unknown argument: %c\n", c);
		}
	}

	if (argc != optind || !file_in) {
		ERR("Fix your arguments, for more details check the code\n");
		exit(EXIT_FAILURE);
	}

	DBG("Reading %s\n", file_in);

	r = stat(file_in, &sb);
	if (r == -1) {
		ERR("Can't stat %s: %m\n", file_in);
		exit(EXIT_FAILURE);
	}

	buffer_len = sb.st_size / 2 + 1;
	p = buffer_in = malloc(buffer_len);
	memset(buffer_in, 0, buffer_len);

	f = fopen(file_in, "r");
	while (!feof(f)) {
		unsigned int c;

		r = fscanf(f, "%02x", &c);
		if (r != 1) {
			if (feof(f)) break;

			ERR("Can't read from %s: %m\n", file_in);
			exit(EXIT_FAILURE);
		}

		*p++ = (unsigned char)c;
	}
	fclose(f);

	stream.buffer = buffer_in;
	stream.buffer_len = buffer_len;

	stream_decode(&stream);

	return 0;
}

