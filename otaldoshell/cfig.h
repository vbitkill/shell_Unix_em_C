#ifndef cfig_h
#define cfig_h

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

#define SPACES " \t\n\v\f\r"
#define ERROR(msg) fprintf(stderr, "Erro critico --> %s\n", (msg))
#define P_ERROR(msg) perror(msg)
#define p(...) printf(__VA_ARGS__)

#define MAX_LINE_BUFFER 1024
#define MAX_ARGS_BUFFER 64
#define MAX_PATH_BUFFER 256

struct Command {
    char **args;
    char *inputFile;
    char *outputFile;
    int append;
    int background;
    struct Command *next;
};

#endif