CFLAGS = -I ./include
LFLAGS = -lrt -lX11 -lGLU -lGL -pthread -lm #-lXrandr

all: walk2

walk2: walk2.cpp log.cpp
	g++ $(CFLAGS) walk2.cpp log.cpp libggfonts.a -Wall -Wextra $(LFLAGS) -owalk2 \
	-D USE_OPEN_AL_SOUND \
	/usr/lib/x86_64-linux-gnu/libopenal.so \
	/usr/lib/x86_64-linux-gnu/libalut.so 	


clean:
	rm -f walk2
	rm -f *.o

