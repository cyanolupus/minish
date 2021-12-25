#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <vector>
#include <cstring>
#include <fstream>

#define MINISH_VERSION "0.2.1"
#define MAX_DIR_NAME 126

class Args {
private:
    std::vector<char *> _args;

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

public:
    Args(std::string string) {
        std::string arg;
        std::stringstream input_s(string);
        while(std::getline(input_s, arg, ' ')) {
            char* carg = new char[arg.size() + 1];
            std::strcpy(carg, arg.c_str());
            _args.push_back(carg);
        }
        _args.push_back(NULL);
    }

    ~Args() {
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
        if (_args.size() <= 1) {
            return;
        } else if (strcmp(_args[0],"exit") == 0) {
            exit(0);
        } else if (strcmp(_args[0],"cd") == 0) {
            minishChdir();
        } else if (strcmp(_args[0],"exec") == 0) {
            if (execvp(_args[1], &_args[1]) == -1) {
                printf("error: Unknown command \'%s\'\n", _args[0]);
                exit(1);
            }
        } else {
            minishExec();
        }
    }
};

class Minish {
private:
    static Minish* _minish;
    char curdir[MAX_DIR_NAME];

    Minish() {
        
    }
    
    void minishVersion(void) {
        printf("minish, version: %s\n", MINISH_VERSION);
    }

    void minishHelp(void) {
        printf("minish is incomplete shell.\n\t-h: help\n\t-v: version\n\t-c: exec command\n");
    }

public:
    static Minish* getInstance() {
        if (_minish == 0) {
            _minish = new Minish;
        }
        return _minish;
    }

    void start(void) {
        std::string input;
        printf("welcome to minish\n");
        while (1) {
            getcwd(curdir, MAX_DIR_NAME);
            printf("%s\n$ ",curdir);
            std::getline(std::cin, input);
            Args args(input);
            args.interpreter();
        }
    }

    void start(char **option) {
        if (strcmp(option[1],"-h") == 0) {
            minishHelp();
        } else if (strcmp(option[1],"-v") == 0) {
            minishVersion();
        } else if (strcmp(option[1],"-c") == 0) {
            if (option[2] == NULL) {
                printf("error: argument missing\n");
                exit(1);
            }
            Args args(option[2]);
            args.interpreter();
        } else {
            printf("error: invalid option '%s'\n", option[1]);
            exit(1);
        }
    }

    void start(char *file) {
        std::ifstream file_s(file);
        std::string line;
        std::vector<char *> args;
        if (file_s.fail()) {
            printf("error: invalid argument '%s'\n", file);
            exit(1);
        } else {
            while (std::getline(file_s, line)) {
                Args args(line);
                args.interpreter(); 
            }
        }
    }
};

Minish* Minish::_minish = 0;

int main(int argc, char **argv) {
    Minish* minish = Minish::getInstance();
    if (argc == 1) {
        minish->start();
    } else if (argv[1][0] == '-') {
        minish->start(argv);
    } else {
        minish->start(argv[1]);
    }
}