IGNORED_WARNINGS = -Wno-unused-result -Wno-trigraphs -Wno-vla -Wno-nested-anon-types -Wno-missing-braces -Wno-vla-extension
CFLAGS=-g -O0 -Wall -Wextra -pedantic -std=c++1z -Isrc $(IGNORED_WARNINGS)
LFLAGS=-g -O0 -Wall -Wextra -pedantic -std=c++1z -Isrc -lSDL2 -ldl -lassimp -lncurses

OBJS=\
	src/gl3w.o \
	src/platform.o \
	src/maths.o \
	src/asset.o \
	src/rendering.o \
	src/entity.o \
	src/world.o \
	src/main.o \
	src/imgui/imgui.o \
	src/imgui/imgui_demo.o \
	src/imgui/imgui_draw.o \
	src/imgui/imgui_impl_sdl_gl3.o \

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
