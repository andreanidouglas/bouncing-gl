CFLAGS:=-Wall -Wextra -g -pedantic `pkg-config --cflags glew glfw3` 
LIBS:=`pkg-config --libs glew glfw3`


bouncing: main.c bouncing.o
	${CC} ${CFLAGS} main.c bouncing.o -o bouncing ${LIBS}

bouncing.o: bouncing.h bouncing.c
	${CC} ${CFLAGS} -c bouncing.c -o bouncing.o

.PHONY:
clean:
	rm -f bouncing bouncing.o