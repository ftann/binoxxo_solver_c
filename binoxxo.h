// 
// binoxxo.h
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


#ifndef BINOXXO_H
#define BINOXXO_H

#include <errno.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

///
/// Enable null pointer and safety checks during runtime
///
#define BNX_SAFETY_CHECKS

///
/// Define default signs
///
enum BnxLetter {
	BNX_LETTER_O     = 'o',
	BNX_LETTER_X     = 'x',
	BNX_LETTER_EMPTY = '_',
	BNX_LETTER_OVER  = '$',		// Used in guess maps to indicate blocked field
};

///
/// Error data type
///
enum BnxError {
	BNX_CORRECT     = 0,
	BNX_ERR_FILL    = 1 << 0,
	BNX_ERR_UNIQUE  = 1 << 1,
	BNX_ERR_BALANCE = 1 << 2,
	BNX_ERR_FOLLOW  = 1 << 3,
	BNX_ERR_UNKNOWN = 1 << 4,
};

///
/// Field data type
///
enum BnxField {
	BNX_FIELD_O     = -1,
	BNX_FIELD_EMPTY = 0,
	BNX_FIELD_X     = 1,
	BNX_FIELD_OVER  = 2,
};

///
/// Scanner operation mode
///
enum BnxScan {
	BNX_SCAN_H     = 1 << 0,
	BNX_SCAN_V     = 1 << 1,
	BNX_SCAN_ABORT = 1 << 2,	// Abort if result of scanner is true
};

///
/// Structure for holding pointers to line data
///
struct BnxLine {
	size_t size;
	int **data;		// Array of pointers referencing the value in
					// a binoxxo matrix
};

///
/// Binoxxo data structure
///
struct Bnx {
	size_t size;
	int **data;		// Dynamic array for storing the binoxxo as matrix
};

///
/// Function pointer for a solver function
/// Takes as argument a line
/// Returns modification status
///
typedef int
(*BnxScannerFnc)(const struct BnxLine *);

///
/// Function pointer for a comparator function
/// Takes as argument two lines
/// Returns modification status
///
typedef int
(*BnxCompareFnc)(struct BnxLine const *, struct BnxLine const *);

///
/// Minimal sane size of a binoxxo
///
extern const size_t bnx_min_size;

///
/// \brief Initialize binoxxo data structure
///
/// \param Size of binoxxo. Must be multiple of 2
/// \return Binoxxo data structure
///
struct Bnx *
bnx_alloc(const size_t);

///
/// \brief Set random values to all fields
/// \todo May returns invalid binoxxos!
///
/// \param Binoxxo data structure
///
void
bnx_randomize(struct Bnx const *);

///
/// \brief Copy data fields to another binoxxo 
///
/// \param Destination
/// \param Source
/// \return Error
///
int
bnx_copy(struct Bnx const *, struct Bnx const *);

///
/// \brief Free used memory
///
/// \param Binoxxo data structure
///
void
bnx_free(struct Bnx *);

///
/// \brief Get a line from a binoxxo
///
/// \param Binoxxo data structure
/// \param Line mode, horizontal or vertical
/// \param Index
/// \return Line data
///
struct BnxLine *
bnx_get_line(struct Bnx const *, const int, const int);

///
/// \brief Free memory used by a binoxxo line
///
/// \param Binoxxo line structure
///
void
bnx_free_line(struct BnxLine *);

///
/// \brief Scan all lines with a callback function
///
/// \param Binoxxo line structure
/// \param Solver function
/// \param Line mode
/// \return Modified
///
int
bnx_scan(struct Bnx const *, BnxScannerFnc, const int);

///
/// \brief Scan all lines with a callback function
///
/// \param Binoxxo line structure
/// \param Solver function
/// \param Line mode
/// \return Modified
///
int
bnx_compare_lines(struct Bnx const *, BnxCompareFnc, const int);

///
/// \brief Scan a line for empty fields
///
/// \param Binoxxo line structure
/// \return Number of empty fields
///
int
bnx_line_count_empty(struct BnxLine const * const);

///
/// \brief Validate all rules of a binoxxo:
/// 	1. Not more than 2 following o's or x's in a row or column
/// 	2. In every column and row equal amount of o's and x's
/// 	3. All rows and columns are unique
///
/// \param Binoxxo data structure
/// \return Error
///
int
bnx_validate(struct Bnx const *);

///
/// \brief Return error number as string
///
/// \param Error
/// \return String
///
char *
bnx_strerror(const int);

///
/// \brief Return size
///
/// \param Binoxxo data structure
/// \return Size
///
size_t
bnx_get_size(struct Bnx const *);

#endif // BINOXXO_H

