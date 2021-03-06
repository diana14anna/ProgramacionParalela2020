/*
 ============================================================================
 Name        : NumberAv.c
 Author      : Diana
 Version     :
 Copyright   : Your copyright notice
 Description : Compute Pi in MPI C++
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <assert.h>

// Creates an array of random numbers. Each number has a value from 0 - 1
float *create_rand_nums(int num_elements) {
  float *rand_nums = (float *)malloc(sizeof(float) * num_elements);
  assert(rand_nums != NULL);
  int i;
  for (i = 0; i < num_elements; i++) {
    rand_nums[i] = (rand() / (float)RAND_MAX);
  }
  return rand_nums;
}

// Computes the average of an array of numbers
float compute_avg(float *array, int num_elements) {
  float sum = 0.f;
  int i;
  for (i = 0; i < num_elements; i++) {
    sum += array[i];
  }
  return sum / num_elements;
}

int main(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: avg num_elements_per_proc\n");
    exit(1);
  }

  int num_elements_per_proc = atoi(argv[1]);
  // Seed the random number generator to get different results each time
  srand(time(NULL));

  MPI_Init(NULL, NULL);

  int my_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  int comm_size;
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

  // Create a random array of elements on the root process. Its total
  // size will be the number of elements per process times the number
  // of processes
  float *list_nums = NULL;
  if (my_rank == 0) {
    list_nums = create_rand_nums(num_elements_per_proc * comm_size);
  }

  // For each process, create a buffer that will hold a subset of the entire
  // array
  float *sub_list_nums = (float *)malloc(sizeof(float) * num_elements_per_proc);
  assert(sub_list_nums != NULL);

  // Scatter the random numbers from the root process to all processes in
  // the MPI world
  MPI_Scatter(list_nums, num_elements_per_proc, MPI_FLOAT, sub_list_nums,
              num_elements_per_proc, MPI_FLOAT, 0, MPI_COMM_WORLD);

  // Compute the average of your subset
  float sub_avg = compute_avg(sub_list_nums, num_elements_per_proc);

  // Gather all partial averages down to all the processes
  float *sub_avgs = (float *)malloc(sizeof(float) * comm_size);
  assert(sub_avgs != NULL);
  MPI_Allgather(&sub_avg, 1, MPI_FLOAT, sub_avgs, 1, MPI_FLOAT, MPI_COMM_WORLD);

  // Now that we have all of the partial averages, compute the
  // total average of all numbers. Since we are assuming each process computed
  // an average across an equal amount of elements, this computation will
  // produce the correct answer.
  float avg = compute_avg(sub_avgs, comm_size);
  printf("Avg of all elements from proc %d is %f\n", my_rank, avg);

  // Clean up
  if (my_rank == 0) {
    free(list_nums);
  }
  free(sub_avgs);
  free(sub_list_nums);

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
}
