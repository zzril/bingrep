#define _GNU_SOURCE

// --------

#include <stdio.h>
#include <stdlib.h>

#include "../lib/src/bingrep.h"

// --------

#define MAX_SIGNATURE_LEN 0x40

// --------

static void print_usage_msg(FILE* stream, char* program_name);
static size_t parse_hexstring(char* dest, const char* hexstring);
static void print_offset(ptrdiff_t offset);
static void print_offset_verbose(ptrdiff_t offset);

// --------

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
			exit(EXIT_FAILURE);
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

static void print_offset(ptrdiff_t offset) {
	printf("0x%lx\n", offset);
}

static void print_offset_verbose(ptrdiff_t offset) {
	printf("Found signature at offset 0x%lx.\n", offset);
}

// --------

int main(int argc, char* argv[]) {

	// Check args:
	if(argc != 3) { print_usage_msg(stderr, argv[0]); exit(EXIT_FAILURE); }

	char signature[MAX_SIGNATURE_LEN];
	size_t signature_length = 0;
	const char* pathname = argv[2];

	// Parse hexstring:
	signature_length = parse_hexstring(signature, argv[1]);

	BINGREP_File file;
	BINGREP_open_file_at(&file, pathname);

	long num_matches = BINGREP_find_signature(&file, signature, signature_length, &print_offset_verbose);

	//printf("Found %lu match(es) in total.\n", num_matches);
	printf("Found %ld match(es) in total.\n", num_matches);

	// Free resources and exit:
	BINGREP_close_file_at(&file);
	exit(EXIT_SUCCESS);
}


