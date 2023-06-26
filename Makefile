ASAN=-fsanitize=address

# -Werror -pedantic -std=c11
CFLAGS=-Wall -Wextra -g3 $(ASAN) -fsanitize=undefined,float-divide-by-zero -fno-sanitize-recover -I$(HOME)/cdevelopment/cj50

