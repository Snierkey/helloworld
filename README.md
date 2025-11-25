# Hello, World

The phrase "Hello, World" began at Bell Labs in the early 1970s. brian kernighan used it in 1972 in a tutorial for the B language to demonstrate the simplest possible output. the example showed how a program could take a basic instruction and display text, making it a clear starting point for learners.

in 1978, kernighan and dennis ritchie included the same phrase in The C Programming Language. that book spread the example widely because C became influential in system programming. the phrase worked well since it was short, unambiguous, and demonstrated a functioning program without extra concepts.

as new languages were created, the tradition carried over. developers kept using "Hello, World" as the standard first program to confirm that the compiler, interpreter, or environment was set up correctly. the example became a universal baseline for testing and teaching across programming.

### benchmark.c

A C program that benchmarks the execution speed of all other "hello, world" programs in the repository. It compiles languages that require it (C, Crystal, OCaml, Nim, Assembly, Zig) and then measures the execution time of each program, reporting the fastest one.

**To run:**

1.  Compile the benchmark program:
    ```bash
    gcc benchmark.c -o benchmark
    ```
2.  Execute the benchmark:
    ```bash
    ./benchmark
    ```

**Note:** This benchmark requires the respective language runtimes/compilers to be installed and available in your system's PATH (e.g., `bal`, `gcc`, `crystal`, `elixir`, `dotnet`, `node`, `ocaml`, `nim`, `python3`, `ruby`, `sml`, `zig`, `bash`).

## License

This project is licensed under the WTFPL (Do What The Fuck You Want To Public License). See the [LICENSE](LICENSE) file for details.