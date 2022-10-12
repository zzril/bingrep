#ifndef BINGREP_H
#define BINGREP_H
// --------

#define _GNU_SOURCE

// --------

#include <stddef.h>

// --------

struct BINGREP_File;

// --------

typedef unsigned short BINGREP_flags_t;

typedef struct BINGREP_File BINGREP_File;

typedef void (*BINGREP_MatchHandler)(ptrdiff_t);

// --------

/**
 * Opens the file with name given in `pathname` either by mapping it to memory 
 * or by reading it into a buffer, depending on its size.
 * The default behaviour for when the file is mapped into memory can be 
 * overridden with the `flags` argument (not implemented yet).
 * Returns a pointer to the allocated BINGREP_File structure, or NULL if the
 * allocation failed.
 */
BINGREP_File* BINGREP_open_file(const char* pathname, BINGREP_flags_t flags);

/**
 * Releases the resources allocated for `file`.
 * Does nothing if `file` is NULL.
 */
void BINGREP_close_file(BINGREP_File* file);

/**
 * Searches for the byte sequence given in `signature` within `file`.
 * For each sequence found, `callback` is executed with the `offset` argument 
 * being the offset within the file where `signature` started.
 * If `callback` is NULL, no operation will be performed.
 * If `finish_early` is not 0 and `callback` is NULL, processing will stop
 * after the first match.
 * Returns the number of matches found or -1, if an error occured.
 */
long BINGREP_find_signature	(	BINGREP_File* file,
					const char* signature,
					size_t signature_length,
					BINGREP_MatchHandler callback,
					int finish_early	);

// --------
#endif


