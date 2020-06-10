# General matrix multiply Halide
## How to run :
- Acquire LLVM and Halide :
    - First download pre built binaries for your system from [llvm download](https://releases.llvm.org/download.html#10.0.0)
    - Run the following commands to clone and build Halide (make sure to specify LLVM_DIR correctly)
        - `git clone https://github.com/halide/Halide` 
        - `cd Halide`
        - `mkdir build`
        - `cd build`
        - `cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DLLVM_DIR="/path/to/llvm/lib/cmake/llvm" ..`
        - `make install`
    - This will propably install Halide on your system and cmake will detect it automatically
    - If you already have Halide installed you don't need to do these steps.
- How to build the executables :
    - Clone this repository.
    - Position your self at the root of the source tree (where this readme file is).
    - Run the following commands :
        - `mkdir build`
        - `cd build`
        - `cmake -G "Unix Makefiles" ..`
        - `make all`
- If you have Halide and llvm installed and detected properly by cmake you can use vscode and cmake tools extension you can open the project, build and run everything peacefully.
- How to run the executables :
    - Position yourself at the the root of the source tree (where this readme file is).
    - I provided 4 executables:
        - One that runs the C implementation of general matrix multiply (c_implementation). It also contains 2 versions one with no optimizations and the other with matrix transpose optimization (You can run it and compare them with the compare_results executable).
        - One that runs the Halide implementation of general matrix multiply (halide_implementation).
        - One that compares the results of the 2 last executables (compare_results).
        - One that prints out the performance of the 2 last executables (compare_performance).
    - To run the compare_results and compare_performance you need to be at the root of the source tree and execute one of the two commands :
        - `./build/compare_results`
        - `./build/compare_performance`
    - You can also specify the size of the matrices to test as CLI arguments:
        - `./build/compare_results 100 200 300`
        - `./build/compare_performance 100 200 300`
    - c_implementation and halide_implementation can take the following CLI arguments in order `c_width c_height a_width alpha beta debug_mode`
        - c_width, c_height, a_width are the sizes of the matrices generated and used to perform the multiplication. As an example c_width=3, c_height and a_width=2 for you'll get the following A, B and C matrices
        <pre>
            A = |-20 -19| B =|-20 -19 -18|, C = |723 684 645|
                |-18 -17|    |-17 -16 -15|      |649 614 579|
                |-16 -15|                       |575 544 513|
        </pre>
        - alpha and beta are the coefficient in the general matrix multiply formula `C <- alpha * A * B + beta * C`
        - debug_mode is a paremeter that defaults to 1 when unused and can be either 1 or 2
            - 1 tells the executable to print the matrices at the end of execution.
            - 2 tells the executable to print the time taken by the matrix multiply operation.
        - If you don't specify any CLI arguments you'll get the following default values:
            c_width=3 c_height=3 a_width=2 alpha=1.0f beta=0.0f debug_mode=1