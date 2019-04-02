// 
// binoxxo.c
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


#include "binoxxo.h"

const size_t bnx_min_size = 4;

struct Bnx *
bnx_alloc(const size_t size)
{
	if (size < bnx_min_size || size % 2 != 0) {
		fprintf(stderr, "Size must be at least %zu and a multiple of 2\n",
			bnx_min_size);
		return NULL;
	}
	
	struct Bnx* b = malloc(sizeof(struct Bnx));
	if (b == NULL) {
		goto bnx_alloc_cleanup;
	}
	
	b->data = malloc(sizeof(enum BnxField*) * size);
	if (b->data == NULL) {
		goto bnx_alloc_cleanup_data;
	}
	
	int i = 0;
	for (i = 0; i < size; i++) {
		b->data[i] = (enum BnxField*)calloc(size, sizeof(enum BnxField));
		if (b->data[i] == NULL) {
			goto bnx_alloc_cleanup_rows;
		}
	}
	
	b->size = size;
	
	return b;
	
bnx_alloc_cleanup_rows:
	while(i) {
		i--;
		free(b->data[i]);
	}
	free(b->data);
	
bnx_alloc_cleanup_data:
	free(b);
	
bnx_alloc_cleanup:
	fprintf(stderr, "Could not allocate memory\n");
	return NULL;
}

int
bnx_get_random_field_value(void)
{
	// [M, N]
	// M + rand() / (RAND_MAX / (N - M + 1) + 1)	
	//return (enum BnxField)(rand() / (RAND_MAX / 2 + 1));
	//return (enum BnxField)(rand() % 3 - 1);

	if (rand() % 2) {
		return BNX_FIELD_O;
	} else {
		return BNX_FIELD_X;
	}

}

void
bnx_randomize(struct Bnx const *b)
{
	
	srand(time(NULL));
	
	const size_t size = b->size;
	int row;
	int col;
	for (row = 0; row < size; row++) {
		for (col = 0; col < size; col++) {
			b->data[row][col] = bnx_get_random_field_value();
		}
	}
}

int
bnx_copy(struct Bnx const *dest, struct Bnx const * const src)
{
	if (dest == NULL || src == NULL || dest->size != src->size) {
		return -ENOMEM;
	}

	int i;
	for (i = 0; i < src->size; i++) {
		memcpy(dest->data[i], src->data[i], src->size * sizeof(enum BnxField));
	}
	
	return 0;
}

void
bnx_free(struct Bnx *b)
{
	if (b != NULL) {
		if (b->data != NULL) {
			int i;
			for (i = 0; i < b->size; i++) {
				if (b->data[i] != NULL) {
					free(b->data[i]);
				}
			}
			free(b->data);
		}
		free(b);
	}
}

struct BnxLine *
bnx_get_line(struct Bnx const * const b, const int mode, const int index)
{
	const size_t size = b->size;
	
	struct BnxLine* line = malloc(sizeof(struct BnxLine));
	if (line == NULL) {
		fprintf(stderr, "Could not allocate memory for line\n");
		return NULL;
	} 
	
	line->data = malloc(sizeof(int*) * size);
	if (line->data == NULL) {
		fprintf(stderr, "Could not allocate memory for line data\n");
		free(line);
		return NULL;
	}
	
	line->size = size;
	
	int i;
	for (i = 0; i < size; i++) {
		
		if (mode & BNX_SCAN_H) {
			line->data[i] = &b->data[index][i];
		} else if (mode & BNX_SCAN_V) {
			line->data[i] = &b->data[i][index];
		}
	}
	
	return line;
}

void
bnx_free_line(struct BnxLine *line)
{
	free(line->data);
	free(line);
}

int
bnx_scan(struct Bnx const * const b, BnxScannerFnc scanner, const int mode)
{
	int i;
	int modified = true;
	struct BnxLine* line;
	
	for (i = 0; i < b->size; i++) {

		line = bnx_get_line(b, mode, i);
		modified = scanner(line);
		bnx_free_line(line);
		
		if ((mode & BNX_SCAN_ABORT) && modified) {
			break;
		}
	}
	
	return modified;
}

