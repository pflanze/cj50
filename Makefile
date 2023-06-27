ASAN=-fsanitize=address

# -Werror -pedantic -std=c11
CFLAGS=-Wall -Wextra -g3 $(ASAN) -fsanitize=undefined,float-divide-by-zero -fno-sanitize-recover -I$(HOME)/cdevelopment/cj50

CC=gcc

%: %.c $(HOME)/cdevelopment/cj50/cj50.h
	$(CC) $(CFLAGS) $< -o $@

