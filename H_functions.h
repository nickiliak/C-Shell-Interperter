/*  @author Nikos Iliakis - csd4375
    shell implemntation          */

/* When compiled with -Wall -ansi -pedantic 
-g getline throws odd warning  technically not necessary*/
#define  _GNU_SOURCE 

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

#define RL_BUFSIZE 512
#define TOK_BUFSIZE 128
#define TOK_DELIM " \n\t\r"

/* Helper functions */
int shell_cd(char **args);
int shell_exit(char **args);
int count_commands(char **args);
void prompt_print();
int count_total_pipes(char **args);
void shell_pipes_args(char **args, char **new_args);