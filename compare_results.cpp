#include <iostream>
#include <chrono>
#include "stdlib.h"
#include "stdio.h"
#include <cmath>
#include <vector>
#include <fstream>

std::vector<float> halide_implementation_A, halide_implementation_B, halide_implementation_C;
std::vector<float> c_implementation_A, c_implementation_B, c_implementation_C;

void read_matrix(FILE* f, std::vector<float>& matrix) {
    char matrix_name[200];
    int width, height;
    fscanf(f, "%s %d %d\n", matrix_name, &width, &height);
    matrix.resize(width * height);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            fscanf(f, "%f", &matrix[x + y * width]);
        }
        fscanf(f, "\n");
    }
}

void compute_halide_implementation_results(int c_width, int c_height, int a_width, float alpha, float beta, int debug_mode) {
    char command[250];
    sprintf(command, "./build/halide_implementation %d %d %d %f %f %d\n", c_width, c_height, a_width, alpha, beta, debug_mode);
    FILE* pipe = popen(command, "r");
    read_matrix(pipe, halide_implementation_A);
    read_matrix(pipe, halide_implementation_B);
    read_matrix(pipe, halide_implementation_C);
}

void compute_c_implementation_results(int c_width, int c_height, int a_width, float alpha, float beta, int debug_mode) {
    char command[250];
    sprintf(command, "./build/c_implementation %d %d %d %f %f %d\n", c_width, c_height, a_width, alpha, beta, debug_mode);
    FILE* pipe = popen(command, "r");
    read_matrix(pipe, c_implementation_A);
    read_matrix(pipe, c_implementation_B);
    read_matrix(pipe, c_implementation_C);
}

int main(int argc, char* argv[]) {
    std::vector<int> v = { 4, 8, 16, 32, 64, 128, 256, 300, 400, 512, 600, 700, 800, 900, 1000, 1200 };
    if (argc > 1) {
        v.clear();
        for (int i = 1; i < argc; ++i) v.push_back(atoi(argv[i]));
    }
    for (int n : v) {
        compute_halide_implementation_results(n, n, n, 1.0f, 1.0f, 1);
        compute_c_implementation_results(n, n, n, 1.0f, 1.0f, 1);
        bool equal_results = true;
        if (halide_implementation_A.size() != c_implementation_A.size()) equal_results = false;
        if (halide_implementation_B.size() != c_implementation_B.size()) equal_results = false;
        if (halide_implementation_C.size() != c_implementation_C.size()) equal_results = false;
        // halide_implementation_A[i] == c_implementation_A[i] is unsafe some times
        for (int i = 0; (i < halide_implementation_A.size()) && equal_results; ++i) equal_results = (halide_implementation_A[i] - c_implementation_A[i]) == 0.0f;
        for (int i = 0; (i < halide_implementation_B.size()) && equal_results; ++i) equal_results = (halide_implementation_B[i] - c_implementation_B[i]) == 0.0f;
        for (int i = 0; (i < halide_implementation_C.size()) && equal_results; ++i) equal_results = (halide_implementation_C[i] - c_implementation_C[i]) == 0.0f;
        if (equal_results) {
            std::cout << "Success! Identical results between Halide and C implementations for n=" << n << std::endl;
        } else {
            std::cout << "Failed! Different results between Halide and C implementations for n=" << n << std::endl;
        }
    }
}