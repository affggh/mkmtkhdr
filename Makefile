CC = gcc
AR = ar rcv
ifeq ($(windir),)
EXE =
RM = rm -f
else
EXE = .exe
RM = del
endif

CFLAGS = -ffunction-sections -O3
LDFLAGS = -Wl,--gc-sections

all:mkmtkhdr$(EXE)

static:mkmtkhdr-static$(EXE)

mkmtkhdr$(EXE):mkmtkhdr.o
	$(CROSS_COMPILE)$(CC) -o $@ $^ -L. $(LDFLAGS) -s

mkmtkhdr-static$(EXE):mkmtkhdr.o
	$(CROSS_COMPILE)$(CC) -o $@ $^ -L. $(LDFLAGS) -static -s

mkmtkhdr.o:mkmtkhdr.c
	$(CROSS_COMPILE)$(CC) -o $@ $(CFLAGS) -c $< -I. -Werror

clean:
	$(RM) mkmtkhdr mkmtkhdr-static mkmtkhdr.o mkmtkhdr.exe mkmtkhdr-static.exe
	$(RM) Makefile.~

