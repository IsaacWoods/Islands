CFLAGS=-g -O1 -Wall -Wextra -pedantic -std=c++14 -Isrc -Wno-vla
LFLAGS=-g -O1 -Wall -Wextra -pedantic -std=c++14 -Isrc -lSDL2 -ldl -lassimp -lncurses

OBJS=\
	src/main.o \
#	src/gl3w.o \
#	src/platform.o \
#	src/maths.o \
#	src/asset.o \
#	src/rendering.o \
#	src/entity.o \
#	src/main.o \
#	src/world.o \

.PHONY: clean

islands: $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LFLAGS)

%.o: %.cpp
	$(CXX) -o $@ -c $< $(CFLAGS)

src/gl3w.cpp: gl3w_gen.py
	python gl3w_gen.py

clean:
	find . -name '*.o' | xargs rm
	rm -f src/gl3w.hpp src/gl3w.cpp
	rm -f islands