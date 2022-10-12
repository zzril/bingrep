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

static void close_fd(BINGREP_File* file);
static void unmap_or_free(BINGREP_File* file);

// --------

static void close_fd(BINGREP_File* file) {
	if(file->fd > 2) {
		close(file->fd);
	}
	file->fd = -1;
	return;
}

static void unmap_or_free(BINGREP_File* file) {
	if(file->is_memory_mapped) {
		if(file->start_address != MAP_FAILED) {
			munmap(file->start_address, file->filesize);
		}
	}
	else {
		free(file->start_address);
	}
	file->start_address = NULL;
	file->is_memory_mapped = 0;
	return;
}

// --------

int BINGREP_open_file_at(BINGREP_File* file, const char* pathname) {

	// Init with default values, so the close function already works properly:
	file->fd = -1;
	file->filesize = 0;
	file->start_address = NULL;
	file->is_memory_mapped = 0;

	file->fd = open(pathname, O_RDONLY);
	if(file->fd < 0) { perror("open"); BINGREP_close_file_at(file); return -1; }

	file->filesize = lseek(file->fd, 0, SEEK_END);
	if(file->filesize < 0) { perror("lseek"); BINGREP_close_file_at(file); return -1; }

	// (For now, we always memory map the file. May instead switch to malloc for small files in the future.)
	file->is_memory_mapped = 1;

	if(file->is_memory_mapped) {
		file->start_address = mmap(NULL, file->filesize, PROT_READ, MAP_PRIVATE, file->fd, 0);
		if(file->start_address == MAP_FAILED) { perror("mmap"); BINGREP_close_file_at(file); return -1; }
	}
	else {
		// Not implemented yet
	}

	return 0;
}

void BINGREP_close_file_at(BINGREP_File* file) {
	close_fd(file);
	unmap_or_free(file);
	return;
}

long BINGREP_find_signature(BINGREP_File* file, char* signature, size_t signature_length, BINGREP_MatchHandler callback) {

	if(file == NULL) {
		return -1;
	}

	char* file_start = file->start_address;
	size_t file_size = file->filesize;
	char* file_end = file_start + file_size;

	size_t num_matches = 0;
	char* search_start = file_start;

	while(search_start + signature_length <= file_end) {
		char* match_addr = (char*) memmem((void*) search_start, file_size, signature, signature_length);
		if(match_addr != NULL) {
			num_matches++;
			if(!(num_matches > 0)) {
				fputs("WARNING: Number of matches incorrect due to integer overflow.\n", stderr);
			}
			callback(match_addr - file_start);
		}
		else { // no more matches
			break;
		}
		// Continue search from 1 byte behind match (so we find overlapping matches as well):
		search_start = match_addr + 1;
	}
	return num_matches;
}


