/*
MIT License

Copyright (c) 2022 zzril

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// --------

#define _GNU_SOURCE

// --------

#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "bingrep.h"

// --------

struct BINGREP_File {
	int fd;
	char* start_address;
	size_t filesize;
	unsigned short is_memory_mapped;
};

// --------

static void close_fd(BINGREP_File* file);
static void free_content(BINGREP_File* file);
static void unmap_memory(BINGREP_File* file);
static void free_buffer(BINGREP_File* file);

static void print_nothing(ptrdiff_t offset);

// --------

static void close_fd(BINGREP_File* file) {
	if(file->fd > 2) {
		close(file->fd);
	}
	file->fd = -1;
	return;
}

static void free_content(BINGREP_File* file) {
	if(file->is_memory_mapped) {
		unmap_memory(file);
	}
	else {
		free_buffer(file);
	}
	file->start_address = NULL;
	file->is_memory_mapped = 0;
	return;
}

static void unmap_memory(BINGREP_File* file) {
	if(file->start_address != MAP_FAILED) {
		munmap(file->start_address, file->filesize);
	}
	return;
}

static void free_buffer(BINGREP_File* file) {
	free(file->start_address);
	return;
}

// Called when `callback` is specified as NULL:
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
static void print_nothing(ptrdiff_t offset) {
	return;
}
#pragma clang diagnostic pop

// --------

BINGREP_File* BINGREP_open_file(const char* pathname, BINGREP_flags_t flags) {

	BINGREP_File* file = malloc(sizeof(BINGREP_File));
	if(file == NULL) { return NULL; }

	// Init with default values, so the close function already works properly:
	file->fd = -1;
	file->filesize = 0;
	file->start_address = NULL;
	file->is_memory_mapped = 0;

	file->fd = open(pathname, O_RDONLY);
	if(file->fd < 0) { perror("open"); BINGREP_close_file(file); return NULL; }

	file->filesize = lseek(file->fd, 0, SEEK_END);
	if(file->filesize < 0) { perror("lseek"); BINGREP_close_file(file); return NULL; }

	// (For now, we always memory map the file. May instead switch to malloc for small files in the future.)
	file->is_memory_mapped = 1;

	if(file->is_memory_mapped) {
		file->start_address = mmap(NULL, file->filesize, PROT_READ, MAP_PRIVATE, file->fd, 0);
		if(file->start_address == MAP_FAILED) { perror("mmap"); BINGREP_close_file(file); return NULL; }
	}
	else {
		// Not implemented yet
	}

	return file;
}

void BINGREP_close_file(BINGREP_File* file) {
	if(file == NULL) { return; }
	// Release resources:
	close_fd(file);
	free_content(file);
	// Free file itself:
	free(file);
	return;
}

long BINGREP_find_signature	(	BINGREP_File* file,
					const char* signature,
					size_t signature_length,
					BINGREP_MatchHandler callback,
					int finish_early	) {

	if(file == NULL) {
		return -1;
	}

	char* file_start = file->start_address;
	size_t file_size = file->filesize;
	char* file_end = file_start + file_size;

	size_t num_matches = 0;
	char* search_start = file_start;

	BINGREP_MatchHandler match_handler = callback == NULL ? &print_nothing : callback;

	int finish = 0;

	while(search_start + signature_length <= file_end && !finish) {
		char* match_addr = (char*) memmem((void*) search_start, file_size, signature, signature_length);
		if(match_addr != NULL) {
			num_matches++;
			if(!(num_matches > 0)) {
				fputs("WARNING: Number of matches incorrect due to integer overflow.\n", stderr);
			}
			match_handler(match_addr - file_start);
			// If `finish_early` is specified, break now:
			finish = finish_early;
		}
		else { // no more matches
			break;
		}
		// Continue search from 1 byte behind match (so we find overlapping matches as well):
		search_start = match_addr + 1;
	}

	return num_matches;
}


