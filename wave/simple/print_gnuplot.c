
#include <stdio.h>
#include "print_gnuplot.h"

void print_gnuplot(const int m, const int n, const double values[m][n]) {

  int i, j;

  for (i=0; i < m; i++) {
    for (j=0; j < n; j++) {

      printf("%d %d % 5.2f\n", i, j, values[i][j]);
    }
    printf("\n");
  }
  return;
}

