# This file requires GNU Make

mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir := $(patsubst %/,%,$(dir $(mkfile_path)))
HERE=$(mkfile_dir)

FOOTDIR=$(HERE)/..
FOOTLIB = dist/libfootag.a

CCOPT            =
CCOPT           += -O2

CFLAGS           =
CFLAGS          += $(CCOPT)
CFLAGS          += -g
CFLAGS          += -std=c99
CFLAGS          += -Wall
CFLAGS          += -Wextra
CFLAGS          += -Wmissing-prototypes
CFLAGS          += -Wstrict-prototypes
CFLAGS          += -Wmissing-declarations
CFLAGS          += -pedantic
CFLAGS          += -Idist/include

LDFLAGS          =
LDFLAGS         += -lm
LDFLAGS         += $(shell pkg-config --libs cairo)
LDFLAGS         += $(shell pkg-config --libs uuid)
LDLIBS           =
LDLIBS          += $(FOOTLIB)

PROGS            =
PROGS           += showtags
PROGS           += myproj
PROGS           += random

.PHONY: all
all: $(PROGS)

showtags: showtags.o $(FOOTLIB)
	$(CC) $(LDFLAGS) showtags.o $(LDLIBS) -o $@

myproj: myproj.o futil.o $(FOOTLIB)
	$(CC) $(LDFLAGS) myproj.o futil.o $(LDLIBS) -o $@

random: random.o futil.o $(FOOTLIB)
	$(CC) $(LDFLAGS) random.o futil.o $(LDLIBS) -o $@

myproj.o: futil.h
random.o: futil.h
futil.o: futil.h

%.o: %.c $(FOOTLIB)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: $(FOOTLIB)
$(FOOTLIB):
	$(MAKE) -C $(FOOTDIR) OUTDIR=$(HERE)/build INSTALLDIR=$(HERE)/dist install

.PHONY: myproj-valgrind
myproj-valgrind: myproj
	valgrind --leak-check=full ./$<

.PHONY: random-valgrind
random-valgrind: random
	valgrind --leak-check=full ./$<

.PHONY: clean
clean:
	$(MAKE) -C $(FOOTDIR) OUTDIR=$(HERE)/build INSTALLDIR=$(HERE)/dist clean
	$(RM) $(FOOTLIB)
	$(RM) -r dist
	$(RM) *.o
	$(RM) $(PROGS)
	$(RM) -r myproj-out
	$(RM) -r random-out

