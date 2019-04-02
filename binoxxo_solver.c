// 
// binoxxo_solver.c
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


#include "binoxxo_solver.h"

static int
bnx_invert_value(const int field)
{
	return BNX_INVERT_FIELD * field;
}

static int
bnx_is_field_empty(struct BnxLine const * const line, const int index)
{
	return *line->data[index] == BNX_FIELD_EMPTY;
}

static int
bnx_is_field_eq_not_empty(struct BnxLine const * const line,
                          const int a, const int b)
{
	return *line->data[a] == *line->data[b] && !bnx_is_field_empty(line, a);
}

struct BnxSolution *
bnx_solution_alloc(void)
{
	struct BnxSolution *s = malloc(sizeof(struct BnxSolution));
	if (!s) {
		fprintf(stderr, "Could not allocate memory for solution list\n");
		return NULL;
	}

	s->data = NULL;
	s->next = NULL;
	
	return s;
}

void
bnx_solution_free(struct BnxSolution *s)
{
	struct BnxSolution *cur = s;
	struct BnxSolution *last = s;
	
	while (cur) {
		if (cur->data) {
			bnx_free(cur->data);
		}
		last = cur;
		cur = cur->next;
		free(last);
	}
}

static int
bnx_scanner_double(struct BnxLine const *line)
{
	const size_t size = line->size - 1;
	int i;
	int inverted;
	int modified = false;
	int empty_prev;
	int empty_next = bnx_is_field_empty(line, 2);
	int equal_not_empty = bnx_is_field_eq_not_empty(line, 0, 1);

	// Scan |xx_
	if (equal_not_empty && empty_next) {
		*line->data[2] = bnx_invert_value(*line->data[0]);
		modified = true;
	}

	// Scan _xx_
	for (i = 1; i < size - 1; i++) {
		
		equal_not_empty = bnx_is_field_eq_not_empty(line, i, i + 1);

		if (equal_not_empty) {
		
			empty_prev = bnx_is_field_empty(line, i - 1);
			empty_next = bnx_is_field_empty(line, i + 2);
			inverted = bnx_invert_value(*line->data[i]);
		
			if (empty_prev) {
				*line->data[i - 1] = inverted;
				modified = true;
			}
			if (empty_next) {
				*line->data[i + 2] = inverted;
				modified = true;
			}		
		}
	}

	equal_not_empty = bnx_is_field_eq_not_empty(line, size, size - 1);
	empty_prev = bnx_is_field_empty(line, size - 2);

	// Scan _xx|
	if (equal_not_empty && empty_prev) {
		*line->data[size - 2] = bnx_invert_value(*line->data[size]);
		modified = true;
	}
	
	return modified;
}

static int
bnx_scanner_triple(struct BnxLine const *line)
{
	const size_t size = line->size - 2;
	int i;
	int modified = false;
	int equal_not_empty;
	int empty;
	
	for (i = 0; i < size; i++) {
		
		equal_not_empty = bnx_is_field_eq_not_empty(line, i, i + 2);
		empty = bnx_is_field_empty(line, i + 1);
		
		if (equal_not_empty && empty) {
			*line->data[i + 1] = bnx_invert_value(*line->data[i]);
			modified = true;
		}
	}
	
	return modified;
}

static int
bnx_get_filler(struct BnxLine const * const line)
{
	const size_t size = line->size;
	int i;
	int sum_o = 0;
	int sum_x = 0;
	int half = line->size / 2;

	for (i = 0; i < size && sum_o < half && sum_x < half; i++) {
		switch (*line->data[i]) {
		
			case BNX_FIELD_O:
				sum_o++;
				break;
			
			case BNX_FIELD_X:
				sum_x++;
				break;
		}
	}
	
	if (sum_o == half && sum_x < half) {
		return BNX_FIELD_X;
	} else if (sum_x == half && sum_o < half) {
		return BNX_FIELD_O;
	}
	return BNX_FIELD_EMPTY;
}

static int
bnx_fill_empty(struct BnxLine const *line, const int filler)
{
	const size_t size = line->size;
	int i;
	int modified = false;
	
	for (i = 0; i < size; i++) {
		if (bnx_is_field_empty(line, i)) {
			*line->data[i] = filler;
			modified = true;
		}
	}
	
	return modified;
}

static int
bnx_scanner_count(struct BnxLine const * const line)
{
	int modified = false;
	int filler = bnx_get_filler(line);
	
	if (filler != BNX_FIELD_EMPTY) {
		modified = bnx_fill_empty(line, filler);
	}

	return modified;
}

static int
bnx_solve_trivial(struct Bnx const *progress)
{
	int error = bnx_validate(progress);
	int modified = true;

	while (error != BNX_CORRECT && modified) {
	
		modified = bnx_scan(progress, &bnx_scanner_double, BNX_SCAN_H)
			| bnx_scan(progress, &bnx_scanner_double, BNX_SCAN_V)
			| bnx_scan(progress, &bnx_scanner_triple, BNX_SCAN_H)
			| bnx_scan(progress, &bnx_scanner_triple, BNX_SCAN_V)
			| bnx_scan(progress, &bnx_scanner_count, BNX_SCAN_H)
			| bnx_scan(progress, &bnx_scanner_count, BNX_SCAN_V);

		error = bnx_validate(progress);
	}

	return error;
}

static int
bnx_solve_rec(struct BnxSolverCtx *ctx)
{

	if (ctx->current == NULL) {
		return BNX_ERR_UNKNOWN;
	}

	int error = bnx_solve_trivial(ctx->current);
	
	// Shortcut
	if (error & ~BNX_ERR_FILL) {
		goto bnx_solve_rec_shortcut;
	}
	
	if (error & BNX_ERR_FILL) {

		bnx_update_guess_map(ctx->guess_map, ctx->current);

		struct Bnx **child = ctx->guesser(ctx);
		int i;
		for (i = 0; i < 2; i++) {
			ctx->current = child[i];
			if (bnx_solve_rec(ctx) != BNX_CORRECT) {
				bnx_free(child[i]);
			}
		}
	
		free(child);

	} else if (error == BNX_CORRECT && ctx->free_solutions != 0) {

		struct BnxSolution *solution = bnx_solution_alloc();
		solution->data = ctx->current;
		solution->next = ctx->solution;
		ctx->solution = solution;

		if (ctx->free_solutions != BNX_SOLUTION_MODE_ALL) {
			ctx->free_solutions--;
		}

	} 

bnx_solve_rec_shortcut:
	return error;
}

struct BnxSolution *
bnx_solve(struct Bnx const * const b, const int mode, const int sol_mode)
{
	struct BnxSolverCtx *ctx = bnx_ctx_alloc(b, mode, sol_mode);

#ifdef BNX_TIMING
	const clock_t start = clock();
#endif

	bnx_solve_rec(ctx);	

#ifdef BNX_TIMING
	const clock_t end = clock();
	printf("Time: %.2f s\n", (double)(end - start) / CLOCKS_PER_SEC);
#endif

	struct BnxSolution *solutions = ctx->solution;
	bnx_ctx_free(ctx);

    return solutions;
}

