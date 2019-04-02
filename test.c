// 
// test.c
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


#include "test.h"

struct Bnx* bnx_valid_4x4(void)
{
	
	struct Bnx* b = bnx_alloc(4);
	
	b->data[0][0] = BNX_FIELD_X;
	b->data[0][1] = BNX_FIELD_X;
	b->data[0][2] = BNX_FIELD_O;
	b->data[0][3] = BNX_FIELD_O;
	
	b->data[1][0] = BNX_FIELD_O;
	b->data[1][1] = BNX_FIELD_O;
	b->data[1][2] = BNX_FIELD_X;
	b->data[1][3] = BNX_FIELD_X;
	
	b->data[2][0] = BNX_FIELD_X;
	b->data[2][1] = BNX_FIELD_O;
	b->data[2][2] = BNX_FIELD_X;
	b->data[2][3] = BNX_FIELD_O;
	
	b->data[3][0] = BNX_FIELD_O;
	b->data[3][1] = BNX_FIELD_X;
	b->data[3][2] = BNX_FIELD_O;
	b->data[3][3] = BNX_FIELD_X;
	
	return b;
}

void test(void)
{

	struct Bnx* test = bnx_valid_4x4();
	enum BnxError error = bnx_validate(test);

	assert(error == BNX_CORRECT);
	
	bnx_free(test);
	
	//assert(test == NULL);

}

