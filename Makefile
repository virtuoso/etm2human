CFLAGS := -g3 -O0
LDFLAGS :=
SRCS := etmtest.c stream.c
OBJS := $(SRCS:.c=.o)

CC ?= $(CROSS)gcc
LD ?= $(CROSS)ld

all: etmtest

etmtest: $(OBJS)
%.o: %.c etmtest.h
	$(CC) $(CFLAGS) -o $@ -c $<

korpus: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

clean:
	rm -f $(OBJS) etmtest

