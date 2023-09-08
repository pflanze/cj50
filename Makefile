ASAN ?= -fsanitize=address

CJ50BASEDIR ?= $(HOME)/cdevelopment/cj50

# -Werror -pedantic -std=c11
CFLAGS=-fdiagnostics-color=always -Wall -Wextra -g3 -fsanitize=undefined,float-divide-by-zero -fno-sanitize-recover -I$(CJ50BASEDIR)

SDLFLAGS:=`sdl2-config --libs`

COMPILER ?= clang
CC=you_have_a_non_existing_dependency

%: %.c $(CJ50BASEDIR)/cj50.h $(CJ50BASEDIR)/*.h
	$(COMPILER) $(CFLAGS) $(ASAN) $< $(SDLFLAGS) -o $@

%_opt: %.c $(CJ50BASEDIR)/cj50.h $(CJ50BASEDIR)/*.h
	$(COMPILER) -O2 $(CFLAGS) $(ASAN) $< $(SDLFLAGS) -o $@

%_max: %.c $(CJ50BASEDIR)/cj50.h $(CJ50BASEDIR)/*.h
	$(COMPILER) -Ofast -mtune=native $(CFLAGS) $< $(SDLFLAGS) -o $@

%_profile: %.c $(CJ50BASEDIR)/cj50.h $(CJ50BASEDIR)/*.h
	$(COMPILER) -Ofast -fno-inline-functions -fno-inline-functions-called-once -fno-optimize-sibling-calls -mtune=native -Rpass=loop-vectorize -Rpass-missed=loop-vectorize -Rpass-analysis=loop-vectorize -gline-tables-only $(CFLAGS) $< $(SDLFLAGS) -o $@

%_E: %.c $(CJ50BASEDIR)/cj50.h $(CJ50BASEDIR)/*.h
	$(COMPILER) -E $(CFLAGS) $< | less

%_EF: %.c $(CJ50BASEDIR)/cj50.h $(CJ50BASEDIR)/*.h
	$(COMPILER) -E $(CFLAGS) $< | clang-format | less

default:
	@echo "Please specify the make target, or 'make auto' to watch."
	@false

auto:
	$(CJ50BASEDIR)/bin/auto-make

doc:
	bin/doc2html

_gendoc:
	rm -rf doc-output
	mkdir doc-output
	bin/gendoc
	git add doc-output

gendoc: _gendoc doc

test:
	bin/run bin/run-tests

