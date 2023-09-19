<!-- index-meta.md: gendoc includes this into the generated documentation index.md file-->

This page contains an introduction, followed by an index of the files and all names making up the cj50 library.

# Introduction

This library exists to support teaching programming using the C programming language, and was originally inspired by the course [CS50: Introduction to Computer Science](https://pll.harvard.edu/course/cs50-introduction-computer-science). You can also find its videos [on YouTube](https://www.youtube.com/c/cs50/videos). Check those out if you like watching someone else than me for a change. Be mindful that the cs50.h library that they are using is different from cj50.h here, although there are some similarities. cj50 is more concerned with allowing you to make larger programs including graphical programs like simple video games.

If you want to read a book about the C programming language, here are some:

* [The C Programming Language](https://en.wikipedia.org/wiki/The_C_Programming_Language) by Brian Kernighan and Dennis Ritchie is from the original inventors of C. You can get an old version of the book for free [on archive.org](https://archive.org/details/TheCProgrammingLanguageFirstEdition/mode/2up), but be mindful that this version is not describing the version of C we use today. But the newer editions (which are not free) are said to be very good to learn C from.

* [Modern C](https://inria.hal.science/hal-02383654/file/ModernC.pdf) is a free book that seems good but is more detailed than we need for this course.

Websites:

* [C reference (cppreference.com)](https://en.cppreference.com/w/c)

Note: this library is going beyond and departing from standard C programming idioms. The primary aim is not to teach how to use the C programming language in a typical way, but to teach programming in general, and in particular for making simple video games. The style of programming is intentionally kept very close to the principles and libraries of the [Rust](https://en.wikipedia.org/wiki/Rust_(programming_language)) programming language. The aim is to make it easier for a cj50 library user to move on to learning Rust should they so desire.

In particular, cj50 is a header-only library and makes liberal use of struct copying (modern compilers optimize this well), metaprogramming (generating code from templates), and is reporting optional values via `Option` and errors via `Result`, whereas standard C programming practise is to separates header files from implementation files, to use boxing (`void*` pointers and casting) or intrusive data structures, and to use special values for representing optional cases (e.g. `-1`, `NULL`), and error codes combined with "return arguments" (pointer to storage where to put the result) to report errors. The emphasis in cj50 is more on abstraction and less on lowlevel details, although it is always possible to follow the lowlevel details by reading the cj50 source code (the `-E` compiler flag may be useful to let you read the generated code<!-- XX -E without system header includes -->).

Here's an overview of the programming principles used when programming with cj50. This is very dense and only meant as a reference, or overview for someone who already knows programming. (The teaching goes into these topics much more slowly.)

## Cleaning up

Programming with C requires you in general to explicitly encode in your program when it is done with a particular piece of data. Using cj50, you do this by calling the function `drop`. (You may have heard that the C programming language has the function `free`. cj50 uses `free` under the hood, but offers `drop` instead as `drop` does more than `free` (it also frees up other resources than memory, and frees up any contained data structures, too), and, unlike `free`, `drop` is generally available for all kinds of data (but not for references!) even if it does nothing, so it's easier to use.)

Some kinds of values, for example number types like `int` or `float`, as well as reference types (see next section), do not require cleaning up. They are so-called "Copy" types, they are small and can be copied everywhere and then will just be let go without needing to care about anything. Those are exempt from needing to be passed to `drop` (and in fact `drop` may not accept some of those, although that would be out of laziness on behalf of the cj50 author rather than out of principle).

## References

When programming, one uses variable names to refer to changing values that the program is handling. Unlike some other programming languages, in C, the bits representing the value are held in the variables directly. If you assign one variable to another, the bits are copied. For example, in the following function, at point A, variable `y` holds a copy of the bits in variable x2:

```C
int f(int x) {
    int x2 = x * 3;
    int y = x2;
    // -- point A
    y++;
    // -- point B
}
```

At point B, y was modified (incremented by one), whereas x2 is still the original value. In other words, the values are not being shared.

In some cases, this is not desirable: if the values are taking up much memory, making copies slows down the program and may take up space. But worse, for values that should be mutated (changed), and where the changes should be visible from multiple variables at once, sharing is preferable. C offers references (commonly called "pointers" in C) for this purpose. References are small, and by passing references to a value, the original value can be modified through the reference. The reference to a place can be taken by putting the `&` operator left of it.

Example:

```C
String s = new_String();
push(&s, 'H');
push(&s, 'i');
println(&s);
drop(s);
```

Passing a reference `&s` instead of the String itself from `s` to
`push` is necessary here because otherwise the change couldn't be
reflected in the variable `s`. The String does not change place here,
only a reference, a way to access it indirectly, is passed to `push` and `println`. OTOH, the String itself is passed to `drop` at the end: this is because `drop` *consumes* its argument. The variable `s` is not usable any longer after having passed its content to `drop`.

In the context of cj50, whenever a value of a type whose name starts with an uppercase letter is "copied" to another place, it is actually *moved*, not copied. By convention, such a copy means that the original place is considered emptied now and can't be accessed any more. Such values can be moved any number of times, but never copied so that they could continue to be used as clones in multiple places (we are talking about copying the main value itself; references are another matter). An exception are types for which the `clone()` function is defined. They can be cloned explicitly via that function and each clone can then be moved independently.

When such a value is not needed any longer, it must be moved into `drop()`. If you forget to arrange for that, the memory sanitizer that we're using will likely report a memory leak when you run the program. The absence of such reports is no guarantee that your program is doing things correctly, though (the sanitizer may miss some issues, and it can't report on issues that are not touched upon in a particular program run).

> Rule 1: Values of types that start with an uppercase initial must only ever exist in a single place at the same time, although they can be moved around. In the end, they must be moved into the `drop()` function.

Exempt from this rule are values of types with a lowercase initial, like `int`, `cstr`, `ucodepoint`, `slice` and more. They can be copied directly without needing to do this explicitly via the `clone()` function, and don't need to be passed to `drop`. `mutslice` also does not need to be passed to `drop`, but it should not be copied because only one part of the code should ever have the ability to mutate the shared data at a time. Some types with uppercase initials, like `Vec2` or `Vec3`, can be copied, too, if their element type can.

*NOTE*: currently `clone` is not defined yet, it will be added in the future.

*NOTE*: C typically uses the term "pointer" instead of "reference". cj50 uses the latter term, because in its documented interfaces it generally only uses pointers to single items. Whereas C allows pointer arithmetic (adding offsets to a pointer to access other items in the vicinity of the originally pointed-at item), cj50 does not make use of that for documented interfaces. This is to make it easier to reason about a program, and to allow for universal use of the `drop` function. Pointers are used internally and hidden that way: for example, `String` has a pointer to an array of characters, but it wraps it and hides that fact (somewhat; in C there is no easy way to enforce hiding). Languages like C++ and Rust use the term "reference" for a feature that only allows the single-item use of pointers, and so shall we.

A reference is a small value, that is cheap to copy, and does not need to be passed to `clone` for copying or to `drop` for cleaning up, just like the lowercase-initial types like `int` mentioned above. (Rust calls such types "Copy" types.) On a low level, it is a memory address, an integer value that says how many bytes from the beginning of the address space the value is positioned.

References are only valid as long as the variable or data structure that they are referencing is still in the original place. This means that as long as you're using a reference somewhere, you may not move the value to a different place (including `drop()`). 

> Rule 2: as long as there are references in use to a variable in the program, that variable must not cease to exist, and its contents must not be moved to another place. If the reference refers to a place inside a data structure, that data structure must not cease to exist nor must it be moved.

It is your duty to ensure that the program follows this rule, just as with rule 1. Again, the memory sanitizer may report violations, but again, this is not guaranteed, both because the sanitizer may not detect a violation, but also because your program may not always misbehave; your current use may not trigger the case where it misbehaves, so you may not discover that there is a bug.

## Types

C built-in types ("primitive types") have all-lowercase names, e.g. `char`, `int`, `float`, `uint8_t`, `bool`, `unsigned int`.

cj50 adds some type aliases which are also all-lowercase: `uint`, `u64`, `cstr`. The first two are primitive types given a new name to avoid the space in the type name, so that they can be used as type parameters (see following subsection), or for using shorter names: `uint` is the same as `unsigned int`, `u64` is the same as `uint64_t`. The latter is really just for shorter naming (and to use the same name as in Rust). `cstr` is an alias for `const char*`, and has the additional *meaning* to represent a "C string", which has the convention of adding a `'\0'` character after the text in the string. We need an alias again because we can't use the `*` as part of type parameters, but it's also clearer to tell what kind of char pointer you're dealing with. It is mostly (in the future, todo) the only array type cj50's documented functions use--that's because it's the only array type that comes with the (run-time) information about its length (the aforementioned `'\0'` marker). It deserves a lowercase type name (todo: why exactly?).

For all non-primitive types (which means, those implemented using `struct` (or `union`)), cj50 uses a name that starts with a capital and use camel casing over underscores.

### Types with parameters

Some types need a type *parameter*. For example, there is a type representing an optional value, `Option`. But, an optional value of *what* type? This other, contained, type, is a parameter that `Option` needs to become a concrete type, for example `Option(int)`.

Now, the C language doesn't actually have any built-in notion of type parameters. It just knows about concrete types. We could have an `Option_holding_an_int` type, or we could call it `Option_int`, but that could only hold an `int` and no other type. So if we want to also have a type that represents an optional `cstr`, we have to define a type with another name like `Option_holding_a_cstr` or `Option_cstr` or similar. You may notice why we need the type aliases as described in the previous section: we couldn't be able to define a type like `Option_const char*`, because C only allows to define new type names that consist of word characters (A-Z, a-z, 0-9, _) (todo: exact rules?). Even `Option_const_char*` would not work since that would rather be a pointer to `Option_const_char`, which sounds like it optionally holds a single `char`, not a `const char*`. If we want to use the same type name for `Option` as outside, we need to define an alias, which is what was done to get `cstr`.

So it's possible to have types that are representative of some more general notion of a type that is parameterized with another type, as long as we care about making the naming work, and are OK with defining a new type for every combination. The way to make this work are macros, which leads us to the next section. Thanks to macros, a new type can be *defined* by *generating* code that is adapted to the parameter, and it can be *used* by generating the type name using the parameters. We're not going into the definition part here, but the usage part is that `Option(int)` is valid in cj50, because `Option` is a macro that takes one argument like `Option(T)` and simply returns the type name `Option_$T` when using shell style variable interpolation; for `Option(int)` it returns the type `Option_int`. And that is valid C code. If you use the `-E` compiler option, you can see the generated code after macro expansion, at which level you will see `Option_int` instead of `Option(int)`. (You may ask, why use a macro named `Option` at all, couldn't you just write `Option_int` in your program? You could. But `Option(int)` represents the meaning in a more explicit way, and even if it doesn't matter for the computer, such understanding of the meaning is important for programmers reading and changing the code.)

## Macros

Macros without parentheses (like `AUTO`) are simply replaced with their definition wherever they are used.

Macros with arguments (parentheses) are like functions, but they take *code* as their arguments and return code as their result, while the program is being compiled (more accurately: as the first step of the compilation). In other words, they transform the code you pass to them. The code that actually executes can be very different. So, macros can lead to program behaviour in ways you don't immediately see by looking at the macro call, and are thus powerful, but can also be confusing. For this reason, there are fewer macro definitions than function definitions.

To visually distinguish them from functions and types (so that you know something special is going on), C macros are spelt in all-capitals, like `AUTO`. There are a few exceptions to this:

* "generic functions" are actually also macros, but their naming is the shared prefix of the functions they are selecting. That's OK since the only code transformation they do is select the right function, nothing more magic than that happens.)

* Types with parameters are also actually implemented as macros, but they are named according to the type naming rules described in the previous section.

* macros that seem to be intuitive extensions to the C syntax and have
  names that contain the C keyword they are "extending" and are
  relatively long and hence won't ever clash with function
  names. Examples: `if_let_Some`/`else_None`, `let_Some_else`,
  `while_let_Some`, `if_let_Ok`/`else_Err`/`end_let_Ok`.

