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

// --------

#define MAX_SIGNATURE_LEN 0x40

// --------

static int g_filedes = -1;

// --------

static void free_and_exit(int exit_code);
static void print_usage_msg(FILE* stream, char* program_name);
static size_t parse_hexstring(char* dest, const char* hexstring);

// --------

static void free_and_exit(int exit_code) {
	if(g_filedes > 2) {
		close(g_filedes);
		g_filedes = -1;
	}
	exit(exit_code);
}

static void print_usage_msg(FILE* stream, char* program_name) {
	fprintf(stream, "Usage: %s hex_signature filename", program_name);
}

static size_t parse_hexstring(char* dest, const char* hexstring) {

	char current_char = 0;
	size_t chars_read = 0;

	// Read two characters per iteration and add the parsed byte to dest:
	while(hexstring[2*chars_read] != '\0' && chars_read < MAX_SIGNATURE_LEN) {
		if(sscanf(hexstring + (2*chars_read), "%2hhx", &current_char) != 1) {
			fprintf(stderr, "Could not parse hexstring \"%s\".\n", hexstring);
			free_and_exit(EXIT_FAILURE);
		}
		dest[chars_read] = current_char;
		chars_read += 1;
	}

	// Check if input was longer than MAX_SIGNATURE_LEN:
	if(hexstring[2*chars_read] != 0) {
		fprintf(stderr, "WARNING: Signature cut after %d bytes.\n", MAX_SIGNATURE_LEN);
	}

	return chars_read;
}

// --------

int main(int argc, char* argv[]) {

	// Check args:
	if(argc != 3) { print_usage_msg(stderr, argv[0]); free_and_exit(EXIT_FAILURE); }

	char signature[MAX_SIGNATURE_LEN];
	size_t signature_length = 0;
	const char* filename = argv[2];

	// Parse hexstring:
	signature_length = parse_hexstring(signature, argv[1]);

	// (Following `https://stackoverflow.com/a/54160576`:)

	// Open file:
	g_filedes = open(filename, O_RDONLY);
	if(g_filedes < 0) { perror("open"); free_and_exit(EXIT_FAILURE); }

	// Get size:
	off_t file_size = lseek(g_filedes, 0, SEEK_END);
	if(file_size < 0) { perror("lseek"); free_and_exit(EXIT_FAILURE); }

	// Create memory mapping for file:
	void* mapped_file = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, g_filedes, 0);
	if(mapped_file == MAP_FAILED) { perror("mmap"); free_and_exit(EXIT_FAILURE); }

	// Find all occurences of signature within file:
	int num_matches = 0;
	char* search_start = (char*) mapped_file;
	while(search_start + signature_length <= (char*) mapped_file + file_size) {
		char* match = (char*) memmem((void*) search_start, file_size, signature, signature_length);
		if(match) {
			num_matches++;
			printf("Found signature at offset 0x%lx\n", match - (char*) mapped_file);
		}
		else {
			break;
		}
		// Continue search from 1 byte behind match (so we find overlapping matches as well):
		search_start = match+ 1;
	}
	printf("Found %d match(es) in total.\n", num_matches);

	// Free resources and exit:
	munmap(mapped_file, file_size);
	close(g_filedes); g_filedes = -1;
	exit(EXIT_SUCCESS);
}


