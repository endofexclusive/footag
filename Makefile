# This file requires GNU Make

# OPTIONS
LIBNAME          = footag
OUTDIR          ?= build/
INSTALLDIR      ?= dist/
AR		 = ar

# Specifies which level of log messages to compile. Default is everything
# except debug messages. Levels which are specified at compile time can be
# dynamically enabled or disabled at run-time.
# FOOLOG           = -DFOOLOG_MASK=FOOLOG_ALL

ifneq ($(strip $(OUTDIR)),)
  ifneq ($(strip $(OUTDIR)),$(dir $(strip $(OUTDIR))))
    override OUTDIR:=$(OUTDIR)/
  endif
endif

CCOPT            =
CCOPT           += -O2

CPPFLAGS         =
CPPFLAGS        += -MMD

CFLAGS           =
CFLAGS          += -g
CFLAGS          += $(CCOPT)
CFLAGS          += -std=c99
CFLAGS          += -Wall
CFLAGS          += -Wextra
CFLAGS          += -Wmissing-prototypes
CFLAGS          += -Wstrict-prototypes
CFLAGS          += -Wmissing-declarations
CFLAGS          += -pedantic
CFLAGS          += -Iinclude
CFLAGS          += -Isrc
CFLAGS          += -Isrc/foot
CFLAGS          += $(shell pkg-config --cflags cairo)
CFLAGS          += $(shell pkg-config --cflags uuid)
CFLAGS          += $(FOOLOG)

LIBFILE = $(OUTDIR)lib$(LIBNAME).a

LIBSRCS  =
LIBSRCS += footson.c
LIBSRCS += geom.c
LIBSRCS += foolog.c
LIBSRCS += wiz.c
LIBSRCS += foot/foot.c
LIBSRCS += foot/grid.c
LIBSRCS += foot/tworow.c
LIBSRCS += fout/fout.c
LIBSRCS += fout/cairo.c
LIBSRCS += fout/horizon.c
LIBSRCS += fout/kicad5.c
LIBSRCS += fout/trace.c
LIBSRCS += ipc7251draft1/dip.c
LIBSRCS += ipc7251draft1/ipc7251draft1.c
LIBSRCS += ipc7251draft1/pga.c
LIBSRCS += ipc7351b/bga.c
LIBSRCS += ipc7351b/chip.c
LIBSRCS += ipc7351b/ipc7351b.c
LIBSRCS += ipc7351b/ipc7351b_table.c
LIBSRCS += ipc7351b/molded.c
LIBSRCS += ipc7351b/sideconcave.c
LIBSRCS += ipc7351b/soic.c
LIBSRCS += ipc7351b/twoside.c

LIBOBJS := $(LIBSRCS:.c=.o)
LIBOBJS := $(addprefix $(OUTDIR),$(LIBOBJS))

.PHONY: all
all: $(LIBFILE)

$(OUTDIR):
	mkdir -p $(OUTDIR)

$(OUTDIR)%.o: src/%.c | $(OUTDIR)
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(LIBFILE): $(LIBOBJS)
	$(AR) cru $@ $^

.PHONY: install
install: $(LIBFILE)
	mkdir -p $(INSTALLDIR)
	cp $(LIBFILE) $(INSTALLDIR)
	mkdir -p $(INSTALLDIR)/include/
	cp -r include/* $(INSTALLDIR)/include

.PHONY: clean
clean:
	$(RM) $(LIBOBJS)
	$(RM) $(LIBOBJS:%.o=%.d)
	$(RM) $(LIBFILE)
	-rmdir $(OUTDIR)/* 2> /dev/null || true
	-rmdir $(OUTDIR) 2> /dev/null || true

# Dependency files are generated with -MMD compiler option
-include $(LIBOBJS:%.o=%.d)

