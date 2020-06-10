#include <iostream>
#include "Halide.h"
#include "time.h"

using namespace Halide;

template<typename T>
void print_halide_matrix(const Buffer<T>& input) {
    for (int y = 0; y < input.height(); ++y) {
        for (int x = 0; x < input.width(); ++x) {
            std::cout << input(x, y) << " ";
        }
        std::cout << std::endl;
    }
}


// fills the the matrix with numbers
template<typename T>
void fill_halide_matrix(Buffer<T>& m) {
    Var x, y;
    Func fill_func;
    // Do "(x + y * m.width()) % 40 - 20" in order to avoid overflows for big matrices
    fill_func(x, y) = Halide::cast<float>( (x + y * m.width()) % 40 - 20 );
    fill_func.parallel(x);
    m = fill_func.realize(m.width(), m.height());
}


double gemm_duration = 0.0;
// Performs C <- alpha * A * B + beta * C;
template<typename T>
void general_matrix_multiply(const Buffer<T>& A, const Buffer<T>& B, Buffer<T>& C, T alpha, T beta) {
    assert(A.width() == B.height() && A.height() == C.height() && B.width() == C.width());

    Var x("x"), y("y");
    Func matrix_prduct("matrix_prduct"), add_matrices("add_matrices");
    RDom r(0, A.width(), "r");
    matrix_prduct(x, y) = 0.f;
    matrix_prduct(x, y) += A(r, y) * B(x, r) * alpha;
    add_matrices(x, y) = matrix_prduct(x, y) + C(x, y) * beta;
    add_matrices.parallel(y);
    // add_matrices.print_loop_nest();

    // We measure duration here because the compilation of Halide code take much longer for most use cases and 
    // the compilation time of Halide code can be amortized by reusing the same compiled pipeline multiple times
    clock_t start = clock();
    C = add_matrices.realize(C.width(), C.height());
    clock_t end = clock();
    gemm_duration = (double)(end - start) / CLOCKS_PER_SEC;
}

int main(int argc, char* argv[]) {
    // By default runs the multiplication between 
    // A = |-20 -19| and B =|-20 -19 -18| which results in C = |723 684 645|
    //     |-18 -17|        |-17 -16 -15|                      |649 614 579|
    //     |-16 -15|                                           |575 544 513|
    // and prints the 3 matrices
    int c_width = 3, c_height = 3, a_width = 2;
    float alpha = 1.0f, beta = 0.0f;
    int debug_mode = 1;
    // 1 for printing the matrices after computation
    // 2 for printing the time taken by the multiplication
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

    Buffer<float> A(a_width, c_height), B(c_width, a_width), C(c_width, c_height);
    fill_halide_matrix(A);
    fill_halide_matrix(B);
    fill_halide_matrix(C);

    general_matrix_multiply(A, B, C, alpha, beta);

    if (debug_mode == 1) {
        std::cout << "A " << A.width() << " " << A.height() << std::endl;
        print_halide_matrix(A);
        std::cout << "B " << B.width() << " " << B.height() << std::endl;
        print_halide_matrix(B);
        std::cout << "C " << C.width() << " " << C.height() << std::endl;
        print_halide_matrix(C);
    } else if (debug_mode == 2) {
        std::cout << " Halide implementtion general matrix multiply duration : " << gemm_duration << " seconds" << std::endl;
    }
}
