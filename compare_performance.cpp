#include <iostream>
#include "stdlib.h"
#include "stdio.h"
#include <cmath>
#include <vector>

void run_halide_implementation(int c_width, int c_height, int a_width, float alpha, float beta, int debug) {
    char command[250];
    sprintf(command, "./build/halide_implementation %d %d %d %f %f %d\n", c_width, c_height, a_width, alpha, beta, debug);
    system(command);
}

void run_c_implementation(int c_width, int c_height, int a_width, float alpha, float beta, int debug) {
    char command[250];
    sprintf(command, "./build/c_implementation %d %d %d %f %f %d\n", c_width, c_height, a_width, alpha, beta, debug);
    system(command);
}

int main(int argc, char* argv[]) {
    std::vector<int> v = { 4, 8, 16, 32, 64, 128, 256, 300, 400, 512, 600, 700, 800, 900, 1000, 1200 };
    if (argc > 1) {
        v.clear();
        for (int i = 1; i < argc; ++i) v.push_back(atoi(argv[i]));
    }
    for (int n : v) {
        std::cout << " N = " << n << std::endl;
        run_c_implementation(n, n, n, 1.0f, 1.0f, 2);
        run_halide_implementation(n, n, n, 1.0f, 1.0f, 2);
        std::cout << "======================================================" << std::endl;
    }
}