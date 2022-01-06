/*
 * C postfix calculator
 * main function
 */

#include <stdio.h>
#include <stdlib.h>
#include "cPostfixCalc.h"

int main(int argc, char **argv) {
	if (argc != 2) {
		printf("Error: wrong number of arguments\n");
		return 1;
	}

	long result = eval(argv[1]);

	// prints result and exits normally
	printf("Result is: %ld\n", result);
	return 0;
}
