# Otaldoshell

Otaldoshell é um interpretador de linha de comando (shell) simples, escrito em C para sistemas de base Unix. O projeto demonstra conceitos fundamentais: leitura de linha, análise (parsing), execução de processos, pipes, redirecionamentos e gerenciamento básico de jobs.

## Funcionalidades principais

- Prompt dinâmico que mostra o diretório de trabalho atual:  
  /caminho/atual |Otaldoshell|>

- Comandos externos via fork() + execvp() (ex: ls, pwd, echo).

- Builtins:
  - `cd [diretório]` — muda o diretório (se nenhum argumento, usa $HOME). Observação: `cd` não funciona dentro de um pipe.
  - `clear` — limpa a tela (usa system("clear")).
  - `exit` — encerra o shell.

- Pipes:
  - Suporte a encadeamento com `|` (ex: `ls -l | grep txt | wc -l`).

- Redirecionamentos:
  - Entrada: `< arquivo` (ex: `sort < dados.txt`).
  - Saída (sobrescrever): `> arquivo` (ex: `echo hi > out.txt`).
  - Saída (append): `>> arquivo` (ex: `echo hi >> out.txt`).

- Execução em background:
  - Use `&` no fim da linha para rodar o último comando em background (ex: `sleep 10 &`).
  - O shell imprime o PID do job em background.

- Tratamento de processos-zumbi:
  - Um tratador de sinal (SIGCHLD) coleta filhos terminados para evitar zumbis quando processos são executados em background.

## Design do código (principais funções)

- parse_line(char *line)  
  Analisa a linha lida (recebe uma cópia via strdup) e constrói uma lista ligada de `struct Command`. Cada nó contém:
  - args: array de strings (ponteiros para tokens dentro da string original),
  - inputFile/outputFile: ponteiros para tokens correspondentes (se houver),
  - append: flag para `>>`,
  - background: flag aplicada ao último comando quando `&` está presente,
  - next: próximo comando em pipeline.

- execute_commands(struct Command *cmd)  
  Percorre a lista de comandos, cria pipes conforme necessário e faz forks para executar cada comando. Configura redirecionamentos (dup2) e lida com espera por processos em foreground ou listar PID em background. Builtins são tratados antes de forks.

- free_commands(struct Command *cmd)  
  Libera os arrays `args` e as estruturas `Command`. As strings (tokens) apontam para a string original retornada por ler_comando() — por isso essa string é liberada separadamente no loop principal.

- handle_sigchld(int sig)  
  Simples tratador que chama waitpid(-1, NULL, WNOHANG) em loop para "colher" filhos terminados.

- ler_comando()  
  Lê uma linha de stdin (usa fgets), faz strdup e retorna a string para o parser. EOF (Ctrl+D) retorna NULL.

- loop_principal()  
  Exibe prompt, chama ler_comando, parse_line, execute_commands e free_commands; libera a string lida.

## Compilação

Compile com avisos ativados:

```bash
gcc -Wall -Wextra main.c -o otaldoshell
```

## Exemplos de uso

- Comando simples:
  /home/usuario |Otaldoshell|> ls -l

- Pipe:
  /home/usuario |Otaldoshell|> ls | grep main

- Redirecionamento:
  /home/usuario |Otaldoshell|> echo "ola" > /tmp/teste.txt
  /home/usuario |Otaldoshell|> cat < /tmp/teste.txt

- Append:
  /home/usuario |Otaldoshell|> echo "mais" >> /tmp/teste.txt

- Background:
  /home/usuario |Otaldoshell|> sleep 5 &
  [12345] sleep

- cd:
  /home/usuario |Otaldoshell|> cd /tmp

## Observações de implementação

- parse_line modifica a string passada (por isso o shell usa strdup ao ler); tokens apontam para trechos dessa string.
- free_commands libera apenas o array de ponteiros `args` e as structs Command; a string original (retornada por ler_comando) deve ser liberada no loop principal.
- `cd` é executado no processo pai (sem fork), por isso não pode ser parte de um pipeline.
- Em caso de erros de execução, o shell usa perror para mensagens apropriadas.

## Testes sugeridos

- Teste comandos simples, pipes encadeados, redirecionamentos e background.
- Verifique que processos em background não geram processos zumbi (`ps`/`top`) e que o shell imprime o PID ao iniciar um background job.

## Créditos

Victor Thiago Barbosa
victorthiagob@hotmail.com
