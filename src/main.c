#define _GNU_SOURCE

// --------

#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "../lib/src/bingrep.h"

// --------

static void print_usage_msg(FILE* stream, char* program_name);
static size_t parse_hexstring(char* dest, const char* hexstring, size_t dest_length);
static void print_offset(ptrdiff_t offset);
static void print_offset_verbose(ptrdiff_t offset);

// --------

static void print_usage_msg(FILE* stream, char* program_name) {
	fprintf(stream, "Usage: %s hex_signature filename", program_name);
}

static size_t parse_hexstring(char* dest, const char* hexstring, size_t max_dest_length) {

	char current_char = 0;
	size_t chars_read = 0;

	// Read two characters per iteration and add the parsed byte to dest:
	while(hexstring[2*chars_read] != '\0' && chars_read < max_dest_length) {
		if(sscanf(hexstring + (2*chars_read), "%2hhx", &current_char) != 1) {
			fprintf(stderr, "Could not parse hexstring \"%s\".\n", hexstring);
			exit(EXIT_FAILURE);
		}
		dest[chars_read] = current_char;
		chars_read += 1;
	}

	// Check if input was longer than `max_dest_length`:
	if(hexstring[2*chars_read] != 0) {
		fprintf(stderr, "WARNING: Signature cut after %zu bytes.\n", max_dest_length);
	}

	return chars_read;
}

static void print_offset(ptrdiff_t offset) {
	printf("%lx\n", offset);
}

static void print_offset_verbose(ptrdiff_t offset) {
	printf("Found signature at offset 0x%lx.\n", offset);
}

// --------

int main(int argc, char* argv[]) {

	// Check args:
	if(argc != 3) { print_usage_msg(stderr, argv[0]); exit(EXIT_FAILURE); }

	size_t signature_length = strlen(argv[1]) / 2;

	char* signature = calloc(signature_length, sizeof(char));
	if(signature == NULL) { perror("calloc"); exit(EXIT_FAILURE); }

	const char* pathname = argv[2];

	// Parse hexstring:
	signature_length = parse_hexstring(signature, argv[1], signature_length);

	// Open file:
	BINGREP_File* file = BINGREP_open_file(pathname, 0);
	if(file == NULL) { perror("BINGREP_open_file"); free(signature); exit(EXIT_FAILURE); }

	// Search for signature:
	long num_matches = BINGREP_find_signature(file, signature, signature_length, &print_offset_verbose);

	// Free resources:
	BINGREP_close_file(file);
	free(signature);

	printf("Found %ld match(es) in total.\n", num_matches);

	exit(EXIT_SUCCESS);
}


