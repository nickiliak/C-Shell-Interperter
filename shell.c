/*  @author Nikos Iliakis - csd4375
    shell implemntation          */

#include "H_functions.h"

extern int count_commands(char **args);
extern void prompt_print();
extern int count_total_pipes(char **args);
extern void shell_pipes_args(char **args, char **new_args);


/* After detecting pipes we prepare to channel the output of the left command to the input of the input 
   of the right command using pipe() and dup2()  */
int shell_pipes(char **args, int pos, int end){
    int old_pipes[2];
    int new_pipes[2];
    pid_t pid;
    char **new_args = malloc(RL_BUFSIZE * sizeof(char*));

    while(pos != end){
        if(pos + 1 != end){
            if (pipe(new_pipes) == -1){
                perror("pipe");
                exit(EXIT_FAILURE);
            }
        }
        
        pid = fork();
        if(pid < 0){
            perror("pipe fork fail");
            exit(EXIT_FAILURE);
        }

        shell_pipes_args(args, new_args);
        if(pid == 0){

            if(pos != 0){
                if(dup2(old_pipes[0], 0) < 0){
                    perror("dup2");
                    exit(EXIT_FAILURE);     
                }
                close(old_pipes[0]);
                close(old_pipes[1]);
            }

            if(pos + 1 != end){
                close(new_pipes[0]);
                if(dup2(new_pipes[1], 1) < 0){
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
                close(new_pipes[1]);
            }
            

            if(execvp(new_args[0], new_args) == -1) perror("exec");
            exit(EXIT_FAILURE);
        }

        if(pos != 0){
            close(old_pipes[0]);
            close(old_pipes[1]);
        }
        if(pos + 1 != end){
            old_pipes[0] = new_pipes[0];
            old_pipes[1] = new_pipes[1];
        }

        pos++;
    }

    close(old_pipes[0]);
    close(old_pipes[1]);
    waitpid(pid, NULL, 0);
    
    return 1;
}


/* Here we check for redirection. If > is detected in the string array we direct the stdout to the file we created,
   with dup , if < is detected in the string array we direct the stdin to the file that already exists */
char **shell_redirect(char **args){
    int i = 0, j = 0;
    int file;
    char **new_args = malloc(RL_BUFSIZE * sizeof(char*));

    
    while(args[i] != NULL){
        
        if(!strcmp(args[i], ">")){

            if(!strcmp(args[i+1], ">")){
                file = open(args[i+2], O_CREAT| O_WRONLY | O_APPEND, 0644);
                if(dup2(file, STDOUT_FILENO) < 0){
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
                close(file);
                break;
            }
            else{
                file = creat(args[i+1], 0644);
                if(dup2(file, STDOUT_FILENO) < 0){
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
                close(file);
                break;
            }
        }
        else if(!strcmp(args[i], "<")){
            file = open(args[i+1], O_RDONLY);
            if(dup2(file, STDIN_FILENO) < 0){
                perror("dup2");
                exit(EXIT_FAILURE);
            }
            close(file);
            break;
        }
        i++;
    }

    i = 0;
    while(args[i] != NULL){
        if(strcmp(args[i], ">") && strcmp(args[i], "<")){
            new_args[j] = args[i];
            j++;
        }else break;
        i++;
    }
    
    new_args[j] = '\0';
    return new_args;
}


/* Here we check if its a simple command , a redirect , or a pipe. First we check if there is a pipe , 
   if there is we call shell_pipes if not we fork we call shell_redirect to check and prepare if there
   is redirection and then we execute the command.*/
int shell_command(char **args){
    pid_t pid;
    int total_pipes;


    if((total_pipes = count_total_pipes(args)) > 0) /* Check if pipes exist */
        if(shell_pipes(args, 0, total_pipes + 1) == 1) return 1;
    
    pid = fork();   
    if(pid == 0){
        args = shell_redirect(args);
        if(execvp(args[0], args) == -1) perror("execvp");
        exit(EXIT_FAILURE);
    }
    else if(pid < 0) perror("command fork failed");
    else{
        waitpid(pid, NULL, WUNTRACED);
    }

    return 1;
}


/* This function is where either the inbuilt commands are called or where
   we go to call shell_command to execute with execvp*/
int execute(char **args){
    int i = 0;
    int status = 404;
    int size = RL_BUFSIZE;
    int flag = 0;
    char **new_args = malloc(RL_BUFSIZE * sizeof(char*));
    char **exec_args = malloc(RL_BUFSIZE * sizeof(char*));

    if (args[0] == NULL){
        return 1;
    }

    if (args[0] != NULL && !strcmp(args[0], "cd")) status = shell_cd(args);
    else if(args[0] != NULL && !strcmp(args[0], "exit")) return shell_exit(args);

    if(status == 404) flag = 1;
    i = 0;
    status = count_commands(args) + 1;
    while(args[status] != NULL){
        new_args[i] = args[status];
        status++;
        i++;
        if(i >= size){
            size += TOK_BUFSIZE;
            new_args = realloc(new_args, size * sizeof(char*));
            exec_args = realloc(exec_args, size * sizeof(char*));
        }
    }

    for(i = 0; i < count_commands(args); i++) exec_args[i] = args[i];
    if(flag == 1) shell_command(exec_args);
    
    i = 0;
    while(args[i] != NULL){ 
        args[i] = "";
        args[i] = NULL;
    }


    if(new_args[0] == NULL || !strcmp(new_args[0], "\0")){
        return 1;
    }
    else execute(new_args);

    return 1;
}



/* Parsing the commands read_line received and splitting it into an array of strings
   everytime it receives ' ' or ; or > or < or | basically tokenizing the command. Strtok
   was used. */
char **parse(char *line){
    int bufsize = TOK_BUFSIZE, split_size = TOK_BUFSIZE, i = 0, pos = 0, flag;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *split_token = malloc(split_size * sizeof(char*));
    char *token;

    if(!tokens || !split_token){
        fprintf(stderr, "parse: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, TOK_DELIM);
    
    while(token != NULL){
        
        while(token[i] != '\0'){
            flag = 0;

            if(token[i] != ';' && token[i] != '>' && token[i] != '<' && token[i] != '|'){
                if(i >= split_size){
                    split_size += TOK_BUFSIZE;
                    split_token = realloc(split_token, split_size * sizeof(char*));
                    if(!split_token){
                        fprintf(stderr, "split_line allocation error\n");
                        exit(EXIT_FAILURE);
                    }
                }
                strncat(split_token, &token[i], 1);
            }
            else{
                flag = 1;
                if(strcmp(split_token, "")){
                    tokens[pos] = malloc(i * sizeof(char*));
                    strcpy(tokens[pos],split_token);
                    pos++;
                } 

                strcpy(split_token, "");

                if(pos >= bufsize){
                    bufsize += TOK_BUFSIZE;
                    tokens = realloc(tokens, bufsize * sizeof(char*));
            
                    if(!tokens){
                        fprintf(stderr, "split_line allocation error\n");
                        exit(EXIT_FAILURE);
                    }
                }

                if(token[i] == ';') tokens[pos] = ";";
                else if(token[i] == '>') tokens[pos] = ">";
                else if(token[i] == '<') tokens[pos] = "<";
                else if(token[i] == '|') tokens[pos] = "|";
                pos++;
            }
            i++;
        }

        if(!flag && split_token != NULL){
            tokens[pos] = malloc(i * sizeof(char*));
            strcpy(tokens[pos],split_token);
            pos++;
            strcpy(split_token, "");
        }

        token = strtok(NULL, TOK_DELIM);
        i=0;
    }

    tokens[pos] = NULL;

    if(tokens[pos+1] != NULL){
        while(tokens[pos+1] != NULL){
            tokens[pos+1] = NULL;
            pos++;
        }
    }

    return tokens;
}


/* Reading the user input with getline and passing to the
   address of char *line */
char *read_line(){
    size_t bufsize = 0;
    char *line = NULL;
    
    if(getline(&line, &bufsize,stdin) == -1){
        if(feof(stdin)) exit(EXIT_FAILURE);
        else{
            printf("read: error");
            exit(EXIT_FAILURE);
        }
    }

    return line;
}


/* The shell implemntation is split in three simple ways:
        READ: read the command the user input from the terminal
        PARSE: prepare and store the command to be ready for execution
        EXECUTE: execute the command
*/
int main(){

    char *line;
    char **args;
    int status = 1;

    while(status){

        prompt_print();
        line = read_line();              
        args = parse(line);             
        status = execute(args);             
            
        free(line);
    }

    return EXIT_SUCCESS;
}