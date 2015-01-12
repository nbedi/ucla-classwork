// UCLA CS 111 Lab 1 command reading

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

#include <error.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */


//***********************************************************************
//              STRUCT DEFINITIONS & AUXILARY FUNCTIONS
//***********************************************************************

typedef struct command_node *command_node_t;

struct command_node {
  command_t command;
  command_node_t next;
  command_node_t prev;
};

struct command_stream{
  command_node_t current_node;
};

typedef enum {
  IF,
  THEN,
  ELSE,
  FI,
  WHILE,
  DO,
  DONE,
  UNTIL,
  SEMICOLON,
  PIPE,
  LPARENS,
  RPARENS,
  INPUT,
  OUTPUT,
  COMMENT,
  NEWLINE,
  END,
  SIMPLE,
} word_type;

typedef struct {
  word_type type;
  char *string;
} word;

void bad_error(int* lineNum) {
  fprintf(stderr, "%d: syntax error\n", *lineNum);
  exit(-1);
}

//***********************************************************************
//                        FUNCTION DECLARATIONS
//***********************************************************************

word get_next_word(char*, int*, int, int*);
int generate_from_simple(command_t, int, char*, int*, int, command_t, int*);
int get_command(char*, int*, int, command_t, int*);

//***********************************************************************
//                            GET NEXT WORD
//***********************************************************************

word get_next_word(char* buffer, int* it, int bufSize, int* lineNum) {
  word w;
  
  // deal with END
  // FILE * fp;
  // fp = fopen("file.txt", "r");
  // fprintf(stdout, "%p %d %d %p %c", it, *it, bufSize, buffer, buffer[*it]);
  // fclose(fp);


  if (*it == bufSize) {
    w.type = END;
    return w;
  }
  
  // remove beginning whitespace
  while (buffer[*it] == ' ' || buffer[*it] == '\t') {
    (*it)++;
    if (*it == bufSize) {
      w.type = END;
      return w;
    }
  }
  
  // deal with single-char tokens, ; | ( ) < > #
  // don't need to update string component of the word bc we don't use it
  char c = buffer[*it];
  switch (c) {
    case ';':
      w.type = SEMICOLON;
      (*it) = (*it) + 1;
      return w;
    case '|':
      w.type = PIPE;
      (*it) = (*it) + 1;
      return w;
    case '(':
      w.type = LPARENS;
      (*it) = (*it) + 1;
      return w;
    case ')':
      w.type = RPARENS;
      (*it) = (*it) + 1;
      return w;
    case '<':
      w.type = INPUT;
      (*it) = (*it) + 1;
      return w;
    case '>':
      w.type = OUTPUT;
      (*it) = (*it) + 1;
      return w;
      
    // TODO: double check, not sure if right
    case '\n':
      w.type = NEWLINE;
      (*lineNum) = (*lineNum) + 1;
      (*it) = (*it) + 1;
      return w;
      
    case '#':
      w.type = COMMENT;
      while (*it < bufSize && buffer[*it] != '\n')
        (*it) = (*it) + 1;
      if (*it != bufSize) {
        (*it) = (*it) + 1;  // move past new line, point to start of next word
        (*lineNum) = (*lineNum) + 1;
      }
      return w;
      
    default:
      break;
  }
  
  // deal with words
  // TODO: deal with realloc if word len exceeds 16
  int wordLen = 1024;
  w.string = (char*)checked_malloc(sizeof(wordLen));
  
  // create string
  int stringIndex = 0;
  while (buffer[*it] != ' ' && buffer[*it] != '\t' && buffer[*it] != '\n' && *it != bufSize) {
    if (!(isalpha(buffer[*it]) || isdigit(buffer[*it]) ||
          buffer[*it]=='!' || buffer[*it]=='%' || buffer[*it]=='+' ||
          buffer[*it]==',' || buffer[*it]=='-' || buffer[*it]=='.' ||
          buffer[*it]=='/' || buffer[*it]==':' || buffer[*it]=='@' ||
          buffer[*it]=='^' || buffer[*it]=='_'
        ))
      bad_error(lineNum);
    w.string[stringIndex] = buffer[*it];
    (*it) = (*it) + 1;
    stringIndex++;
  }
  w.string[stringIndex] = '\0';
  
  // assign special words
  if (strcmp(w.string, "if") == 0) {
    w.type = IF;
    return w;
  }
  else if (strcmp(w.string, "then") == 0) {
    w.type = THEN;
    return w;
  }
  else if (strcmp(w.string, "else") == 0) {
    w.type = ELSE;
    return w;
  }
  else if (strcmp(w.string, "fi") == 0) {
    w.type = FI;
    return w;
  }
  else if (strcmp(w.string, "while") == 0) {
    w.type = WHILE;
    return w;
  }
  else if (strcmp(w.string, "do") == 0) {
    w.type = DO;
    return w;
  }
  else if (strcmp(w.string, "done") == 0) {
    w.type = DONE;
    return w;
  }
  else {
    w.type = SIMPLE;
    return w;
  }
}

