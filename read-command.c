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

#include <error.h>

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */


//***********************************************************************
//              STRUCT DEFINITIONS & AUXILARY FUNCTIONS
//***********************************************************************

struct command_node
{
  struct command* command;
  struct command_node* next;
  struct command_node* prev;
}

struct command_stream
{
  struct command_node* current_node;
}

struct word
{
  enum word_type type;
  char *string;
}

enum word_type
{
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
  EOF,
  SIMPLE
};

void bad_error(int lineNum)
{
  fprintf(stderr, "%d: syntax error\n", lineNum);
  exit(-1);
}


//***********************************************************************
//                            GET NEXT WORD
//***********************************************************************

word get_next_word(char *buffer, int &it, int bufSize, int &lineNum)
{
  word newWord;
  if (it == bufSize) 
  {
    newWord.type = EOF;
    return newWord;
  }
}


//***********************************************************************
//                            GET COMMAND
//***********************************************************************

bool get_command(char *buffer, int &it, int bufSize, command_t &com, int &lineNum)
{
  word next_word = get_next_word(buffer, &it, bufSize, &lineNum);
  com.status = -1;
  command_t newCom;

  switch (next_word.type) 
  {
      
      
    case IF:
      com.type = IF_COMMAND;
      // IF
      com->u.command[0] = *(newCom);
      if (get_command(buffer, &it, bufSize, &newCom, &lineNum))
      {
          next_word = get_next_word(buffer, &it, bufSize, &lineNum);
          // THEN
          if (next_word.type == THEN)
          {
              command_t thenCom;
              com->u.command[1] = *(thenCom);
              if (get_command(buffer, &it, bufSize, &thenCom, &lineNum))
              {
                  next_word = get_next_word(buffer, &it, bufSize, &lineNum);
                  // ELSE
                  if (next_word.type == ELSE)
                  {
                    command_t elseCom;
                    com->u.command[2] = *(elseCom);
                    if (get_command(buffer, &it, bufSize, &elseCom, &lineNum))
                    {
                        next_word = gget_next_word(buffer, &it, bufSize, &lineNum);
                        // FI AFTER ELSE
                        if (next_word.type == FI)
                          return true;
                        else
                          bad_error(lineNum);
                    }
                    else
                      bad_error(lineNum);
                  }
                  // NO ELSE - STRAIGHT TO FI
                  else if (next_word.type == FI)
                    return true;
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
      com.type = WHILE_COMMAND;
      // WHILE
      com->u.command[0] = *(newCom);
      if (get_command(buffer, &it, bufSize, &newCom, &lineNum))
      {
        next_word = get_next_word(buffer, &it, bufSize, &lineNum);
        // DO
        if (next_word.type == DO)
        {
          command_t doCom;
          com->u.command[1] = *(doCom);
          if (get_command(buffer, &it, bufSize, &newCom, &lineNum))
          {
            next_word = get_next_word(buffer, &it, bufSize, &lineNum);
            // DONE
            if (next_word.type == DONE)
              return true;
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
      com.type = UNTIL_COMMAND;
      // UNTIL
      com->u.command[0] = *(newCom);
      if (get_command(buffer, &it, bufSize, &newCom, &lineNum))
      {
        next_word = get_next_word(buffer, &it, bufSize, &lineNum);
        // DO
        if (next_word.type == DO)
        {
          command_t doCom;
          com->u.command[1] = *(doCom);
          if (get_command(buffer, &it, bufSize, &newCom, &lineNum))
          {
            next_word = get_next_word(buffer, &it, bufSize, &lineNum);
            // DONE
            if (next_word.type == DONE)
              return true;
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
      com.type = SUBSHELL_COMMAND;
      if (get_command(buffer, &it, bufSize, &newCom, &lineNum))
      {
        next_word = get_next_word(buffer, &it, bufSize, &lineNum);
        if (next_word.type == RPARENS)
        {
          com->u.command[0] = *(newCom);
          return true;
        }
        else bad_error(lineNum);
      }
      else bad_error(lineNume);


      
    case COMMENT:
      while (get_next_word(buffer, &it, bufSize, &lineNum).type != NEWLINE)
        continue;
      return get_command(buffer, &it, bufSize, &com, &lineNum);


    case NEWLINE:
    case SEMICOLON:
      return get_command(buffer, &it, bufSize, &com, &lineNum);


    case EOF:
          return false;

    case THEN:
    case ELSE:
    case FI:
    case DO:
    case DONE:
    case SIMPLE: 
      // make temporary command
      command tempCom;
      tempCom.type = SIMPLE_COMMAND;
      //TODO FIX ALLOCATION MAYBE
      tempCom->u.word = checked_malloc(sizeof(char*)*10);
      tempCom->u.word[word_count] = checked_malloc(sizeof(char)*10);
      tempCom->u.word[word_count] = next_word.string;
      int word_count = 1;


      next_word = get_next_word(buffer, &it, bufSize);
      switch(next_word.type)
      {
          case PIPE:
            com.type = PIPE_COMMAND;
            com->u.command[0] = tempCom;
            command_t secondCom;
            if (get_command (buffer, &it, bufSize, &secondCom, &lineNum))
            {
              com->u.command[1] = *(secondCom);
              return true;
            }
            else bad_error(lineNum);

          // case SEMICOLON:
          //   com.type = SEQUENCE_COMMAND;
          //   com->u.command[0] = tempCom;
          //   command_t secondCom;
          //   next_word = get_next_word(buffer, &it, bufSize);

          //   if (get_command (buffer, int &it, int bufSize, command_t &secondCom, int &lineNum))
          //   {
          //     com->u.command[1] = *(secondCom);
          //     return true;
          //   }
          //   else return bad_error(lineNum);

          case LPARENS:

          case INPUT:

          case OUTPUT:

          case COMMENT:
            next_word = get_next_word(buffer, &it, bufSize, &lineNum);
            while (next_word.type != NEWLINE && next_word.type != EOF) 
            {
              next_word = get_next_word(buffer, &it, bufSize, &lineNum);
            }
            return true;

          // TODO: SEQUENCE STATEMENTS
          case SEMICOLON:
          case NEWLINE:
          case EOF:
            com = tempCom;
            return true;

          case IF:
          case THEN:
          case ELSE:
          case FI:
          case WHILE:
          case DO:
          case DONE:
          case UNTIL:
          case SIMPLE:


          default: bad_error(lineNum);
      }

    default:  bad_error(lineNum);
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
  
  char *buffer;
  int current_size = 1024;
  buffer = checked_malloc(sizeof(current_size));
  int byte_count = 0;
  while (true)
  {
    int c = *(get_next_byte)(get_next_byte_argument);
    if (byte_count == current_size) 
    {
      current_size = current_size*2;
      buffer = checked_grow_alloc(buffer, sizeof(current_size));
    }
    if (c != EOF)
    {
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
  int lineNum = 0;
  
  // initialize command stream
  command_stream_t command_stream;
  
  // initialize first node
  command_node firstCommandNode;
  command_stream->current_node = firstCommandNode;
  command_node *currentCommandNode = command_stream.current_node;
  
  // initialize first command
  command firstCommand;
  firstCommandNode->command = firstCommand;
  command_t currentCommand = currentCommandNode.command;

  while (get_command(buffer, &currentPos, byte_count, &currentCommand, &lineNum))
  {
    // create new node
    command_node newCommandNode;
    newCommandNode->prev = *(currentCommandNode);
    currentCommandNode->next = newCommandNode;
    currentCommandNode = currentCommandNode.next;

    // create new command for that node
    command newCommand;
    newCommandNode->command = newCommand;
    currentCommand = currentCommandNode.command;
  }
  
  if (currentCommandNode->prev != NULL) // couldn't add first command
    currentCommandNode->prev.next = NULL;
  else
    command_stream.current_node = NULL;

}


//***********************************************************************
//                     READ COMMAND STREAM
//***********************************************************************


command_t
read_command_stream (command_stream_t s)
{
  if (s->current_node)
  {
    command_node current_node = s->current_node;
    s.current_node = current_node.next;
    return current_node->command;
  }
  else
    return NULL;
}
