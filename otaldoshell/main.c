#include "cfig.h"

struct Command* parse_line(char *line);
int execute_commands(struct Command *cmd);
void free_commands(struct Command *cmd);

void handle_sigchld(int sig) {
    while (waitpid(-1, NULL, WNOHANG) > 0) { }
}

void free_commands(struct Command *cmd) {
    struct Command *temp;
    while (cmd != NULL) {
        temp = cmd;
        cmd = cmd->next;
        free(temp->args);
        free(temp);
    }
}

int execute_commands(struct Command *cmd) {
    if (cmd == NULL || cmd->args[0] == NULL) {
        return 1;
    }

    if (strcmp(cmd->args[0], "exit") == 0) {
        return 0;
    }

    if (strcmp(cmd->args[0], "clear") == 0) {
        system("clear");
        return 1;
    }

    if (strcmp(cmd->args[0], "cd") == 0) {
        if (cmd->next != NULL) {
            fprintf(stderr, "Erro: 'cd' não pode ser usado em um pipe.\n");
            return 1;
        }
        char *caminho = cmd->args[1];
        if (caminho == NULL) {
            caminho = getenv("HOME");
            if (caminho == NULL) {
                ERROR("HOME não definida");
            }
        }
        if (caminho != NULL && chdir(caminho) != 0) {
            P_ERROR("cd");
        }
        return 1;
    }

    int status = 1;
    pid_t pid;
    int fd_in = 0;
    int pipefd[2];

    struct Command *current = cmd;

    while (current != NULL) {
        if (current->next != NULL) {
            if (pipe(pipefd) < 0) {
                P_ERROR("pipe");
                exit(EXIT_FAILURE);
            }
        }

        pid = fork();
        if (pid < 0) {
            P_ERROR("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            if (fd_in != 0) {
                dup2(fd_in, 0);
                close(fd_in);
            } else if (current->inputFile != NULL) {
                int fd = open(current->inputFile, O_RDONLY);
                if (fd < 0) { P_ERROR("open input file"); exit(EXIT_FAILURE); }
                dup2(fd, 0);
                close(fd);
            }

            if (current->next != NULL) {
                dup2(pipefd[1], 1);
                close(pipefd[0]);
                close(pipefd[1]);
            } else if (current->outputFile != NULL) {
                int flags = O_WRONLY | O_CREAT;
                if (current->append) flags |= O_APPEND;
                else flags |= O_TRUNC;
                int fd = open(current->outputFile, flags, 0644);
                if (fd < 0) { P_ERROR("open output file"); exit(EXIT_FAILURE); }
                dup2(fd, 1);
                close(fd);
            }

            if (execvp(current->args[0], current->args) == -1) {
                perror(current->args[0]);
                exit(EXIT_FAILURE);
            }
        } else {
            if (current->next != NULL) {
                close(pipefd[1]);
                if (fd_in != 0) close(fd_in);
                fd_in = pipefd[0];
            } else {
                if (fd_in != 0) close(fd_in);
                if (!current->background) {
                    do {
                        waitpid(pid, &status, WUNTRACED);
                    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
                } else {
                    printf("[%d] %s\n", pid, current->args[0]);
                }
            }
        }

        current = current->next;
    }

    return 1;
}

char *ler_comando(void) {
    char buffer[MAX_LINE_BUFFER];

    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = 0;
        char *comando = strdup(buffer);
        return comando;
    }

    if (feof(stdin)) {
        printf("\n");
        return NULL;
    }
    return NULL;
}

struct Command* parse_line(char *line) {
    struct Command *head = NULL, *current = NULL, *prev = NULL;
    char *pipe_saveptr;

    int bg = 0;
    char *bg_char = strrchr(line, '&');
    if (bg_char != NULL && *(bg_char+1) == '\0') {
        *bg_char = '\0';
        bg = 1;
    }

    char *pipe_token = strtok_r(line, "|", &pipe_saveptr);

    while (pipe_token != NULL) {
        struct Command *cmd = calloc(1, sizeof(struct Command));
        if (!cmd) {
            ERROR("alocacao de memoria cmd");
            exit(EXIT_FAILURE);
        }
        cmd->args = malloc(MAX_ARGS_BUFFER * sizeof(char *));
        if (!cmd->args) {
            ERROR("alocacao de memoria args");
            exit(EXIT_FAILURE);
        }

        if (head == NULL) head = cmd;
        else prev->next = cmd;
        prev = cmd;

        char *arg_saveptr;
        char *arg_token = strtok_r(pipe_token, SPACES, &arg_saveptr);
        int arg_pos = 0;

        while (arg_token != NULL) {
            if (strcmp(arg_token, "<") == 0) {
                cmd->inputFile = strtok_r(NULL, SPACES, &arg_saveptr);
            } else if (strcmp(arg_token, ">") == 0) {
                cmd->outputFile = strtok_r(NULL, SPACES, &arg_saveptr);
                cmd->append = 0;
            } else if (strcmp(arg_token, ">>") == 0) {
                cmd->outputFile = strtok_r(NULL, SPACES, &arg_saveptr);
                cmd->append = 1;
            } else {
                cmd->args[arg_pos++] = arg_token;
            }
            arg_token = strtok_r(NULL, SPACES, &arg_saveptr);
        }
        cmd->args[arg_pos] = NULL;

        pipe_token = strtok_r(NULL, "|", &pipe_saveptr);
    }

    if (head != NULL) prev->background = bg;
    return head;
}

void loop_principal(void) {
    char *comando;
    struct Command *cmd_list;
    int statusshell;

    do {
        char cwd[MAX_PATH_BUFFER];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s |Otaldoshell|> ", cwd);
        } else {
            printf("|Otaldoshell|> ");
        }
        fflush(stdout);

        comando = ler_comando();

        if (comando == NULL) {
            statusshell = 0;
        } else {
            if (*comando) {
                cmd_list = parse_line(comando);
                statusshell = execute_commands(cmd_list);
                free_commands(cmd_list);
            }
            free(comando);
        }

    } while (statusshell);
}

int main(int argc, char **argv) {
    if (signal(SIGCHLD, handle_sigchld) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    loop_principal();
    return EXIT_SUCCESS;
}