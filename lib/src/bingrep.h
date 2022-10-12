#ifndef BINGREP_H
#define BINGREP_H
// --------

#define _GNU_SOURCE

// --------

#include <stddef.h>

// --------

struct BINGREP_File;

struct BINGREP_File {
	int fd;
	char* start_address;
	size_t filesize;
	unsigned short is_memory_mapped;
};

typedef struct BINGREP_File BINGREP_File;
typedef void (*BINGREP_MatchHandler)(ptrdiff_t offset);

// --------

/**
 * (`file` must be already allocated.)
 */
int BINGREP_open_file_at(BINGREP_File* file, const char* pathname);
void BINGREP_close_file_at(BINGREP_File* file);

long BINGREP_find_signature(BINGREP_File* file, char* signature, size_t signature_length, BINGREP_MatchHandler callback);

// --------
#endif


