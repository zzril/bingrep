#define _GNU_SOURCE

// --------

#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "bingrep.h"

// --------

#define MAX_SIGNATURE_LEN 0x40

// --------

// Private declarations:

struct Resources;

struct Resources {
	int fd;
	void* mempage;
	char* file;
	off_t file_size;
};

typedef struct Resources Resources;

static __attribute__ ((destructor)) void free_resources();

// --------

// Global variables:

static Resources g_resources = {
	-1,		// fd
	MAP_FAILED,	// mempage
	NULL,		// file
	0,		// file_size
};

// --------

// Private implementations:

static __attribute__ ((destructor)) void free_resources() {
	if(g_resources.fd > 2) {
		close(g_resources.fd);
		g_resources.fd = -1;
	}
	if(g_resources.mempage != MAP_FAILED) {
		munmap(g_resources.mempage, g_resources.file_size);
		g_resources.mempage = MAP_FAILED;
	}
	if(g_resources.file != NULL) {
		free(g_resources.file);
		g_resources.file = NULL;
	}
	g_resources.file_size = 0;
	return;
}

// --------

// Public implementations:

unsigned long BINGREP_find_signature(char* file, off_t file_size, char* signature, size_t signature_length, BINGREP_MatchHandler callback) {
	size_t num_matches = 0;
	char* search_start = file;
	while(search_start + signature_length <= file + file_size) {
		char* match_addr = (char*) memmem((void*) search_start, file_size, signature, signature_length);
		if(match_addr != NULL) {
			num_matches++;
			if(!(num_matches > 0)) {
				fputs("WARNING: Number of matches incorrect due to integer overflow.\n", stderr);
			}
			callback(match_addr - file);
		}
		else { // no more matches
			break;
		}
		// Continue search from 1 byte behind match (so we find overlapping matches as well):
		search_start = match_addr + 1;
	}
	return num_matches;
}



