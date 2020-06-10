#include "stdio.h"
#include "stdlib.h"
#include "assert.h"
#include "time.h"

// row-major ordering
#define GET(m, x, y) ( (m)->data[(x) + (y) * (m)->width] )

struct matrix_t {
    float* data;
    int width;
    int height;
} matrix_t;

void fill_matrix(struct matrix_t* m, int width, int height) {
    m->data = (float*) malloc(width * height * sizeof(float));
    m->width = width;
    m->height = height;
    // Fill with "i % 40 - 20" in order to avoid overflows for big matrices
    for (int i = 0; i < m->width * m->height; ++i) m->data[i] = i % 40 - 20;
}

void free_matrix(struct matrix_t* m) {
    free(m->data);
    m->width = -1;
    m->height = -1;
}

void print_matrix(struct matrix_t* m) {
    for (int y = 0; y < m->height; ++y) {
        for (int x = 0; x < m->width; ++x) {
            printf("%f ", GET(m, x, y));
        }
        printf("\n");
    }
}

void general_matrix_multiply(struct matrix_t* A, struct matrix_t* B, struct matrix_t* C, float alpha, float beta) {
    assert(A->width == B->height && A->height == C->height && B->width == C->width);
    for (int y = 0; y < C->height; ++y) {
        for (int x = 0; x < C->width; ++x) {
            GET(C, x, y) = GET(C, x, y) * beta;
            for (int r = 0; r < A->width; ++r) {
                GET(C, x, y) += GET(A, r, y) * GET(B, x, r) * alpha;
            }
        }
    }
}

// This optimization relies on the memory access pattters and tries to reduce cache misses by transposing the B matrix
void general_matrix_multiply_transpose_optimization(struct matrix_t* A, struct matrix_t* B, struct matrix_t* C, float alpha, float beta) {
    assert(A->width == B->height && A->height == C->height && B->width == C->width);
    
    struct matrix_t B_transposed;
    B_transposed.data = (float*) malloc(B->width * B->height * sizeof(float));
    B_transposed.width = B->height;
    B_transposed.width = B->width;
    for (int y = 0; y < B_transposed.height; ++y) {
        for (int x = 0; x < B_transposed.width; ++x) {
            GET(&B_transposed, x, y) = GET(B, y, x);
        }
    }

    for (int y = 0; y < C->height; ++y) {
        for (int x = 0; x < C->width; ++x) {
            GET(C, x, y) = GET(C, x, y) * beta;
            for (int r = 0; r < A->width; ++r) {
                GET(C, x, y) += GET(A, r, y) * GET(&B_transposed, r, x) * alpha;
            }
        }
    }

    free(B_transposed.data);
}

int main(int argc, char* argv[]) {
    // By default runs the multiplication between 
    // A = |-20 -19| and B =|-20 -19 -18| which results in C = |723 684 645|
    //     |-18 -17|        |-17 -16 -15|                      |649 614 579|
    //     |-16 -15|                                           |575 544 513|
    // and prints the 3 matrices
    int c_width = 3, c_height = 3, a_width = 2;
    float alpha = 1.0f, beta = 0.0f;
    // 1 for printing the matrices after computation
    // 2 for printing the time taken by the multiplication
    int debug_mode = 1;
    if (argc >= 6) {
        c_width = atoi(argv[1]);
        c_height = atoi(argv[2]);
        a_width = atoi(argv[3]);
        alpha = atof(argv[4]);
        beta = atof(argv[5]);
    }
    if (argc >= 7) {
        debug_mode = atoi(argv[6]);
        if (debug_mode != 1 && debug_mode != 2) debug_mode = 1;
    }

    struct matrix_t A, B, C;
    fill_matrix(&A, a_width, c_height);
    fill_matrix(&B, c_width, a_width);
    fill_matrix(&C, c_width, c_height);

    clock_t unoptimized_start = clock();
    general_matrix_multiply(&A, &B, &C, alpha, beta);
    clock_t unoptimized_end = clock();

    clock_t optimized_start = clock();
    if (debug_mode == 2) {
        general_matrix_multiply_transpose_optimization(&A, &B, &C, alpha, beta);
    }
    clock_t optimized_end = clock();


    if (debug_mode == 1) {
        printf("A %d %d\n", A.width, A.height);
        print_matrix(&A);
        printf("B %d %d\n", B.width, B.height);
        print_matrix(&B);
        printf("C %d %d\n", C.width, C.height);
        print_matrix(&C);
    } else if (debug_mode == 2) {
        printf(" C implementtion general matrix multiply duration : %f seconds\n", (double)(unoptimized_end - unoptimized_start) / CLOCKS_PER_SEC);
        printf(" Optimized C implementtion general matrix multiply duration : %f seconds\n", (double)(optimized_end - optimized_start) / CLOCKS_PER_SEC);
    }

    free_matrix(&A);
    free_matrix(&B);
    free_matrix(&C);
}