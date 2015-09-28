LENSVER= 2.63
TCLVER= 8.6
TCLSUBVER= $(TCLVER).4
LENSDIR= ${HOME}/src/lens
TCLDIR= $(LENSDIR)/TclTk
BINDIR= $(LENSDIR)/Bin
SRCDIR= $(LENSDIR)/Src
LIBDIR= $(LENSDIR)/usr/local/lib
INCL=   -I$(TCLDIR)/tcl$(TCLSUBVER)/generic -I$(TCLDIR)/tk$(TCLSUBVER)/generic \
        -I$(TCLDIR)/tcl$(TCLSUBVER)/unix    -I$(TCLDIR)/tk$(TCLSUBVER)/unix \
				-I$(SRCDIR)
LIBS=   -L$(LIBDIR) -L$(BINDIR) -llens$(LENSVER) -ltk$(TCLVER) -ltcl$(TCLVER) -lm -lX11

layeract: src/layeract.c
	gcc -Wall -std=c99 -o bin/layeract $(INCL) src/layeract.c $(LIBS)

netacc: src/netacc.c
	gcc -Wall -std=c99 -o bin/netacc $(INCL) src/netacc.c $(LIBS)
