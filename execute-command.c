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

#include <error.h>


//***********************************************************************
//                    FUNCTION DECLARATIONS
//***********************************************************************

void execute_if(command_t c);
void execute_until(command_t c);
void execute_while(command_t c);
void execute_sequence(command_t c);
void execute_pipe(command_t c);
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
  error (0, 0, "warning: profiling not yet implemented");
  return -1;
}

//***********************************************************************
//                    EXECUTE COMMAND
//***********************************************************************

void
execute_command(command_t c, int profiling) {
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
  
  execute_io(c);
}


void execute_if(command_t c) {
  
  int condition;
  // TODO: evaluate command[0] as true or false, store result in condition
  if (condition)
    execute_command(c->u.command[1]);
  else if (c->u.command[2] != NULL)
    execute_command(c->u.command[2]);
  
}


void execute_until(command_t c) {
  // TODO
}


void execute_while(command_t c) {
  // TODO
}


void execute_sequence(command_t c) {
  // TODO
}


void execute_pipe(command_t c) {
  // TODO
}


void execute_simple(command_t c) {
  // TODO
}


void execute_subshell(command_t c) {
  // TODO
}


void execute_io(command_t c) {
  // TODO
}




