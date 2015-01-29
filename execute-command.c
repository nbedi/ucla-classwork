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
#include "alloc.h"

#include <string.h>
#include <error.h>
#include <unistd.h>    // fork
#include <sys/types.h> // pid_t, execvp
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>  // waitpid
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h> //profiling


//***********************************************************************
//                    FUNCTION DECLARATIONS
//***********************************************************************

void execute_if(command_t c, int profiling);
void execute_until(command_t c, int profiling);
void execute_while(command_t c, int profiling);
void execute_sequence(command_t c, int profiling);
void execute_pipe(command_t c, int profiling);
void execute_simple(command_t c, int profiling);
void execute_subshell(command_t c, int profiling);
void execute_io(command_t c, int profiling);

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

  int prof_file = open(name, O_CREAT |  // O_CREAT: if file doesn't exist, create it
                          O_TRUNC |  // O_TRUNC: initally clear all data from file
                          O_WRONLY,  // write only
                          S_IRUSR | S_IRGRP | S_IWUSR | S_IWGRP);
                          // 3rd parameter modes: mean user + group can read/write
  if (prof_file < 0)
    error(1, 0, "error opening output file");
  // if (dup2(prof_file, 1) < 1) // 1 refers to stdout
    // error(1, 0, "error in dup2: output file");

  return prof_file;
}

//***********************************************************************
//                    INITIAL_PROFILING
//***********************************************************************

// time_t
// initial_profiling (char **command_name, int fd) {

// }


//***********************************************************************
//                    EXECUTE_PROFILING
//***********************************************************************

void
execute_profiling (char **command_name, int fd) {
  char* buffer = checked_malloc(sizeof(char)*1024);
  int buffer_index = 0;
  struct timespec *p_timespec = checked_malloc(sizeof(struct timespec));

  time_t time_finished = 0;
  time_t time_finished_nano = 0;
  // time_t real_time = 0;
  // time_t user_cpu_time = 0;
  // time_t command_cpu_time = 0;
  //TODO: trailing zeros
  if (!(clock_gettime(CLOCK_REALTIME, p_timespec)<0))
  {
    fflush(stdout);
    time_finished = p_timespec->tv_sec;
    time_finished_nano = p_timespec->tv_nsec;
  }

  double output_time = 0.0;
  output_time = ((double)time_finished_nano)/1000000000;
  output_time = (double)time_finished+ output_time;

  char* time_finished_string = checked_malloc(sizeof(char)*1024);
  sprintf(time_finished_string, "%f", output_time);

  //TODO figure out for all stuff
  int i;
  for (i=0; time_finished_string[i]!='\0' && buffer_index < 1023; i++)
  {
    buffer[buffer_index] = time_finished_string[i];
    buffer_index++;
  }
  if (buffer_index < 1023)
  {
    buffer[buffer_index] = ' ';
    buffer_index++;
  }

  int j;
  int k;
  //TODO segfault
  for (j =0; command_name[j]!=NULL&& buffer_index < 1023; j++)
  {
    for (k=0; command_name[j][k]!='\0'&& buffer_index < 1023; k++)
    {
      if (command_name[j][k]=='\n')
      {
        buffer[buffer_index] = ' ';
        buffer_index++;
      }
      else
      {
        buffer[buffer_index] = command_name[j][k];
        buffer_index++;
      }
    }
    if (buffer_index < 1023)
    {
      buffer[buffer_index] = ' ';
      buffer_index++;
    }
  }
  if (buffer_index < 1023)
  {
    buffer[buffer_index]='\n';
    buffer_index++;
  }

  buffer[buffer_index] = '\0';

  if (write(fd, buffer, sizeof(char)*buffer_index) < 0)
  {
    error(1, 0, "error writing profile");
  }
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
  if (profiling == 0) {
    ;
  }
  
  switch (c->type) {
      
    case IF_COMMAND:
      execute_if(c, profiling);
      break;
    case UNTIL_COMMAND:
      execute_until(c, profiling);
      break;
    case WHILE_COMMAND:
      execute_while(c, profiling);
      break;
    case SEQUENCE_COMMAND:
      execute_sequence(c, profiling);
      break;
    case PIPE_COMMAND:
      execute_pipe(c, profiling);
      break;
    case SIMPLE_COMMAND:
      execute_simple(c, profiling);
      break;
    case SUBSHELL_COMMAND:
      execute_subshell(c, profiling);
      break;
    default:
      error (1, 0, "command not found");
  }
  
}


