ASAN ?= -fsanitize=address

# -Werror -pedantic -std=c11
CFLAGS=-fdiagnostics-color=always -Wall -Wextra -g3 -fsanitize=undefined,float-divide-by-zero -fno-sanitize-recover -I$(HOME)/cdevelopment/cj50

SDLFLAGS:=`sdl2-config --libs`

COMPILER ?= clang
CC=you_have_a_non_existing_dependency

%: %.c $(HOME)/cdevelopment/cj50/cj50.h $(HOME)/cdevelopment/cj50/*.h
	$(COMPILER) $(CFLAGS) $(ASAN) $< $(SDLFLAGS) -o $@

%_opt: %.c $(HOME)/cdevelopment/cj50/cj50.h $(HOME)/cdevelopment/cj50/*.h
	$(COMPILER) -O2 $(CFLAGS) $(ASAN) $< $(SDLFLAGS) -o $@

%_max: %.c $(HOME)/cdevelopment/cj50/cj50.h $(HOME)/cdevelopment/cj50/*.h
	$(COMPILER) -Ofast -mtune=native $(CFLAGS) $< $(SDLFLAGS) -o $@

%_profile: %.c $(HOME)/cdevelopment/cj50/cj50.h $(HOME)/cdevelopment/cj50/*.h
	$(COMPILER) -Ofast -fno-inline-functions -fno-inline-functions-called-once -fno-optimize-sibling-calls -mtune=native -Rpass=loop-vectorize -Rpass-missed=loop-vectorize -Rpass-analysis=loop-vectorize -gline-tables-only $(CFLAGS) $< $(SDLFLAGS) -o $@

%_E: %.c $(HOME)/cdevelopment/cj50/cj50.h $(HOME)/cdevelopment/cj50/*.h
	$(COMPILER) -E $(CFLAGS) $<

%_EF: %.c $(HOME)/cdevelopment/cj50/cj50.h $(HOME)/cdevelopment/cj50/*.h
	$(COMPILER) -E $(CFLAGS) $< | clang-format

default:
	@echo "Please specify the make target, or 'make auto' to watch."
	@false

auto:
	$(HOME)/cdevelopment/cj50/bin/auto-make

doc:
	bin/doc2html

_gendoc:
	rm -rf doc-output
	mkdir doc-output
	bin/gendoc
	git add doc-output

gendoc: _gendoc doc

test:
	bin/run-tests

