#ifndef BINGREP_H
#define BINGREP_H
// --------

#define _GNU_SOURCE

// --------

#include <stddef.h>

// --------

struct BINGREP_File;

typedef struct BINGREP_File BINGREP_File;
typedef void (*BINGREP_MatchHandler)(ptrdiff_t offset);

// --------

/**
 * (`file` must be already allocated.)
 */
BINGREP_File* BINGREP_open_file(const char* pathname);
void BINGREP_close_file(BINGREP_File* file);

long BINGREP_find_signature(BINGREP_File* file, char* signature, size_t signature_length, BINGREP_MatchHandler callback);

// --------
#endif


