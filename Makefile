SOURCES=$(wildcard src/*.cpp)
OBJS=$(SOURCES:.cpp=.o)

# compiler options : add debug information in debug mode
# optimize speed and size in release mode
ifneq (,$(findstring debug,$(MAKECMDGOALS)))
	CFLAGS=-g
else
	CFLAGS=-O2 -s
endif
LIBFLAGS=-lcurses

debug : tuto
release : tuto

tuto : $(OBJS)
	c++ $(OBJS) -o tuto -Wall $(LIBFLAGS) $(CFLAGS)

src/%.o : src/%.cpp
	c++ $< -c -o $@ -Iinclude -Wall $(CFLAGS)
 
clean :
	rm -f $(OBJS)