//***********************************************************************
//                        GENERATE FROM SIMPLE
//***********************************************************************

int generate_from_simple(command_t tempCom, int word_count, char* buffer, int* it, int bufSize, command_t com, int* lineNum)
{
    word next_word = get_next_word(buffer, it, bufSize, lineNum);
    switch(next_word.type) {
        case PIPE:
          com->type = PIPE_COMMAND;
          com->u.command[0] = tempCom;
          command_t secondCom = checked_malloc(sizeof(struct command));
          if (get_command(buffer, it, bufSize, secondCom, lineNum)) {
            com->u.command[1] = secondCom;
            return 1;
          }
          else bad_error(lineNum);

        case SEMICOLON:
          ;
          int* seqIt = &(*it);
          next_word = get_next_word(buffer, it, bufSize, lineNum);
          if (next_word.type == NEWLINE || next_word.type == END)
          {
            com = tempCom;
            return 1;
          }
          else 
          {
            com->type = SEQUENCE_COMMAND;
            com->u.command[0] = tempCom;
            command_t secondCom = checked_malloc(sizeof(struct command));
            get_command(buffer, seqIt, bufSize, secondCom, lineNum);
            com->u.command[1] = secondCom;
            it = seqIt;
            return 1;
          }  

        //TODO: CORNER CASE
        case LPARENS:
          bad_error(lineNum);
          return 0;

        case RPARENS:
          *it = (*it)-1;
          com = tempCom;    
          return 1;       

        case INPUT:
          com = tempCom;
          next_word = get_next_word(buffer, it, bufSize, lineNum);
          com->input = next_word.string;
          int* inIt = &(*it);
          next_word = get_next_word(buffer, inIt, bufSize, lineNum);
          if (next_word.type == OUTPUT)
          {
            next_word = get_next_word(buffer, inIt, bufSize, lineNum);
            com->output = next_word.string;
            it = inIt;
          }
          return 1;

        case OUTPUT:
          com = tempCom;
          next_word = get_next_word(buffer, it, bufSize, lineNum);
          com->output = next_word.string;
          return 1;

        case COMMENT:
          next_word = get_next_word(buffer, it, bufSize, lineNum);
          while (next_word.type != NEWLINE && next_word.type != END) {
            next_word = get_next_word(buffer, it, bufSize, lineNum);
          }
          return 1;

        case NEWLINE:
        case END:
          com = tempCom;
          return 1;

        //TODO: Multiple words
        case IF:
        case THEN:
        case ELSE:
        case FI:
        case WHILE:
        case DO:
        case DONE:
        case UNTIL:
        case SIMPLE:
          tempCom->u.word[word_count] = next_word.string;
          com = tempCom;
          word_count++;
          return generate_from_simple(tempCom, word_count, buffer, it, bufSize, com, lineNum);

        default:
          bad_error(lineNum);
          return 0;
    }
}

//***********************************************************************
//                            GET COMMAND
//***********************************************************************

