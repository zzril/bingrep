#ifndef BINGREP_H
#define BINGREP_H
// --------

#define _GNU_SOURCE

// --------

#include <stddef.h>

// --------

typedef void (*BINGREP_MatchHandler)(ptrdiff_t offset);

// --------

unsigned long BINGREP_find_signature(char* file, off_t file_size, char* signature, size_t signature_length, BINGREP_MatchHandler callback);

// --------
#endif


