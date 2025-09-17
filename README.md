# Otaldoshell

    Otaldoshell é um interpretador de linha de comando (shell) simples, escrito em C para sistemas operacionais do tipo Unix (como Linux). O projeto demonstra os conceitos fundamentais de criação de um shell, incluindo a leitura de comandos, a análise de argumentos e a execução de processos.

## Funcionalidades

    O shell possui as seguintes funcionalidades implementadas:

### Prompt Dinâmico

    Exibe o diretório de trabalho atual antes do prompt |Otaldoshell|> .

### Execução de Comandos Externos

    Executa qualquer programa encontrado no PATH do sistema (ex: ls, gcc, pwd, echo).

### Gerenciamento de Processos

    Utiliza o padrão fork() e execvp() para criar novos processos para executar comandos, enquanto o processo principal do shell aguarda a sua finalização (waitpid()).

### Comandos Embutidos (Builtins)

    possui comandos internos que são executados diretamente pelo shell, sem criar um novo processo:

- `cd [diretório]`: Altera o diretório de trabalho atual. Se nenhum diretório for especificado, muda para o diretório HOME do usuário.

- `clear`: Limpa a tela do terminal (utilizando system("clear")).

- `exit`: Encerra a execução do Otaldoshell.

## Como Compilar

Para compilar o projeto, você precisará de um compilador C, como o GCC.

    ```bash


    gcc main.c -o otaldoshell


    ```

  Este comando irá gerar um arquivo executável chamado otaldoshell.

## Como Executar

Após a compilação, execute o shell a partir do seu terminal:

    ```bash

    ./otaldoshell

    ```

Você verá o prompt e poderá começar a digitar os comandos.

Exemplo de uso:

    ```bash

    /home/usuario |Otaldoshell|> ls -l
    total 48
    -rwxr-xr-x 1 usuario usuario 16896 set 16 13:25 otaldoshell
    -rw-r--r-- 1 usuario usuario  2409 set 16 13:24 main.c

    /home/usuario |Otaldoshell|> cd /tmp
    /tmp |Otaldoshell|> pwd
    /tmp

    /tmp |Otaldoshell|> exit

    ```

## Estrutura do Código

    O código é dividido em funções modulares para maior clareza:

- `main()`: Ponto de entrada do programa que inicia o loop principal.

- `loop_principal()`: O coração do shell. Responsável por exibir o prompt, chamar as funções de leitura, análise e execução em um loop contínuo.

- `ler_comando()`: Lê uma linha de comando da entrada padrão (stdin).

- `identificador_d_comando()`: Analisa a linha de comando lida, quebrando-a em "tokens" (o comando e seus argumentos).

- `executor()`: A função principal de lógica. Verifica se o comando é um dos builtins e, caso não seja, utiliza fork() e execvp() para executá-lo como um processo externo.

## CREDITOS

    Victor Thiago Barbosa
    victorthiagob@hotmail.com
