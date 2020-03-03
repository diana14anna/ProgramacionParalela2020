/*
 ============================================================================
 Name        : labo13_01.c
 Author      : FF
 Version     :
 Copyright   : Your copyright notice
 Description : Hello OpenMP World in C
 ============================================================================
 */
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int max = 3;

int f(int x);
int g(int y);

int main(int argc, char *argv[]) {

	int x = 0;

	omp_set_num_threads(5);

	#pragma omp parallel
		{
			for (int i = 0; i < max; ++i) {

				x = f(i);

				printf("\nEl resultado 1 es: %d\n", x);

	#pragma omp critical
				g(x);

				printf("\nEl resultado 2 es: %d\n", g(x));
			}
		}
	return 0;
}

int f(int x) {
	x = x + 1;
	return x;
}

int g(int y) {
	y = y + 3;
	return y;
}

