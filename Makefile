CFLAGS := -g3 -O0 -Wall
LDFLAGS :=
SRCS := etmtest.c stream.c etm_v3.c
OBJS := $(SRCS:.c=.o)

CC ?= $(CROSS)gcc
LD ?= $(CROSS)ld

all: etmtest

etmtest: $(OBJS)
%.o: %.c etmtest.h stream.h
	$(CC) $(CFLAGS) -o $@ -c $<

korpus: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

clean:
	rm -f $(OBJS) etmtest

