#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

double f(double x) {
    return 1.0 / (1.0 + x * x);
}

double Trap(double left_endpt, double right_endpt, int trap_count, double base_len) {
    double estimate, x;
    int i;

    estimate = (f(left_endpt) + f(right_endpt)) / 2.0;
    for (i = 1; i <= trap_count - 1; i++) {
        x = left_endpt + i * base_len;
        estimate += f(x);
    }
    estimate = estimate * base_len;

    return estimate;
}

int main(int argc, char* argv[]) {
    int my_rank, comm_sz;
    int n = 5000, local_n;
    double a = 0.0, b = 1.0, h, local_a, local_b;
    double local_int, total_int;
    int source;
    double start, end;
    double pi_est, pi_ref, erreur;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    if (argc >= 2) {
        n = atoi(argv[1]);
    }

    if (n % comm_sz != 0) {
        if (my_rank == 0) {
            printf("Erreur : n doit etre divisible par le nombre de processus.\n");
        }
        MPI_Finalize();
        return 1;
    }

    h = (b - a) / n;
    local_n = n / comm_sz;

    local_a = a + my_rank * local_n * h;
    local_b = local_a + local_n * h;

    MPI_Barrier(MPI_COMM_WORLD);
    start = MPI_Wtime();

    local_int = Trap(local_a, local_b, local_n, h);

    if (my_rank != 0) {
        MPI_Send(&local_int, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    } else {
        total_int = local_int;
        for (source = 1; source < comm_sz; source++) {
            MPI_Recv(&local_int, 1, MPI_DOUBLE, source, 0,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total_int += local_int;
        }
    }

    end = MPI_Wtime();

    if (my_rank == 0) {
        pi_est = 4.0 * total_int;
        pi_ref = acos(-1.0);
        erreur = fabs(pi_est - pi_ref);

        printf("=====================================\n");
        printf("Calcul de pi par la methode des trapezes\n");
        printf("n              = %d\n", n);
        printf("nb processus   = %d\n", comm_sz);
        printf("integrale      = %.15f\n", total_int);
        printf("pi estime      = %.15f\n", pi_est);
        printf("pi reference   = %.15f\n", pi_ref);
        printf("erreur absolue = %.15e\n", erreur);
        printf("temps          = %.6f s\n", end - start);
        printf("=====================================\n");
    }

    MPI_Finalize();
    return 0;
}



