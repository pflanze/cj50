ASAN=-fsanitize=address

# -Werror -pedantic -std=c11
CFLAGS=-Wall -Wextra -g3 $(ASAN) -fsanitize=undefined,float-divide-by-zero -fno-sanitize-recover -I$(HOME)/cdevelopment/cj50

SDLFLAGS:=`sdl2-config --libs`
#SDLFLAGS=-lSDL2


CC=gcc

%: %.c $(HOME)/cdevelopment/cj50/cj50.h $(HOME)/cdevelopment/cj50/cjmath.h
	$(CC) $(CFLAGS) $< $(SDLFLAGS) -o $@

