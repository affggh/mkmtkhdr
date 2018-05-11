ifeq ($(CC),cc)
CC = gcc
endif
AR = ar rcv
ifeq ($(windir),)
EXE =
RM = rm -f
else
EXE = .exe
RM = del
endif

CFLAGS = -ffunction-sections -O3

ifneq (,$(findstring darwin,$(CROSS_COMPILE)))
    UNAME_S := Darwin
else
    UNAME_S := $(shell uname -s)
endif
ifeq ($(UNAME_S),Darwin)
    LDFLAGS += -Wl,-dead_strip
else
    LDFLAGS += -Wl,--gc-sections -s
endif

all:mkmtkhdr$(EXE)

static:
	$(MAKE) LDFLAGS="$(LDFLAGS) -static"

mkmtkhdr$(EXE):mkmtkhdr.o
	$(CROSS_COMPILE)$(CC) -o $@ $^ -L. $(LDFLAGS)

mkmtkhdr.o:mkmtkhdr.c
	$(CROSS_COMPILE)$(CC) -o $@ $(CFLAGS) -c $< -I. -Werror

clean:
	$(RM) mkmtkhdr
	$(RM) *.~ *.exe *.o

