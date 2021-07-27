#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <vector>
#include <cstring>
#include <fstream>

#define MINISH_VERSION "0.0.2"
#define MAX_DIR_NAME 126

int minish_version() {
    printf("minish, version: %s\n", MINISH_VERSION);
    return 0;
}

int minish_help() {
    printf("minish is incomplete shell.\n\t-h: help\n\t-v: version\n\t-c: exec command\n");
    return 0;
}

void minish_chdir(std::vector<char *> args) {
  if (args[1] == NULL) {
    printf("error: argument missing\n");
  } else {
    chdir(args[1]);
  }
}

void minish_exec(std::vector<char *> args) {
    pid_t pid;
    int status;

    pid = fork();

    if (pid == 0) {
        execvp(args[0], &args[0]);
        exit(1);
    } else if (pid < 0) {
        printf("error: fork failed");
    } else {
        waitpid(pid, &status, WUNTRACED);
        while (!WIFEXITED(status) && !WIFSIGNALED(status)) {
            waitpid(pid, &status, WUNTRACED);
        }
    }
}

void minish_find(std::vector<char *> args) {
    if (args.size() <= 1) {
        return;
    } else if (strcmp(args[0],"exit") == 0) {
        exit(0);
    } else if (strcmp(args[0],"cd") == 0) {
        minish_chdir(args);
    } else if (strcmp(args[0],"help") == 0) {
        minish_help();
    } else if (strcmp(args[0],"version") == 0) {
        minish_version();
    } else if (strcmp(args[0],"exec") == 0) {
        args.erase(args.begin());
        minish_exec(args);
    } else {
        minish_exec(args);
    }
}

void minish_free(std::vector<char *> args) {
    for (int i = 0; i < args.size(); i++) {
        delete[] args[i];
    }
}

std::vector<char *> input2args(std::string input) {
    std::vector<char *> args;
    
    std::string arg;
    std::stringstream input_s(input);
    while(std::getline(input_s, arg, ' ')) {
        char* carg = new char[arg.size() + 1];
        std::strcpy(carg, arg.c_str());
        args.push_back(carg);
    }
    args.push_back(NULL);
    
    return args;
}

std::string minish_input(char dirname[MAX_DIR_NAME]) {
    std::string input;

    getcwd(dirname, MAX_DIR_NAME);
    printf("%s\n$ ",dirname);
    std::getline(std::cin, input);

    return input;
}

void minish_interpreter(std::string line) {
    std::vector<char *> args = input2args(line);
    minish_find(args);
    minish_free(args);
    std::vector<char *>().swap(args);
}

int minish_file(char *file) {
    std::ifstream file_s(file);
    std::string line;
    std::vector<char *> args;
    if (file_s.fail()) {
        printf("error: invalid argument '%s'\n", file);
        return 1;
    } else {
        while (std::getline(file_s, line)) {
            minish_interpreter(line);
        }
        return 0;
    }
}

int minish() {
    char curdir[MAX_DIR_NAME];
    while (1) { minish_interpreter(minish_input(curdir)); }
    return 0;
}

int main(int argc, char **argv) {
    if (argc == 1) {
        return minish();
    } else if (argv[1][0] == '-') {
        if (strcmp(argv[1],"-h") == 0) {
            return minish_help();
        } else if (strcmp(argv[1],"-v") == 0) {
            return minish_version();
        } else if (strcmp(argv[1],"-c") == 0) {
            if (argv[2] == NULL) {
                printf("error: argument missing\n");
                return 1;
            }
            std::string code = argv[2];
            minish_interpreter(code);
            return 0;
        } else {
            printf("error: invalid option '%s'\n", argv[1]);
            return 1;
        }
    } else {
        return minish_file(argv[1]);
    }
}