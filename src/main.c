#define _GNU_SOURCE

// --------

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <unistd.h>

#include "../lib/src/bingrep.h"

// --------

typedef void (*ResultPrinter)(long);

typedef struct {
	char* signature;
	size_t signature_length;
	BINGREP_File* file;
} Resources;

typedef struct {
	int verbose;
	int count;
	int finish_early;
	BINGREP_MatchHandler match_handler;
	ResultPrinter result_printer;
} Options;

// --------

static void parse_args(int argc, char** argv, Options* options);
static void parse_flags(int argc, char** argv, Options* options);
static void finish_flags(int argc, char** argv, Options* options);
static size_t parse_hexstring(char* dest, const char* hexstring, size_t dest_length);

static void print_usage_msg(FILE* stream, char* program_name, int detailed);
static void fail_with_usage_msg(char* program_name);

static void print_offset(ptrdiff_t offset);
static void print_offset_verbose(ptrdiff_t offset);

static void print_result_count_only(long count);
static void print_results_verbose(long count);

static void free_resources();
static void free_and_exit(int exit_code);

// --------

static Resources g_resources = {
	NULL,	// signature
	0,	// signature_length
	NULL,	// file
};

// --------

static void parse_args(int argc, char** argv, Options* options) {

	// Process options:
	parse_flags(argc, argv, options);

	// Process signature argument:

	size_t max_signature_length = strlen(argv[optind]) / 2;

	g_resources.signature = calloc(max_signature_length, sizeof(char));
	if(g_resources.signature == NULL) { perror("calloc"); free_and_exit(EXIT_FAILURE); }

	g_resources.signature_length = parse_hexstring(g_resources.signature, argv[optind], max_signature_length);

	// Parse filename argument:
	g_resources.file = BINGREP_open_file(argv[optind + 1], 0);
	if(g_resources.file == NULL) { free_and_exit(EXIT_FAILURE); }

	return;
}

static void parse_flags(int argc, char** argv, Options* options) {
	int opt;
	while((opt = getopt(argc, argv, "chsv")) != -1 ) {
		switch(opt) {
			case 'c':
				options->count = 1;
				break;
			case 'h':
				print_usage_msg(stdout, argv[0], 1);
				free_and_exit(EXIT_SUCCESS);
				break;
			case 's':
				options->verbose = -1;
				break;
			case 'v':
				options->verbose = 1;
				break;
			default:
				fail_with_usage_msg(argv[0]);
		}
	}
	finish_flags(argc, argv, options);
	return;
}

static void finish_flags(int argc, char** argv, Options* options) {
	if(argc != optind + 2) {
		fail_with_usage_msg(argv[0]);
	}
	if(options->verbose < 0) {
		options->match_handler = NULL;
		options->result_printer = NULL;
		options->finish_early = 1;
	}
	if(options->verbose > 0) {
		options->match_handler = &print_offset_verbose;
		options->result_printer = &print_results_verbose;
	}
	if(options->count) {
		options->finish_early = 0;
		options->match_handler = NULL;
		options->result_printer = &print_result_count_only;
	}
	return;
}

static void print_usage_msg(FILE* stream, char* program_name, int detailed) {
	fprintf(stream, "Usage: %s [-chsv] hex_signature filename\n", program_name);
	if(detailed && stream == stdout) {
		puts("");
		puts("Options:");
		puts("-c: Count: Instead of normal output, only print the number of matches.");
		puts("-h: Help: Print a usage message (with options) and exit.");
		puts("-s: Silent: Do not output anything. Return code will (as in the other");
		puts("    cases) be 0 iff at least one match was found and no error occured.");
		puts("-v: Verbose: Print actual text, not just numbers. Always recommended if");
		puts("    your're not running this as part of a pipeline.");
	}
	return;
}

static void fail_with_usage_msg(char* program_name) {
	print_usage_msg(stderr, program_name, 0);
	free_and_exit(EXIT_FAILURE);
}

static size_t parse_hexstring(char* dest, const char* hexstring, size_t max_dest_length) {

	char current_char = 0;
	size_t chars_read = 0;

	// Read two characters per iteration and add the parsed byte to dest:
	while(hexstring[2*chars_read] != '\0' && chars_read < max_dest_length) {
		if(sscanf(hexstring + (2*chars_read), "%2hhx", &current_char) != 1) {
			fprintf(stderr, "Could not parse hexstring \"%s\".\n", hexstring);
			free_and_exit(EXIT_FAILURE);
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
	return;
}

static void print_offset_verbose(ptrdiff_t offset) {
	printf("Found signature at offset 0x%lx.\n", offset);
	return;
}

static void print_result_count_only(long count) {
	printf("%ld\n", count);
	return;
}

static void print_results_verbose(long count) {
	printf("Found %ld match(es) in total.\n", count);
	return;
}

static void free_resources() {
	if(g_resources.signature != NULL) {
		free(g_resources.signature);
		g_resources.signature = NULL;
	}
	if(g_resources.file != NULL) {
		BINGREP_close_file(g_resources.file);
		g_resources.file = NULL;
	}
	return;
}

static void free_and_exit(int exit_code) {
	free_resources();
	exit(exit_code);
}

// --------

int main(int argc, char** argv) {

	// Process command-line arguments:

	Options options = {
		0,		// verbose
		0,		// count
		0,		// finish_early
		&print_offset,	// match_handler
		NULL,		// result_printer
	};

	parse_args(argc, argv, &options);

	// Search for signature:
	long num_matches = BINGREP_find_signature(g_resources.file, g_resources.signature, g_resources.signature_length, options.match_handler, options.finish_early);

	// Free resources:
	free_resources();

	// Print results:
	if(options.result_printer != NULL) {
		options.result_printer(num_matches);
	}

	// Exit:
	free_and_exit(num_matches != 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}


