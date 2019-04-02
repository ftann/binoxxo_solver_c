// 
// binoxxo_io.h
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


#ifndef BINOXXO_INPUT_H
#define BINOXXO_INPUT_H

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "binoxxo.h"

///
/// Define default buffer size
///
extern const size_t bnx_buffer_size;

///
/// \brief Print binoxxo to console
///
/// \param Binoxxo data structure
///
void
bnx_print(struct Bnx const * const);

///
/// \brief Print binoxxo line to console
///
/// \param Binoxxo line data structure
///
void
bnx_line_print(struct BnxLine const * const);

///
/// \brief Fill binoxxo data structure with user input
///
/// \return Binoxxo data structure
///
struct Bnx *
bnx_fill_by_input(void);

///
/// \brief Read a binoxxo from a file
///
/// 	File must be in following format:
///
/// 	First line is the size n.
/// 	The following n lines contain n chars specifing the field content.
///
///		A char must be either of 'o','x' or '_'. The latter defines an empty
/// 	field.
///
/// \param Filename
/// \return Binoxxo data structure
///
struct Bnx *
bnx_read_file(const char *);

///
/// \brief Fill binoxxo data structure with user input
///
/// \param Binoxxo data structure
/// \param Filename
/// \return Error
///
int
bnx_write_file(struct Bnx const * const, const char *);

#endif // BINOXXO_INPUT_H

