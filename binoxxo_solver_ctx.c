// 
// binoxxo_solver_ctx.c
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


#include "binoxxo_solver_ctx.h"

static struct Bnx *
bnx_alloc_guess_map(struct Bnx const * const b)
{
	struct Bnx *map = bnx_alloc(b->size);

	bnx_update_guess_map(map, b);

	return map;
}

void
bnx_update_guess_map(struct Bnx const *map, struct Bnx const *b)
{
	const size_t size = b->size;
	int row;
	int col;

	for (row = 0; row < size; row++) {
		for (col = 0; col < size; col++) {
			int guess = BNX_FIELD_OVER;
			if (b->data[row][col] == BNX_FIELD_EMPTY) {
				guess = BNX_FIELD_EMPTY;
			}
			map->data[row][col] = guess;
		}
	}
}

static BnxGuesserFnc
bnx_get_guesser(const int mode)
{
	switch (mode) {
		case BNX_GUESS_TOPLEFT:
			return &bnx_guesser_topleft;

		case BNX_GUESS_RANDOM:
			return &bnx_guesser_random;

		case BNX_GUESS_NONE:
			return &bnx_guesser_none;

		case BNX_GUESS_MOSTFILLED:
		default:
			return &bnx_guesser_mostfilled;
	}
}

struct BnxSolverCtx *
bnx_ctx_alloc(struct Bnx const * const b, const int mode, const int sol_mode)
{
	struct BnxSolverCtx *ctx = malloc(sizeof(struct BnxSolverCtx));

	ctx->root = bnx_alloc(b->size);
	bnx_copy(ctx->root, b);

	ctx->current		= ctx->root;
	ctx->guess_map      = bnx_alloc_guess_map(b);
	ctx->guesser        = bnx_get_guesser(mode);
	ctx->free_solutions = sol_mode;

	return ctx;
}

void
bnx_ctx_free(struct BnxSolverCtx *ctx)
{
	bnx_free(ctx->guess_map);
	bnx_free(ctx->root);
	free(ctx);
}

struct Bnx **
bnx_guesser_topleft(struct BnxSolverCtx const *ctx)
{
	const size_t size = ctx->current->size;
	int col;
	int row;

	struct Bnx **child = malloc(sizeof(struct Bnx *) * 2);

	int i;
	for (i = 0; i < 2; i++) {
		child[i] = bnx_alloc(size);
		bnx_copy(child[i], ctx->current);
	}

	for (row = 0; row < size; row++) {
		for (col = 0; col < size; col++) {

			int *field = &ctx->guess_map->data[row][col];
			if (*field == BNX_FIELD_EMPTY) {

				child[0]->data[row][col] = BNX_FIELD_O;
				child[1]->data[row][col] = BNX_FIELD_X;

				*field = BNX_FIELD_OVER;
				return child;
			}

		}
	}

	bnx_free(child[0]);
	bnx_free(child[1]);
	free(child);

	return NULL;
}

struct Bnx **
bnx_guesser_mostfilled(struct BnxSolverCtx const *ctx)
{
	const size_t size = ctx->current->size;
	int i;
	int empty_fields;
	int min_row = size;
	int min_col = size;
	int row = 0;
	int col = 0;

	struct Bnx **child = malloc(sizeof(struct Bnx *) * 2);

	for (i = 0; i < 2; i++) {
		child[i] = bnx_alloc(size);
		bnx_copy(child[i], ctx->current);
	}

	struct BnxLine *line;

	for (i = 0; i < size; i++) {

		line = bnx_get_line(ctx->current, BNX_SCAN_H, i);
		empty_fields = bnx_line_count_empty(line);

		if (empty_fields < min_row) {
			min_row = empty_fields;
			row = i;
		}

		bnx_free_line(line);

		line = bnx_get_line(ctx->current, BNX_SCAN_V, i);
		empty_fields = bnx_line_count_empty(line);

		if (empty_fields < min_col) {
			min_col = empty_fields;
			col = i;
		}

		bnx_free_line(line);
	}

	if (min_row < min_col) {
		line = bnx_get_line(ctx->current, BNX_SCAN_H, row);
	} else {
		line = bnx_get_line(ctx->current, BNX_SCAN_V, col);
	}

	for (i = 0; i < size; i++) {
		if (*line->data[i] == BNX_FIELD_EMPTY) {
			if (min_row < min_col) {
				col = i;
			} else {
				row = i;
			}
			break;
		}
	}

    bnx_free_line(line);

	ctx->guess_map->data[row][col] = BNX_FIELD_OVER;

	child[0]->data[row][col] = BNX_FIELD_O;
	child[1]->data[row][col] = BNX_FIELD_X;

	return child;
}

struct Bnx **
bnx_guesser_random(struct BnxSolverCtx const *ctx)
{
    // todo implement
	return NULL;
}

struct Bnx **
bnx_guesser_none(struct BnxSolverCtx const *ctx)
{
	return NULL;
}

