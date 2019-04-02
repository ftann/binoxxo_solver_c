// 
// binoxxo_solver.h
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


#ifndef BINOXXO_SOLVER_H
#define BINOXXO_SOLVER_H

#include "binoxxo.h"
#include "binoxxo_solver_ctx.h"

///
/// Enable time measuring of solving function for binoxxo
///
#define BNX_TIMING

///
/// Hold pointers to different possible solutions of a binoxxo
/// Implemented as linked list
///
struct BnxSolution {
	struct Bnx *data;
	struct BnxSolution *next;
};

///
/// \brief Allocate linked list with binoxxo solutions
///
/// \return Empty linked list
///
struct BnxSolution *
bnx_solution_alloc(void);

///
/// \brief Free used memory of found solutions
///
/// \param Linked list with solutions
///
void
bnx_solution_free(struct BnxSolution *);

///
/// \brief Solve a binoxxo and return solution.
///
/// 	1. Scan binoxxo for doulbe x's and o's (_xx_), if an occurrence is
/// 	found add before and or after an inverted letter (oxxo)
///
/// 	2. Scan for groups of three with an empty sign in between (x_x), if
/// 	found add inverted letter (xox)
///
/// 	3. Check for lines with maximum amount of a letter
/// 	(size / 2 == amount of a letter) fill line with inverted letters
///
/// 	If previous steps have modified data then repeat first 3 steps
/// 	otherwise look for the line with the greatest amount of letters and
/// 	make a guess
///
/// \param Binoxxo data structure
/// \param Guess mode
/// \param Solution mode
/// \return Solutions
///
struct BnxSolution *
bnx_solve(struct Bnx const * const, const int, const int);

#endif // BINOXXO_SOLVER_H

