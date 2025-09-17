#include <stdio.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <err.h>

#define SPACES " \t\n\v\f\r"
#define ERROR(msg) fprintf(stderr, "Erro critico -->" msg "\n")
#define p(...) printf(__VA_ARGS__)

//executor-------------------------------------------------------------------
int executor(char **args){
    pid_t pid;
    int status;

    if(args[0]==NULL){ 
        return 1; 
    }else 
        if (strcmp(args[0], "exit") == 0) {
            return 0; 
    }else 
        if (strcmp(args[0], "clear") == 0) { 
            system("clear"); 
    }else 
        if(strcmp(args[0], "cd")==0){
            char *caminho = args[1];
            if(caminho== NULL){
                caminho = getenv("HOME");
                if(caminho == NULL){
                    ERROR("HOME não definida");
                }else{
                    if (chdir(caminho) != 0) {
                        perror("ERRO 1 -->");

                    }
                }
            }else {
                if (chdir(caminho) != 0) {
                    perror("ERRO 2 -->");
                }
            }
            return 1;
        }
    pid = fork();
    if( pid < 0){
        perror("ERRO 4 -->");
    }else
        if(pid == 0){
            if (execvp(args[0],args)== -1){
                perror("ERRO 3 -->");
                exit(EXIT_FAILURE);
            }    
        }else{   
            do{
                waitpid(pid, &status, WUNTRACED);
            }while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }
    return 1;
}
//leitor dos comandos--------------------------------------------------------
char* ler_comando(void){
    char buffer [1024];
   
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {

        buffer[strcspn(buffer, "\n")] = 0;

        char *comando = strdup(buffer); 

        return comando;
    }
    return NULL;
}
//identificador dos comandos-------------------------------------------------
char** identificador_d_comando(char *comandos){
    int buffer = 64;
    int position = 0;

    char **tokens = malloc(buffer * sizeof(char*));
    

    if(!tokens){
        ERROR("alocacao de memoria tokens");
        exit(EXIT_FAILURE);
    }
    char *token = strtok(comandos,SPACES);

    while (token != NULL){
        tokens[position]=token;
        position++;

        token = strtok(NULL, SPACES);

    }
    tokens[position]=NULL;
    return tokens;
}
//loop principal-------------------------------------------------------------
void loop_principal(void){

    char *comando;
    char **args;
    char statusshell;

    do{
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s |Otaldoshell|> ", cwd);
        } else {
            printf("|Otaldoshell|> ");
        }
    
        comando = ler_comando(); 
        args = identificador_d_comando(comando);
        statusshell = executor(args);
        
        free(comando);
        free(args);

    }while (statusshell);
}
//MAIN-----------------------------------------------------------------------
int main(int argc, char **argv){
   
   loop_principal();
   
return EXIT_SUCCESS;
}
//---------------------------------------------------------------------------