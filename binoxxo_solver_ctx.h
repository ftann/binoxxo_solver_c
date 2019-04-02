// 
// binoxxo_solver_ctx.h
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


#ifndef BINOXXO_SOLVER_CTX_H
#define BINOXXO_SOLVER_CTX_H

#include "binoxxo.h"

///
/// Constant for inverting a field's value
///
#define BNX_INVERT_FIELD (-1)

///
/// Guess algorithm
///
enum BnxGuessMode {
	BNX_GUESS_NONE,
	BNX_GUESS_TOPLEFT,
	BNX_GUESS_MOSTFILLED,
	BNX_GUESS_RANDOM,
};

///
/// Define how many solutions should be searched
///
enum BnxSolutionMode {
	BNX_SOLUTION_MODE_ALL = -1,
	BNX_SOLUTION_MODE_ONE = 1,
};

// Forward definition
struct BnxSolverCtx;

///
/// Function pointer for a guesser function
/// Takes as a solver context and a binoxxo
/// Returns modification status
///
typedef struct Bnx ** (*BnxGuesserFnc)(struct BnxSolverCtx const *);

///
/// Hold current solver context including a map of guesses,
/// the solution tree and a pointer to the current root 
///
struct BnxSolverCtx {
	struct Bnx *guess_map;
	struct Bnx *root;
	struct Bnx *current;			// Pointer to current node
	struct BnxSolution *solution;
	BnxGuesserFnc guesser;
	int free_solutions;				// Free slots for solutions
};

///
/// \brief Update guess map of a context
///
/// \param Solver context
/// \param Guess map
///
void
bnx_update_guess_map(const struct Bnx *, const struct Bnx *);

///
/// \brief Allocate a solver context
///
/// \param Binoxxo data structure
/// \param Guess mode
/// \param Solution mode
/// \return Solver context 
///
struct BnxSolverCtx *
bnx_ctx_alloc(struct Bnx const * const, const int, const int);

///
/// \brief Free a solver context
///
/// \param Solver context
///
void
bnx_ctx_free(struct BnxSolverCtx *);

///
/// \brief Guesser function. This function guesses based on it's definition
/// 	which field should be filled.
///
/// \param Solver context 
/// \param Binoxxo data structure
/// \return Guessed solutions
///
#define MAKE_GUESSER(g) struct Bnx ** \
	bnx_guesser_##g (struct BnxSolverCtx const *);

///
/// Scan from top left field down to bottom left field
/// and then the next column
///
MAKE_GUESSER(topleft)

///
/// Search the most filled, not full line
///
MAKE_GUESSER(mostfilled)

///
/// Take a random field
///
MAKE_GUESSER(random)

///
/// Do not guess
///
MAKE_GUESSER(none)

#endif // BINOXXO_SOLVER_CTX_H