int
bnx_compare_lines(struct Bnx const * const b,
                  BnxCompareFnc compare, const int mode)
{
	const size_t size = b->size;
	int i;
	int j;
	int error = BNX_CORRECT;
	struct BnxLine *line;
	struct BnxLine *next;
	
	for (i = 0; i < size; i++) {

		line = bnx_get_line(b, mode, i);
		error = BNX_CORRECT;

		for (j = i + 1; j < size; j++) {
	
			next = bnx_get_line(b, mode, j);
			error |= compare(line, next);
			bnx_free_line(next);
		}

		bnx_free_line(line);
		
		if ((mode & BNX_SCAN_ABORT) && error) {
			break;
		}
	}
	
	return error;
}

static int
bnx_validate_rule_triple(const int triple)
{
	if (abs(triple) < BNX_FIELD_OVER) {
		return BNX_CORRECT;
	}
	return BNX_ERR_FOLLOW;
}

static int
bnx_validate_rule_count(const int sum)
{
	if (sum == BNX_FIELD_EMPTY) {
		return BNX_CORRECT;
	}
	return BNX_ERR_BALANCE;
}

static int
bnx_validate_rule_empty(const int field)
{
	if (field != BNX_FIELD_EMPTY) {
		return BNX_CORRECT;
	}
	return BNX_ERR_FILL;
}

static int
bnx_validate_line(struct BnxLine const * const line)
{
	const size_t size = line->size - 1;
	int i = 1;
	int triple;
	int sum = *line->data[0] + *line->data[size];
	int error = bnx_validate_rule_empty(*line->data[0])
		| bnx_validate_rule_empty(*line->data[size]);
	
	while (error == BNX_CORRECT && i < size) {
	
		sum    += *line->data[i];
		triple = *line->data[i - 1] + *line->data[i] + *line->data[i + 1];
		error  = bnx_validate_rule_empty(*line->data[i]);
		
		if (error == BNX_CORRECT) {
			error |= bnx_validate_rule_triple(triple);
		}
		i++;
	}
	
	if (i == size) {
		error |= bnx_validate_rule_count(sum);
	}

	return error;
}

static int
bnx_is_eq_and_not_empty(const int a, const int b)
{
	return a == b && a != BNX_FIELD_EMPTY;
}

static int
bnx_has_empty(struct BnxLine const * const l)
{
	const size_t size = l->size;
	int i = 0;
	while (i < size && *l->data[i] != BNX_FIELD_EMPTY) {
		i++;
	}

	return i < size;
}

int
bnx_line_count_empty(struct BnxLine const * const l)
{
	const size_t size = l->size;
	int i = 0;
	int count = 0;
	while (i < size) {
		if (*l->data[i] == BNX_FIELD_EMPTY) {
			count++;
		}
		i++;
	}

	return count;
}

static int
bnx_validate_unique(struct BnxLine const * const a,
                    struct BnxLine const * const b)
{
	if (bnx_has_empty(a) || bnx_has_empty(b)) {
		return BNX_CORRECT;
	}

	const size_t size = a->size;
	int i = 0;
	while (i < size && bnx_is_eq_and_not_empty(*a->data[i], *b->data[i])) {
		i++;
	}

	if (i < size) {
		return BNX_CORRECT;
	}
	return BNX_ERR_UNIQUE;
}

int
bnx_validate(struct Bnx const * const b)
{
	int flags_h = BNX_SCAN_H | BNX_SCAN_ABORT;
	int flags_v = BNX_SCAN_V | BNX_SCAN_ABORT;

	return bnx_scan(b, &bnx_validate_line, flags_h)
		| bnx_scan(b, &bnx_validate_line, flags_v)
		| bnx_compare_lines(b, &bnx_validate_unique, flags_h)
		| bnx_compare_lines(b, &bnx_validate_unique, flags_v);
}

char *
bnx_strerror(const int error)
{
	if (error & BNX_ERR_FILL) {
		return "Binoxxo not complete\n";
	}

	if (error & BNX_ERR_UNIQUE) {
		return "Duplicate line found\n";
	}

	if (error & BNX_ERR_BALANCE) {
		return "Uneven amount of 'o' and 'x' found\n";
	}
	
	if (error & BNX_ERR_FOLLOW) {
		return "Triple of 'o' and 'x' found\n";
	}

	if (error == BNX_CORRECT) {
		return "Binoxxo valid\n";
	}

	return "Undefined error\n";
}

size_t
bnx_get_size(struct Bnx const * const b)
{
	return b->size;
}

