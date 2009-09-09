#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "output.h"

static const char *file_in;
static unsigned char *buffer_in;
static int buffer_len;

int main(int argc, const char **argv)
{
	FILE *f;
	struct stat sb;
	int r;
	unsigned char *p;

	if (argc != 2) {
		ERR("Need a trace buffer\n");
		exit(EXIT_FAILURE);
	}

	file_in = argv[1];
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

	stream_decode(buffer_in, buffer_len);

	return 0;
}

