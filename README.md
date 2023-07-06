# CJ50 utility library

This is a helper library for teaching programming using C.

The emphasis here is on teaching programming, not teaching C: C is
being used because it is a relatively simple language that also allows
talking about memory and bits, has an important place in history, and
is practical for making simple video games (which young starting
programmer doesn't fancy doing that?) that can also run in the browser
via WASM.

The aim of the library is hence to provide for an as nice
introductionary programming experience as possible. It offers easy
ways to get input from and write output to the terminal, and (not yet
finished) 2D graphics output. It follows a Rust-inspired programming
style: `Option`, `unwrap`, `drop`, using type names with an upper-case
initial for complex types. It provides generic functions (via C11
`_Generic`) to avoid having to type out the fully named function
specializations everywhere. The makefile compiles with sanitizers to
catch undefined behaviour.

The name is derived from [cs50.h](https://github.com/cs50/libcs50)
which this library was originally inspired by.

## Usage

This needs make, Clang and SDL2, as well as pandoc for generating the html files. In the current form it will probably only work on Linux or other Unix-like systems. On Debian-derived systems, install the dependencies with:

    sudo apt-get install make clang libsdl2-dev pandoc

For ease of use and updating, the [Makefile](Makefile) can be used to build both the included [examples](examples/) and third party files but assumes that this library is checked out at `~/cdevelopment/cj50`. Meaning, run the following to use this library:

    cd
    mkdir -p cdevelopment
    cd cdevelopment
    git clone https://github.com/pflanze/cj50
    mkdir ownproject
    cd ownproject
    git init
    ln -s ../cj50/Makefile
    cp ../cj50/examples/basics.c .
    make basics
    ./basics
    # then write your own files and compile and use them:
    $EDITOR myown.c
    make myown
    ./myown

If you want to use `make auto` (which runs `bin/auto-make`), you need to install [chj-scripts](https://github.com/pflanze/chj-bin.git).

## Documentation

The library files contain embedded documentation, which can be
converted to Markdown and then html format via:

    cd ~/cdevelopment/cj50
    make doc

You can then open the file
[doc-output/index.html](doc-output/index.md) in your browser.

The above simply uses the generated markdown files contained in the
repository. If you edit the documentation in the library files, you
need to run `make gendoc` instead to update the markdown files, but
that also requires the
[FunctionalPerl](https://metacpan.org/pod/FunctionalPerl) Perl package
to be installed.
