
#include "binoxxo.h"
#include "binoxxo_io.h"
#include "binoxxo_solver.h"

int
main(int argc, char **argv)
{
	// Determine unsolved binoxxo file
	char *file;
	if (argc == 1) {
		//file = "./data/6x6_1.binoxxo";
		file = "./data/14x14_veryhard_2.binoxxo";
	} else if (argc >= 2) {
		file = argv[1];
	}

	struct Bnx *b = bnx_read_file(file);
	if (!b) {
		printf("Could not create binoxxo from file %s\n", file);
		return EXIT_FAILURE;
	}

	puts("Read binoxxo:\n");
	bnx_print(b);

	struct BnxSolution *s =
		bnx_solve(b, BNX_GUESS_TOPLEFT, BNX_SOLUTION_MODE_ALL);

	if (s) {
		puts("Solved binoxxo:\n");

		struct BnxSolution *ps = s;
		do {
			bnx_print(ps->data);
			ps = ps->next;
		} while (ps);
	} else {
		puts("No solution");
	}

	bnx_solution_free(s);
	bnx_free(b);

	return EXIT_SUCCESS;
}
