CFLAGS:=-Wall -Wextra -g -pedantic -std=c11 `pkg-config --cflags glew glfw3` 
LIBS:=`pkg-config --libs glew glfw3` -lm


bouncing: main.c bouncing.o la.o
	${CC} ${CFLAGS} main.c bouncing.o la.o -o bouncing ${LIBS}

bouncing.o: bouncing.h bouncing.c
	${CC} ${CFLAGS} -c bouncing.c -o bouncing.o

la.o: la.h la.c
	${CC} ${CFLAGS} -c la.c -o la.o

.PHONY:
clean:
	rm -f bouncing bouncing.o la.o