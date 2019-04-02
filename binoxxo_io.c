// 
// binoxxo_io.c
// Copyright (C) 2014  Florian Tanner <florian.tanner@gmail.com>
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// 


#include "binoxxo_io.h"

const size_t bnx_buffer_size = 24;

static char
bnx_field_to_char(const int field)
{
	switch (field) {
		case BNX_FIELD_O:
			return BNX_LETTER_O;

		case BNX_FIELD_X:
			return BNX_LETTER_X;
			
		// Used in guess maps to indicate blocked fields
		case BNX_FIELD_OVER:
			return BNX_LETTER_OVER;

		default:
			return BNX_LETTER_EMPTY;
	}
}

void
bnx_print(struct Bnx const * const b)
{
	if (!b) {
		return;
	}

	const size_t size = b->size;
	int row;
	int col;
	for (row = 0; row < size; row++) {
		for (col = 0; col < size; col++) {
			printf("%c", bnx_field_to_char(b->data[row][col]));
		}
		printf("\n");
	}
	printf("\n");
}

void
bnx_line_print(struct BnxLine const * const l)
{
	if (!l) {
		return;
	}
	
	const size_t size = l->size;
	int i;
	for (i = 0; i < size; i++) {
		printf("%c", bnx_field_to_char(*l->data[i]));
	}
	printf("\n");
}

static void
bnx_get_input(char *buffer, const size_t size)
{
	while (true) {
		// Skip leading whitespace
		int c = getchar();
		if (c == EOF) {
			break;
		}
		if (!isspace(c)) {
			ungetc(c, stdin);
			break;
		}
	}

	int position = size;
	int i = 0;
	while (true) {
		int c = getchar();
		if (iscntrl(c) || c == EOF) {
			// At end, add terminating zero
			buffer[i] = '\0';
			break;
		}
	
		buffer[i] = c;
		if (i == position - 1) {
			// Buffer full
			// Get a new and larger buffer
			position <<= 1;
			buffer = (char*)realloc(buffer, position);
			if (buffer == NULL) {
				return;
			}
		}
		i++;
	}
}

static int
bnx_char_to_field(const char token)
{
	switch (token) {

		case 'O':
		case BNX_LETTER_O:
			return BNX_FIELD_O;

		case 'X':
		case BNX_LETTER_X:
			return BNX_FIELD_X;
			
		case ' ':
		case BNX_LETTER_EMPTY:
		default:
			return BNX_FIELD_EMPTY;
	}
}

static void
bnx_parse_line(struct Bnx const *b, const size_t row, const char *buffer)
{
	if (buffer == NULL || strlen(buffer) < b->size) {
		fprintf(stderr, "Input buffer is invalid\n");
		return;
	}

	int i = 0;
	for (i = 0; i < b->size; i++) {
		b->data[row][i] = bnx_char_to_field(buffer[i]);
	}
}

struct Bnx *
bnx_fill_by_input(void)
{
	char *buffer = malloc(sizeof(char) * bnx_buffer_size);
	if (buffer == NULL) {
		fprintf(stderr, "Could not allocate buffer for input\n");
		return NULL;
	}

	printf("Enter size: ");
	bnx_get_input(buffer, bnx_buffer_size);
	
	const size_t size = atoi(buffer);
	struct Bnx *b = bnx_alloc(size);
	if (b == NULL) {
		fprintf(stderr, "Could not allocate structure\n");
		return NULL;
	}
	
	int i;
	for (i = 0; i < b->size; i++) {
		printf("Enter row %i: ", i + 1);
		bnx_get_input(buffer, bnx_buffer_size);
		bnx_parse_line(b, i, buffer);
	}
	
	printf("\n");
	free(buffer);
	
	return b;
}

struct Bnx *
bnx_read_file(const char *filename)
{
	FILE *file = fopen(filename, "r");
	if (!file) {
		fprintf(stderr, "Could not open file\n");
		return NULL;
	}
	
	size_t size;
	fscanf(file, "%zu", &size);
	
	// line size + \n + null
	char *buffer = malloc(sizeof(char) * (size + 2));
	if (buffer == NULL) {
		fprintf(stderr, "Could not allocate memory\n");
		return NULL;
	}

	struct Bnx *b = bnx_alloc(size);
	
	int i;
	for (i = 0; i < size; i++) {
		fscanf(file, "%s\n", buffer);
		bnx_parse_line(b, i, buffer);		
	}
	
	free(buffer);
	fclose(file);
	
	return b;
}

int
bnx_write_file(struct Bnx const * const b, const char *filename)
{
	FILE* file = fopen(filename, "w");
	if (!file) {
		fprintf(stderr, "Could not open file\n");
		return 1;
	}
	
	const size_t size = b->size;
	
	fprintf(file, "%zu\n", size);
	
	int row;
	int col;
	for (row = 0; row < size; row++) {
		for (col = 0; col < size; col++) {
			fprintf(file, "%c", bnx_field_to_char(b->data[row][col]));
		}
		fprintf(file, "\n");
	}
	
	fclose(file);
	return true;
}

