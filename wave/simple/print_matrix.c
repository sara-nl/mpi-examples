
#include <stdio.h>
#include "print_matrix.h"

void print_matrix(const int m, const int n, const double values[m][n]) {

	int i, j;

	for (i=0; i < m; i++) {
		for (j=0; j < n; j++) {

			printf(" % 5.2f", values[i][j]);
		}
		printf("\n");
	}
	return;
}

