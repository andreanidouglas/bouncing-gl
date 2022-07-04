CFLAGS:=-Wall -Wextra -g -pedantic -std=c11 `pkg-config --cflags glew glfw3` 
LIBS:=`pkg-config --libs glew glfw3` -lm


bouncing: bouncing.c
	${CC} ${CFLAGS} $< -o $@ ${LIBS}

.PHONY:
clean:
	rm -f bouncing bouncing.o la.o