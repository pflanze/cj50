ASAN ?= -fsanitize=address

# -Werror -pedantic -std=c11
CFLAGS=-fdiagnostics-color=always -Wall -Wextra -g3 $(ASAN) -fsanitize=undefined,float-divide-by-zero -fno-sanitize-recover -I$(HOME)/cdevelopment/cj50

SDLFLAGS:=`sdl2-config --libs`

COMPILER ?= clang
CC=you_have_a_non_existing_dependency

%: %.c $(HOME)/cdevelopment/cj50/cj50.h $(HOME)/cdevelopment/cj50/*.h
	$(COMPILER) $(CFLAGS) $< $(SDLFLAGS) -o $@

%_opt: %.c $(HOME)/cdevelopment/cj50/cj50.h $(HOME)/cdevelopment/cj50/*.h
	$(COMPILER) -O2 $(CFLAGS) $< $(SDLFLAGS) -o $@

default:
	@echo "Please specify the make target"
	@false

doc:
	bin/doc2html

_gendoc:
	rm -rf doc-output
	mkdir doc-output
	bin/gendoc
	git add doc-output

gendoc: _gendoc doc

test:
	OPT= tests/run

