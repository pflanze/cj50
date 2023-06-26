
# -Werror -pedantic -std=c11
CFLAGS=-Wall -Wextra -g3 -fsanitize=undefined,float-divide-by-zero,implicit-conversion,local-bounds -fsanitize=implicit-integer-truncation -fno-sanitize-recover -fsanitize=integer -fsanitize=nullability -I$(HOME)/cdevelopment/cj50

