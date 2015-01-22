// UCLA CS 111 Lab 1 command execution

// Copyright 2012-2014 Paul Eggert.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "command.h"
#include "command-internals.h"

#include <string.h>
#include <error.h>
#include <unistd.h>    // fork
#include <sys/types.h> // pid_t, execvp
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>  // waitpid
#include <stdlib.h>
#include <stdio.h>


//***********************************************************************
//                    FUNCTION DECLARATIONS
//***********************************************************************

void execute_if(command_t c);
void execute_until(command_t c);
void execute_while(command_t c);
void execute_sequence(command_t c);
void execute_pipe(command_t c);
void do_pipe(int fd[], command_t c);
void execute_simple(command_t c);
void execute_subshell(command_t c);
void execute_io(command_t c);

//***********************************************************************
//                    AUXILARY FUNCTIONS
//***********************************************************************

int
command_status (command_t c) {
  return c->status;
}

//***********************************************************************
//                    PREPARE_PROFILING
//***********************************************************************

int
prepare_profiling (char const *name) {
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
  if (strcmp(name, ""))
  {
    ;
  }
  error (0, 0, "warning: profiling not yet implemented");
  return -1;
}

//***********************************************************************
//                    EXECUTE COMMAND
//***********************************************************************

void
execute_command(command_t c, int profiling) {
  // TODO: what is the "profiling" int passed as a parameter??
  if (profiling == 1)
  {
    ;
  }
  switch (c->type) {
      
    case IF_COMMAND:
      execute_if(c);
      break;
    case UNTIL_COMMAND:
      execute_until(c);
      break;
    case WHILE_COMMAND:
      execute_while(c);
      break;
    case SEQUENCE_COMMAND:
      execute_sequence(c);
      break;
    case PIPE_COMMAND:
      execute_pipe(c);
      break;
    case SIMPLE_COMMAND:
      execute_simple(c);
      break;
    case SUBSHELL_COMMAND:
      execute_subshell(c);
      break;
    default:
      error (1, 0, "command not found");
  }
  
}


void execute_if(command_t c) {
  execute_command(c->u.command[0], 0);
  // todo: returns 0 if successful??
  if (c->u.command[0]->status == 0)
    execute_command(c->u.command[1], 0);
  else if (c->u.command[2] != NULL)
    execute_command(c->u.command[2], 0);
}


void execute_until(command_t c) {
  int condition;
  for(;;) {
    execute_command(c->u.command[0], 0);
    condition = c->u.command[0]->status;
    if (condition != 0)
      execute_command(c->u.command[1], 0);
    else
      break;
  }
}


void execute_while(command_t c) {
  int condition;
  for(;;) {
    execute_command(c->u.command[0], 0);
    condition = c->u.command[0]->status;
    if (condition != 0)
      break;
    else
      execute_command(c->u.command[1], 0);;
  }
}


void execute_sequence(command_t c) {
  execute_io(c);
  execute_command(c->u.command[0], 0);
  execute_command(c->u.command[1], 0);
  c->status = c->u.command[1]->status;
}


void execute_pipe(command_t c) {
  int fd[2];
  pid_t pid = fork();
  
  pipe(fd);
  
  if (pid > 0) { // parent
    int status;
    if (waitpid(pid, &status, 0) == -1)
      error(1, 0, "error with child proccess exiting simple command");
    // TODO: status of parent
  }
  
  else if (pid == 0) // child
    do_pipe(fd, c);
  
  else
    error(1, 0, "error with forking");

}


void do_pipe(int fd[], command_t c) {
  
  pid_t pid = fork();
  
  if (pid > 0) {    // parent - write end of the pipe
    dup2(fd[1], 1); // stdout
    close(fd[0]);   // doesn't need it
    execute_command(c->u.command[0], 0);
  }
  
  else if (pid == 0) { // child - read end of the pipe
    dup2(fd[0], 0);    // stdin
    close(fd[1]);      // doesn't need it
    execute_command(c->u.command[1], 0);
  }
  
  else
    error(1, 0, "error with pipe");
  
}


void execute_simple(command_t c) {
  // if fork function is successful, it returns twice: once in child process with
  // return value "0" and again in the parent process with child's PID as its return
  // value.
  pid_t pid = fork();
  
  // parent process
  if (pid > 0) {
    int status;
    // wait for child to finish
    if (waitpid(pid, &status, 0) == -1)
      error(1, 0, "error with child proccess exiting simple command");
    // TODO: check
    c->status = WEXITSTATUS(status);
  }
  // child process
  else if (pid == 0) {

    execute_io(c);
    // first argument: name of file to execute, second: next arguments
    if (execvp(c->u.word[0], c->u.word) < 0)
      error(1, 0, "command not found");
  }
  else
    error(1, 0, "error with forking");
  
}


void execute_subshell(command_t c) {
  execute_io(c);
  execute_command(c->u.command[0], 0);
  c->status = c->u.command[0]->status;
}


void execute_io(command_t c) {
  // input
  if (c->input != NULL) {
    close(0);
    int in = open(c->input, O_RDONLY); // read only
    if (in < 0)
      error(1, 0, "error opening input file");
    if (dup2(in, 0) < 0) // 0 refers to stdin
      error(1, 0, "error in dup2: input file");
    if (close(in) < 0)
      error(1, 0, "error closing input file");
  }
  // output
  if (c->output != NULL) {
    int out = open(c->output, O_CREAT |  // O_CREAT: if file doesn't exist, create it
                              O_TRUNC |  // O_TRUNC: initally clear all data from file
                              O_WRONLY,  // write only
                              S_IRUSR | S_IRGRP | S_IWUSR | S_IWGRP);
                              // 3rd parameter modes: mean user + group can read/write
    if (out < 0)
      error(1, 0, "error opening output file");
    if (dup2(out, 1) < 1) // 1 refers to stdout
      error(1, 0, "error in dup2: output file");
    if (close(out) < 0)
      error(1, 0, "error closing output file");
  }
}




