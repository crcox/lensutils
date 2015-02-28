LENS_SRC= ${LENSDIR}/Src
LENS_BIN= ${LENSDIR}/Bin/x86_64
LIBS= -Llib -L${LENS_BIN} -llens2.63 -ltk8.3 -ltcl8.3 -lm -lX11

layeract: src/layeract.c Makefile
	gcc -Wall -std=c99 -o bin/layeract -I ${LENS_SRC} src/layeract.c ${LIBS}

netacc: src/netacc.c Makefile
	gcc -Wall -std=c99 -o bin/netacc -I ${LENS_SRC} src/netacc.c ${LIBS}
