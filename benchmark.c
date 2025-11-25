#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#define MAX_COMMAND_LEN 256
#define MAX_ARGS 32

typedef struct {
    const char* name;
    const char* compile_cmd;
    const char* run_cmd_template;
    char actual_run_cmd[MAX_COMMAND_LEN];
    double duration;
} ProgramInfo;

static void parse_command(char* cmd_str, char* argv[]) {
    char* token;
    int i = 0;
    token = strtok(cmd_str, " ");
    while (token != NULL && i < MAX_ARGS - 1) {
        argv[i++] = token;
        token = strtok(NULL, " ");
    }
    argv[i] = NULL;
}

double measure_execution_time(const char* command) {
    struct timespec start, end;
    pid_t pid;
    int status;

    clock_gettime(CLOCK_MONOTONIC, &start);

    pid = fork();
    if (pid == -1) {
        perror("fork failed");
        return -1.0;
    } else if (pid == 0) {
        FILE* dev_null = fopen("/dev/null", "w");
        if (dev_null == NULL) {
            perror("Failed to open /dev/null in child");
            _exit(1);
        }
        if (dup2(fileno(dev_null), STDOUT_FILENO) == -1) {
            perror("Failed to dup2 stdout in child");
            fclose(dev_null);
            _exit(1);
        }
        fclose(dev_null);

        char cmd_copy[MAX_COMMAND_LEN];
        strncpy(cmd_copy, command, MAX_COMMAND_LEN - 1);
        cmd_copy[MAX_COMMAND_LEN - 1] = '\0';

        char* argv[MAX_ARGS];
        parse_command(cmd_copy, argv);

        execvp(argv[0], argv);
        perror("execvp failed");
        _exit(127);
    } else {
        waitpid(pid, &status, 0);
        clock_gettime(CLOCK_MONOTONIC, &end);

        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            fprintf(stderr, "Command '%s' exited with status %d\n", command, WEXITSTATUS(status));
            return -1.0;
        }
        if (WIFSIGNALED(status)) {
            fprintf(stderr, "Command '%s' terminated by signal %d\n", command, WTERMSIG(status));
            return -1.0;
        }

        return (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1e9;
    }
}

int main() {
    ProgramInfo programs[] = {
        {"hello.bal", NULL, "bal run hello.bal"},
        {"hello.c", "gcc hello.c -o hello.c.out", "./hello.c.out"},
        {"hello.cr", "crystal build hello.cr -o hello.cr.out", "./hello.cr.out"},
        {"hello.english", NULL, "cat hello.english"},
        {"hello.ex", NULL, "elixir hello.ex"},
        {"hello.fs", NULL, "dotnet fsi hello.fs"},
        {"hello.js", NULL, "node hello.js"},
        {"hello.miaw", NULL, "cat hello.miaw"},
        {"hello.ml", "ocamlopt hello.ml -o hello.ml.out", "./hello.ml.out"},
        {"hello.nim", "nim c hello.nim", "./hello"},
        {"hello.py", NULL, "python3 hello.py"},
        {"hello.rb", NULL, "ruby hello.rb"},
        {"hello.s", "gcc hello.s -o hello.s.out", "./hello.s.out"},
        {"hello.sh", NULL, "bash hello.sh"},
        {"hello.sml", NULL, "sh -c \"sml < hello.sml\""},
        {"hello.zig", "zig build-exe hello.zig -o hello.zig.out", "./hello.zig.out"},
        {"hellohc.c", "gcc hellohc.c -o hellohc.c.out", "./hellohc.c.out"},
        {"hellohc.cr", "crystal build hellohc.cr -o hellohc.cr.out", "./hellohc.cr.out"},
        {"hellohc.py", NULL, "python3 hellohc.py"},
        {NULL, NULL, NULL, "", 0.0}
    };

    int num_programs = sizeof(programs) / sizeof(ProgramInfo) - 1;

    printf("--- Compiling programs ---\n");
    for (int i = 0; i < num_programs; ++i) {
        if (programs[i].compile_cmd != NULL) {
            printf("Compiling %s...\n", programs[i].name);
            int ret = system(programs[i].compile_cmd);
            if (ret != 0) {
                fprintf(stderr, "Error compiling %s (command: '%s'). Skipping.\n", programs[i].name, programs[i].compile_cmd);
                programs[i].duration = -1.0;
                continue;
            }
            strncpy(programs[i].actual_run_cmd, programs[i].run_cmd_template, MAX_COMMAND_LEN - 1);
            programs[i].actual_run_cmd[MAX_COMMAND_LEN - 1] = '\0';
        } else {
            strncpy(programs[i].actual_run_cmd, programs[i].run_cmd_template, MAX_COMMAND_LEN - 1);
            programs[i].actual_run_cmd[MAX_COMMAND_LEN - 1] = '\0';
        }
    }

    printf("\n--- Benchmarking programs (output redirected to /dev/null) ---\n");
    for (int i = 0; i < num_programs; ++i) {
        if (programs[i].duration == -1.0) {
            printf("Skipping %s (compilation failed).\n", programs[i].name);
            continue;
        }
        printf("Running %s...\n", programs[i].name);
        programs[i].duration = measure_execution_time(programs[i].actual_run_cmd);
        if (programs[i].duration < 0) {
            fprintf(stderr, "Error running %s (command: '%s'). Marking as failed.\n", programs[i].name, programs[i].actual_run_cmd);
        }
    }

    printf("\n--- Results ---\n");
    double fastest_time = -1.0;
    const char* fastest_program = NULL;

    for (int i = 0; i < num_programs; ++i) {
        if (programs[i].duration >= 0) {
            printf("%-20s: %.6f seconds\n", programs[i].name, programs[i].duration);
            if (fastest_program == NULL || programs[i].duration < fastest_time) {
                fastest_time = programs[i].duration;
                fastest_program = programs[i].name;
            }
        } else {
            printf("%-20s: Failed or Skipped\n", programs[i].name);
        }
    }

    printf("\n--------------------------------\n");
    if (fastest_program != NULL) {
        printf("The fastest program is: %s (%.6f seconds)\n", fastest_program, fastest_time);
    } else {
        printf("No successful runs to determine the fastest program.\n");
    }
    printf("--------------------------------\n");

    printf("\n--- Cleaning up compiled binaries ---\n");
    for (int i = 0; i < num_programs; ++i) {
        if (programs[i].compile_cmd != NULL) {
            char rm_cmd[MAX_COMMAND_LEN];
            if (strstr(programs[i].name, ".nim") != NULL) {
                char base_name[MAX_COMMAND_LEN];
                strncpy(base_name, programs[i].name, sizeof(base_name) - 1);
                base_name[sizeof(base_name) - 1] = '\0';
                char* dot = strrchr(base_name, '.');
                if (dot) *dot = '\0';
                snprintf(rm_cmd, MAX_COMMAND_LEN, "rm -f %s", base_name);
            } else {
                // for other compiled languages, the actual_run_cmd is the path to the binary,
                // which usually starts with "./". i remove the "./" prefix for the rm command
                snprintf(rm_cmd, MAX_COMMAND_LEN, "rm -f %s", programs[i].actual_run_cmd + 2);
            }
            printf("Cleaning up: %s\n", rm_cmd);
            system(rm_cmd);
        }
    }

    return 0;
}