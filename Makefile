ASAN ?= -fsanitize=address

# -Werror -pedantic -std=c11
CFLAGS=-fdiagnostics-color=always -Wall -Wextra -g3 $(ASAN) -fsanitize=undefined,float-divide-by-zero -fno-sanitize-recover -I$(HOME)/cdevelopment/cj50

SDLFLAGS:=`sdl2-config --libs`
#SDLFLAGS=-lSDL2

COMPILER ?= clang
CC=$(COMPILER)

%: %.c $(HOME)/cdevelopment/cj50/cj50.h $(HOME)/cdevelopment/cj50/cjmath.h
	$(CC) $(CFLAGS) $< $(SDLFLAGS) -o $@

%_opt: %.c $(HOME)/cdevelopment/cj50/cj50.h $(HOME)/cdevelopment/cj50/cjmath.h
	$(CC) -O2 $(CFLAGS) $< $(SDLFLAGS) -o $@


doc:
	bin/doc2html

_gendoc:
	bin/gendoc

gendoc: _gendoc doc

