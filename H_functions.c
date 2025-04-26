/*  @author Nikos Iliakis - csd4375
    shell implemntation          */
#include "H_functions.h"


/* Count the number of commands seperated by ; */
int count_commands(char **args){
    int i = 0;
    while(args[i] != NULL && strcmp(args[i],";")) i++;
    return i;
}


/* Print the prompt of the terminal */
void prompt_print(){
    long buffer = pathconf(".", _PC_PATH_MAX);
	char* pathbuffer = (char *) malloc((size_t)buffer);
    printf("%s@cs345sh/%s$ ", getlogin(),getcwd(pathbuffer, buffer));
}


/* Count the total number of pipes in args string array */
int count_total_pipes(char **args){
    int total = 0;
    int i = 0 ;

    while(args[i] != NULL){
        if(!strcmp(args[i], "|")){
            total++;
        }
        i++;
    }

    return total;
}

/* Here we prepare so that the |s are removed from the arg strin array 
   and prepare it for the next command */
void shell_pipes_args(char **args, char **new_args){
    int i,j = 0;

    for(i = 0; new_args[i] != NULL; i++) new_args[i] = NULL;

    i = 0;
    while(args[i] != NULL){
        if(strcmp(args[i], "|")){
            new_args[j] = args[i];
            j++;
        }
        else{
            i++;
            j = 0;
            break;
        }
        i++;
    }


    for(; args[i] != NULL; i++){
        args[j] = args[i];
        j++;
    }
    for(; args[j] != NULL ; j++) args[j]=NULL;

    return;
}

/* Implementation of cd */
int shell_cd(char **args){

    int total = count_commands(args);
    if(total > 2){
        fprintf(stderr, "cd: too many arguements! \"cd\"\n");
        return total;
    }


    if (args[1] == NULL) fprintf(stderr, "cd: expected argument to \"cd\"\n");
    else if(chdir(args[1]) != 0) perror("cd");

    return 1;
}

/* An exit function that uses the exit function because why not */
int shell_exit(char **args){
    exit(EXIT_SUCCESS);
}