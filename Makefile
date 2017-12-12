CC=gcc
INSTALL=install
PREFIX ?= /usr/local
EXEC_PREFIX=$(PREFIX)
BINDIR=$(EXEC_PREFIX)/bin
LENSVER= 2.63
TCLVER= 8.6
#INCL=   -I$(TCLDIR)/tcl$(TCLSUBVER)/generic -I$(TCLDIR)/tk$(TCLSUBVER)/generic \
#        -I$(TCLDIR)/tcl$(TCLSUBVER)/unix    -I$(TCLDIR)/tk$(TCLSUBVER)/unix \
#				-I$(SRCDIR)
LDLIBS= -llens$(LENSVER) -ltk$(TCLVER) -ltcl$(TCLVER) -lm -lX11

default: layeract netacc

layeract: src/layeract.c
	$(CC) -Wall -std=c99 $(CFLAGS) -o bin/layeract src/layeract.c $(LDFLAGS) $(LDLIBS)

netacc: src/netacc.c
	$(CC) -Wall -std=c99 $(CFLAGS) -o bin/netacc src/netacc.c $(LDFLAGS) $(LDLIBS)

install:
	$(INSTALL) bin/layeract $(DESTDIR)$(BINDIR)/layeract
	$(INSTALL) bin/netacc $(DESTDIR)$(BINDIR)/netacc
