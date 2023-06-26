# CJ50 utility library

Helper library for teaching, inspired by [cs50.h](https://github.com/cs50/libcs50).

## Usage

This expects GCC to be installed and as is will probably only work on Linux or other Unix-like systems.

For ease of use and updating, the [Makefile](Makefile) can be used to build both the included [example.c](example.c) and third party files but assumes that this library is checked out at `~/cdevelopment/cj50`. Meaning, run the following to use this library:

    cd
    mkdir -p cdevelopment
    cd cdevelopment
    git clone https://github.com/pflanze/cj50
    mkdir ownproject
    cd ownproject
    git init
    ln -s ../cj50/Makefile
    cp ../cj50/example.c .
    make example
    ./example
    # then write your own files and compile and use them:
    $EDITOR myown.c
    make myown
    ./myown

