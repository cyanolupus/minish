#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <vector>
#include <cstring>
#include <fstream>

#define MINISH_VERSION "0.1.0"
#define MAX_DIR_NAME 126

class Minish {

private:
    std::vector<char *> _args;
    char curdir[MAX_DIR_NAME];

    void minishExec(void) {
        pid_t pid;
        int status;

        pid = fork();

        if (pid == 0) {
            if (execvp(_args[0], &_args[0]) == -1) {
                printf("error: Unknown command \'%s\'\n", _args[0]);
            }
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

    void minishChdir(void) {
        if (_args[1] == NULL) {
            printf("error: argument missing\n");
        } else {
            chdir(_args[1]);
        }
    }

    void string2Args(std::string string) {
        std::string arg;
        std::stringstream input_s(string);
        while(std::getline(input_s, arg, ' ')) {
            char* carg = new char[arg.size() + 1];
            std::strcpy(carg, arg.c_str());
            _args.push_back(carg);
        }
        _args.push_back(NULL);
    }

    void findArgs() {
        if (_args.size() <= 1) {
            return;
        } else if (strcmp(_args[0],"exit") == 0) {
            exit(0);
        } else if (strcmp(_args[0],"cd") == 0) {
            minishChdir();
        } else if (strcmp(_args[0],"help") == 0) {
            minishHelp();
        } else if (strcmp(_args[0],"version") == 0) {
            minishVersion();
        } else if (strcmp(_args[0],"exec") == 0) {
            if (execvp(_args[1], &_args[1]) == -1) {
                printf("error: Unknown command \'%s\'\n", _args[0]);
            }
            exit(1);
        } else {
            minishExec();
        }
    }

public:
    Minish() {
        std::string input;

        getcwd(curdir, MAX_DIR_NAME);
        printf("%s\n$ ",curdir);
        std::getline(std::cin, input);

        string2Args(input);
    }

    Minish(std::string string) {
        string2Args(string);
    }

    int minishVersion() {
        printf("minish, version: %s\n", MINISH_VERSION);
        return 0;
    }

    int minishHelp(void) {
        printf("minish is incomplete shell.\n\t-h: help\n\t-v: version\n\t-c: exec command\n");
        return 0;
    }

    ~Minish() {
        for (int i = 0; i < _args.size(); i++) delete[] _args[i];
        std::vector<char *>().swap(_args);
    }

    std::vector<char *> getArgs(void) {
        return _args;
    }

    void setArgs(std::vector<char *> Args) {
        _args = Args;
    }

    void interpreter() {
        findArgs();
        std::vector<char *>().swap(_args);
    }
};

int minish_file(char *file) {
    std::ifstream file_s(file);
    std::string line;
    std::vector<char *> args;
    if (file_s.fail()) {
        printf("error: invalid argument '%s'\n", file);
        return 1;
    } else {
        while (std::getline(file_s, line)) {
            Minish minish(line);
            minish.interpreter(); 
        }
        return 0;
    }
}

int minish() {
    while (1) { 
        Minish minish;
        minish.interpreter();
    }
    return 0;
}

int main(int argc, char **argv) {
    if (argc == 1) {
        return minish();
    } else if (argv[1][0] == '-') {
        if (strcmp(argv[1],"-h") == 0) {
            return 0;
        } else if (strcmp(argv[1],"-v") == 0) {
            return 0;
        } else if (strcmp(argv[1],"-c") == 0) {
            if (argv[2] == NULL) {
                printf("error: argument missing\n");
                return 1;
            }
            Minish minish(argv[2]);
            minish.interpreter();
            return 0;
        } else {
            printf("error: invalid option '%s'\n", argv[1]);
            return 1;
        }
    } else {
        return minish_file(argv[1]);
    }
}