int get_command(char* buffer, int* it, int bufSize, command_t com, int* lineNum) {
  word next_word = get_next_word(buffer, it, bufSize, lineNum);
  com->status = -1;
  command_t newCom = checked_malloc(sizeof(struct command));
  
  switch (next_word.type) {
    case IF:
      com->type = IF_COMMAND;
      // IF
      com->u.command[0] = newCom;
      if (get_command(buffer, it, bufSize, newCom, lineNum)) {
          next_word = get_next_word(buffer, it, bufSize, lineNum);
          // THEN
          if (next_word.type == THEN) {
              command_t thenCom = checked_malloc(sizeof(struct command));
              com->u.command[1] = thenCom;
              if (get_command(buffer, it, bufSize, thenCom, lineNum)) {
                  next_word = get_next_word(buffer, it, bufSize, lineNum);
                  // ELSE
                  if (next_word.type == ELSE) {
                    command_t elseCom = checked_malloc(sizeof(struct command));
                    com->u.command[2] = elseCom;
                    if (get_command(buffer, it, bufSize, elseCom, lineNum)) {
                        next_word = get_next_word(buffer, it, bufSize, lineNum);
                        // FI AFTER ELSE
                        if (next_word.type == FI)
                          return 1;
                        else
                          bad_error(lineNum);
                    }
                    else
                      bad_error(lineNum);
                  }
                  // NO ELSE - STRAIGHT TO FI
                  else if (next_word.type == FI)
                    return 1;
                  else
                    bad_error(lineNum);
              }
              else
                bad_error(lineNum);
          }
          else
            bad_error(lineNum);
      }
      else
        bad_error(lineNum);

    case WHILE:
      com->type = WHILE_COMMAND;
      // WHILE
      com->u.command[0] = newCom;
      if (get_command(buffer, it, bufSize, newCom, lineNum)) {
        next_word = get_next_word(buffer, it, bufSize, lineNum);
        // DO
        if (next_word.type == DO) {
          command_t doCom = checked_malloc(sizeof(struct command));
          com->u.command[1] = doCom;
          if (get_command(buffer, it, bufSize, doCom, lineNum)) {
            next_word = get_next_word(buffer, it, bufSize, lineNum);
            // DONE
            if (next_word.type == DONE)
              return 1;
            else
              bad_error(lineNum);
          }
          else
            bad_error(lineNum);
        }
        else
          bad_error(lineNum);
      }
      else
        bad_error(lineNum);

    case UNTIL:
      com->type = UNTIL_COMMAND;
      // UNTIL
      com->u.command[0] = newCom;
      if (get_command(buffer, it, bufSize, newCom, lineNum)) {
        next_word = get_next_word(buffer, it, bufSize, lineNum);
        // DO
        if (next_word.type == DO) {
          command_t doCom = checked_malloc(sizeof(struct command));
          com->u.command[1] = doCom;
          if (get_command(buffer, it, bufSize, doCom, lineNum)) {
            next_word = get_next_word(buffer, it, bufSize, lineNum);
            // DONE
            if (next_word.type == DONE)
              return 1;
            else
              bad_error(lineNum);
          }
          else
            bad_error(lineNum);
        }
        else
          bad_error(lineNum);
      }
      else
        bad_error(lineNum);
      
    case LPARENS:
      com->type = SUBSHELL_COMMAND;
      if (get_command(buffer, it, bufSize, newCom, lineNum)) {
        next_word = get_next_word(buffer, it, bufSize, lineNum);
        if (next_word.type == RPARENS) {
          com->u.command[0] = newCom;
          return 1;
        }
        else bad_error(lineNum);
      }
      else bad_error(lineNum);


    case COMMENT:
    case NEWLINE:
      return get_command(buffer, it, bufSize, com, lineNum);

    case SEMICOLON:
      bad_error(lineNum);
      return 0;

    case END:
      return 0;

    case THEN:
    case ELSE:
    case FI:
    case DO:
    case DONE:
    case SIMPLE: 
      ;
      // make temporary command
      command_t tempCom = checked_malloc(sizeof(struct command));
      tempCom->type = SIMPLE_COMMAND;
      //TODO FIX ALLOCATION MAYBE
      tempCom->u.word = checked_malloc(sizeof(char*)*64);
      tempCom->u.word[0] = checked_malloc(sizeof(char)*64);
      tempCom->u.word[0] = next_word.string;
      int word_count = 1;
      return generate_from_simple(tempCom, word_count, buffer, it, bufSize, com, lineNum);
      
    default:
      bad_error(lineNum);
      return 0;
  } 
}

//***********************************************************************
//                     MAKE COMMAND STREAM
//***********************************************************************

command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
  
  //*****************************************
  //         CREATE & ALLOCATE BUFFER
  //*****************************************
  
  char buffer[1024]="";
  int current_size = 1024;
  //TODO: ALLOCATE MEMORY BETTER SOON
  //buffer = (char*)checked_malloc(sizeof(current_size));
  int byte_count = 0;
  for(;;) {
    int c = get_next_byte(get_next_byte_argument);
    //TODO: ALLOCATE MEMORY
    // if (byte_count == current_size) {
    //   current_size = current_size*2;
    //   size_t size_size = sizeof(current_size);
    //   buffer = (char*)checked_grow_alloc((void*)buffer, &(size_size));
    // }
    if (c != EOF && byte_count < current_size) {
      buffer[byte_count] = c;
      byte_count++;
    }
    else
      break;
  }
  
  
  //*****************************************
  //   MAKE COMMAND STREAM OF COMMAND NODES
  //*****************************************

  int currentPos = 0;
  int lineNum = 1;
  
  // initialize command stream
  command_stream_t command_stream = checked_malloc(sizeof(struct command_stream));
  
  // initialize first node
  command_node_t firstCommandNode = checked_malloc(sizeof(struct command_node));
  command_stream->current_node = firstCommandNode;
  command_node_t currentCommandNode = command_stream->current_node;
  
  // initialize first command
  command_t firstCommand = checked_malloc(sizeof(struct command));
  firstCommandNode->command = firstCommand;
  command_t currentCommand = currentCommandNode->command;

  while (get_command(buffer, &currentPos, byte_count, currentCommand, &lineNum) == 1) {
    // create new node
    command_node_t newCommandNode = checked_malloc(sizeof(struct command_node));
    newCommandNode->prev = currentCommandNode;
    currentCommandNode->next = newCommandNode;
    currentCommandNode = currentCommandNode->next;

    // create new command for that node
    command_t newCommand = checked_malloc(sizeof(struct command));
    newCommandNode->command = newCommand;
    currentCommand = currentCommandNode->command;
  }
  
  if (currentCommandNode->prev != NULL) // couldn't add first command
    currentCommandNode->prev->next = NULL;
  else
    command_stream->current_node = NULL;

  return command_stream;
}


//***********************************************************************
//                     READ COMMAND STREAM
//***********************************************************************


command_t
read_command_stream (command_stream_t s) {
  if (s->current_node) {
    command_node_t current_node = s->current_node;
    s->current_node = current_node->next;
    return current_node->command;
  }
  else
    return NULL;
}
