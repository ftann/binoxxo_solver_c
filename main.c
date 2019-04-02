
#include "binoxxo.h"
#include "binoxxo_io.h"
#include "binoxxo_solver.h"

int
main(int argc, char **argv)
{
	//char *file = "./data/12x12_1.binoxxo";
	char *file = "./data/14x14_veryhard_2.binoxxo";

	struct Bnx *b = bnx_read_file(file);
	if (!b) {
		printf("Could not create binoxxo from file %s\n", file);
		return EXIT_FAILURE;
	}

	puts("Read binoxxo:\n");
	bnx_print(b);

	struct BnxSolution *s =
		bnx_solve(b, BNX_GUESS_TOPLEFT, BNX_SOLUTION_MODE_ALL);

	if (!s) {
		puts("No solution");
	} else {
		puts("Solved binoxxo:\n");

		struct BnxSolution *ps = s;
		do {
			bnx_print(ps->data);
			ps = ps->next;
		} while (ps);

	}

	bnx_solution_free(s);
	bnx_free(b);

	return EXIT_SUCCESS;
}
