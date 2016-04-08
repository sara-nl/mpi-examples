
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "print_gnuplot.h"

/************************************************************************
 

         i ->

           0       1       2

       +-------+-------+-------+-------+-- -- --
       |       |       |       |       |
  j 0  |   0   |   1   |   2   |  ...  |
       |       |       |       |       |
  |    +-------+-------+-------+-------+-- -- --
  V    |       |       |       |       |
    1  |   m   |  m+1  |  m+2  |  ...  |
       |       |       |       |       |
       +-------+-------+-------+-------+-- -- --
       |       |       |       |       |
    2  |  2m   | 2m+1  | 2m+2  |  ...  |
       |       |       |       |       |




*************************************************************************/
int main(int argc, char** argv) {

  /* problem definition */
  const double c = 0.15;

  /* grid definition */
  const int m = 100;
  const int n = 100;


  const int    ts     = 80000;       /* number of steps in time */
  const double t_end  = 320.0;

  const double r0     = 8.0;

  double u [m][n];    /* u      */
  double v [m][n];    /* du/dt  */

  double uk[m][n];    /* update of u */
  double vk[m][n];    /* update of v */

  /* grid size, step sizes */
  double size_x = 10.0;
  double size_y = 10.0;


  double dx = size_x / (double) m;
  double dy = size_y / (double) n;

  double dt = t_end / (double) ts;

  /* for determining the radius, for initial condition */
  double r;
  const double pi = 4.0 * atan(1.0);

  /* discretisation, finite differences */
  double uxx, uyy;   /* d^2 u / dx^2; d^2 u / dy^2 */

  /* loop counters */
  int i, j, k;

  /* start of the program */
  /* initial condition */

  /* set u to zero in the whole domain, except u(21,16) = 1 */
  for (i = 0; i < m; i++) {
    for (j = 0; j < n; j++) {

      /* distance to (i,j) = (30,90) */
      r = sqrt( (double)((i-30)*(i-30)) + (double)((j-90)*(j-90)) );
 
      u[i][j] = (r < r0 ? cos((r / r0) * (pi / 2.0)) : 0.0);
      v[i][j] = 0.0;
    }
  }

  /* print the initial condition */
  print_gnuplot(m, n, u);

  /* loop over time */

  for (k=0; k <= ts; k++) {
  
    /* first, do the interior */
    for (i = 0; i < m ; i++) {
      for (j = 0; j < n ; j++) {

        /* determine d^ u / dx^2 */

        if (j == 0) {
          
          /* n boundary: dirichlet boundary condition */
          uxx = -3 * u[i][0] + u[i][1];
        }
        else if (j == n - 1) {
        
          /* s boundary: neumann boundary condition */
          uxx = u[i][n-2] - u[i][n-1];
        }
        else {

          /* interior point in i-direction */
          uxx = u[i][j-1] - 2 * u[i][j] + u[i][j+1];
        }

        uxx /= (dx * dx);


        /* determine d^2 u / dy^2 */
          
        if (i == 0) {
          /* w boundary: dirichlet boundary condition */
          uyy = -3 * u[0][j] + u[1][j];

        }
        else if (i == m - 1) {
          /* e boundary: dirichlet boundary condition */
          uyy = u[m-2][j] - 3 * u[m-1][j];
        }
        else {
          /* interior point in j-direction */
          uyy = u[i-1][j] - 2 * u[i][j] + u[i+1][j];
        }
       
        uyy /= (dy * dy);

        /* integrate forward in time */

        uk[i][j] = u[i][j] + dt * (1 - 0.50) * v[i][j];
        vk[i][j] = v[i][j] + dt * c * c * (uxx + uyy); 
      }
    }

    if (k % 800 == 0) {
      /* print the matrix at time t */
      printf("\n\n");
      print_gnuplot(m, n, uk);
    }

    /* copy the new state into the original state */
    memcpy(u, uk, m * n * sizeof(double));
    memcpy(v, vk, m * n * sizeof(double));

  } /* next k */
 
  return 0;
}

