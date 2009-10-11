CFLAGS := -g3 -O2 -Wall
LDFLAGS :=
SRCS := etmtest.c stream.c etm_v3.c tracer.c
OBJS := $(SRCS:.c=.o)

CC ?= $(CROSS)gcc
LD ?= $(CROSS)ld

DESTDIR ?= $(HOME)

all: etm2human

etm2human: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

%.o: %.c etmtest.h stream.h tracer.h version.h
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(OBJS) etm2human

install:
	cp etm2human $(DESTDIR)/bin
	chmod 0755 $(DESTDIR)/bin/etm2human

.PHONY: install clean all