void execute_if(command_t c, int profiling) {
  execute_command(c->u.command[0], profiling);
  // todo: returns 0 if successful??
  if (c->u.command[0]->status == 0)
    execute_command(c->u.command[1], profiling);
  else if (c->u.command[2] != NULL)
    execute_command(c->u.command[2], profiling);
}


void execute_until(command_t c, int profiling) {
  int condition;
  for(;;) {
    execute_command(c->u.command[0], profiling);
    condition = c->u.command[0]->status;
    if (condition != 0)
      execute_command(c->u.command[1], profiling);
    else
      break;
  }
}


void execute_while(command_t c, int profiling) {
  int condition;
  for(;;) {
    execute_command(c->u.command[0], profiling);
    condition = c->u.command[0]->status;
    if (condition != 0)
      break;
    else
      execute_command(c->u.command[1], profiling);
  }
}


void execute_sequence(command_t c, int profiling) {
  execute_io(c, profiling);
  execute_command(c->u.command[0], profiling);
  execute_command(c->u.command[1], profiling);
  c->status = c->u.command[1]->status;
}


void execute_pipe(command_t c, int profiling) {
  int fd[2];
  
  if (pipe(fd) == -1)
    error(1, 0, "error creating pipe");

  pid_t first_pid = fork(); 

  // OUTER PARENT
  if (first_pid > 0) {
    pid_t second_pid = fork();
    
    // INNER PARENT - waits for all to finish, cleans up
    if (second_pid > 0) {
      if (close(fd[0]) < 0)
        error(1, 0, "error closing");
      if (close(fd[1]) < 0)
        error(1, 0, "error closing");
      pid_t first_finished;
      int status;
      first_finished = waitpid(-1, &status, 0);
      // second command finished first - want that status
      if (first_finished == first_pid) {
        c->status = status;
        execute_profiling(c->u.word, profiling); 
        if (waitpid(second_pid, &status, 0) == -1)
          error(1, 0, "error with child proccess exiting simple command");
      }
      // first command finished first - want status of second command
      else if (first_finished == second_pid) {
        if (waitpid(first_pid, &status, 0) == -1)
          error(1, 0, "error with child proccess exiting simple command");
        c->status = status;
      }
      else
        error(1, 0, "error in parent of forking");
    }
    
    // INNER CHILD - does first command
    else if (second_pid == 0) {
      if (dup2(fd[1], 1) < 0)
        error(1, 0, "error in dup2");
      if (close(fd[0]) < 0)
        error(1, 0, "error closing");
      execute_command(c->u.command[0], profiling);
      _exit(c->u.command[0]->status);
    }
    
    else
      error(1, 0, "error with forking"); 
  }
  
  // OUTER CHILD - does second command
  else if (first_pid == 0) {
    if (dup2(fd[0], 0) < 0)
      error(1, 0, "error in dup2");
    if (close(fd[1]) < 0)
      error(1, 0, "error closing");
    execute_command(c->u.command[1], profiling);
    _exit(c->u.command[1]->status);
  }
  
  else
    error(1, 0, "error with forking");

}




void execute_simple(command_t c, int profiling) {
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
    execute_profiling(c->u.word, profiling); 
  }
  // child process
  else if (pid == 0) {

    execute_io(c, profiling);
    // first argument: name of file to execute, second: next arguments
    if (execvp(c->u.word[0], c->u.word) < 0)
      error(1, 0, "command not found");
  }
  else
    error(1, 0, "error with forking");
  
}


void execute_subshell(command_t c, int profiling) {
  execute_io(c, profiling);
  execute_command(c->u.command[0], profiling);
  c->status = c->u.command[0]->status;
}


void execute_io(command_t c, int profiling) {
  // input
  if (profiling)
  {
    ;
  }

  if (c->input != NULL) {
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




