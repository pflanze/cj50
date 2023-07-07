/*
  Copyright (C) 2021-2023 Christian Jaeger, <ch@christianjaeger.ch>
  Published under the terms of the MIT License, see the LICENSE file.
*/

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static
void die_outofmemory() {
    fprintf(stderr, "Out of memory, aborting\n");
    fflush(stderr);
    abort();
}

static UNUSED
void *xmalloc(size_t size) {
    void *p = malloc(size);
    if (!p) die_outofmemory();
    return p;
}

static UNUSED
void *xmallocarray(size_t nmemb, size_t size) {
    size_t bytes = nmemb * size;
    if ((bytes < nmemb) || (bytes < size))
        die_outofmemory();
    void *p = xmalloc(bytes);
    if (!p) die_outofmemory();
    return p;
}

static UNUSED
void *xreallocarray(void *ptr, size_t nmemb, size_t size) {
    // void *p = reallocarray(ptr, nmemb, size);
    size_t bytes = nmemb * size;
    if ((bytes < nmemb) || (bytes < size))
        die_outofmemory();
    void *p = realloc(ptr, bytes);
    if (!p) die_outofmemory();
    return p;
}


static UNUSED
char *xstrdup(const char *str) {
    char *res= strdup(str);
    if (!res) die_outofmemory();
    return res;
}

static UNUSED
void *xmemcpy(const void *src, size_t n) {
    void *p = xmalloc(n);
    memcpy(p, src, n);
    return p;
}


