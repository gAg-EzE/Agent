#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

int main(int argc, char* argv[]) {

    int rank, size;
    long long int n = 1000000;   // nombre total de tirages
    long long int local_n;
    long long int i;
    long long int local_in = 0;
    long long int global_in = 0;

    double x, y;
    double pi;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc >= 2) {
        n = atoll(argv[1]);
    }

    local_n = n / size;

    srand(time(NULL) + rank);

    for (i = 0; i < local_n; i++) {
        x = (double) rand() / RAND_MAX;
        y = (double) rand() / RAND_MAX;

        if (x*x + y*y <= 1.0) {
            local_in++;
        }
    }

    MPI_Reduce(&local_in, &global_in, 1, MPI_LONG_LONG,
               MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        pi = 4.0 * (double) global_in / (double) n;
        printf("=================================\n");
        printf("Monte Carlo MPI simple\n");
        printf("tirages total = %lld\n", n);
        printf("processus     = %d\n", size);
        printf("pi estime     = %.15f\n", pi);
        printf("pi reference  = %.15f\n", acos(-1.0));
        printf("=================================\n");
    }

    MPI_Finalize();
    return 0;
}
