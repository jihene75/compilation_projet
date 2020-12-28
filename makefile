CC= gcc 
OPTION= -Wall 
OUT= -o compilation
install: compilation.c
	${CC} ${OPTION} compilation.c ${OUT} -lm
run: compilation
	./compilation
clean: compilation 
	rm -f compilation
