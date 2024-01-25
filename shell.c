//#include "../include/ic.h"
#include "ic.c"

#define _POSIX_C_SOURCE 1

#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../include/msgs.h"


void handle_sigint(int);

static char *detect = "CTRL-C is not allowed\n";
void handle_sigint(int signum) 
{ 
  write(STDOUT_FILENO, detect, strlen(detect)); 
}
void stop()
{
  //printf("child stopped\n");
  exit(0);
}

int main() {

  char historylist[10][128] = {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}};
  int hlsize = 0;
  char prevdir[128];
  int pidlist[128];
  int plsize = 0;
  int running = 1;

  struct sigaction handler;
  handler.sa_handler = handle_sigint;
  handler.sa_flags = 0;
  sigemptyset(&handler.sa_mask);
  sigaction(SIGINT, &handler, NULL);


  while (running) {
    /*
    if (kill(getpid(), SIGINT) == -1)
    {
      break;
    }
      */

    char input[128] = {0};
    char *cmdline = "$ ";
    char dir[128] = {0};
    
    // Get current directory
    if (getcwd(dir, sizeof(dir)) == NULL)
      write(STDOUT_FILENO, GETCWD_ERROR_MSG, sizeof(GETCWD_ERROR_MSG));

    // Get command input

    write(STDOUT_FILENO, dir, strlen(dir));
    write(STDOUT_FILENO, cmdline, strlen(cmdline));

    if (read(STDIN_FILENO, input, sizeof(input)) == -1)
    {
      char *errmsg = FORMAT_MSG("shell", READ_ERROR_MSG);
      write(STDOUT_FILENO, errmsg, strlen(errmsg));
      continue;
    }

    // Convert input string to clean command string
    char arg1[128] = {0};
    int index1 = 0;
    char arg2[128] = {0};
    int index2 = 0;

    char *args[2] = {NULL, NULL};

    char cmd[strlen(input)];
    int i = 0;
    input[strlen(input)] = 0;

    while (input[i] != ' ' && i < strlen(input) - 1) {
      cmd[i] = input[i];
      i++;
    }
    cmd[i] = 0;
    i++;
    
    if (input[0] == '!') 
    {
      if (history_function(hlsize, input, cmd, args[0]) >= 0) 
      {
        int index = history_function(hlsize, input, cmd, args[0]);

        if (index < hlsize - 10 || index >= hlsize)
        {
          char *errmsg = FORMAT_MSG("history", HISTORY_INVALID_MSG);
          write(STDOUT_FILENO, errmsg, strlen(errmsg));
          continue;
        }
        if (hlsize >= 10)
        {
          int temp = (hlsize - index);
          index = 10 - temp;
        }
        
        strcpy(input, historylist[index]);
        input[strlen(input)] = 0;
        
        char prevcmd[128];
        int pindex = 0;

        while(input[pindex] != ' ' && pindex < strlen(input) - 1)
        {
          prevcmd[pindex] = input[pindex];
          pindex++;
        }
        prevcmd[pindex] = 0;
        pindex++;
        i = pindex;
        strcpy(cmd, prevcmd);
        cmd[strlen(cmd)] = 0;

        write(STDOUT_FILENO, input, strlen(input));
      }
      else
      {
        continue;
      }
    }

    while (i < strlen(input) && input[i] != ' ') {
      arg1[index1] = input[i];
      index1++;
      i++;
    }
    i++;
    while (i < strlen(input) && input[i] != ' ') {
      arg2[index2] = input[i];
      index2++;
      i++;
    }
    i++;

    if (strlen(arg1) != 0 && strlen(arg2) == 0) {
      arg1[strlen(arg1) - 1] = 0;
      args[0] = arg1;
    }

    if (strlen(arg2) != 0) {
      arg1[strlen(arg1)] = 0;
      arg2[strlen(arg2) - 1] = 0;
      args[0] = arg1;
      args[1] = arg2;
    }

// INTERNAL EXECUTION ---------------------------------------------------

    // pwd command
    if (!strcmp(cmd, "pwd")) {
      if (pwd_function(input, strlen(cmd))) {
        history_add(hlsize, input, historylist);
        hlsize++;
      }
    }

    // exit command
    else if (!strcmp(cmd, "exit")) {
      if (exit_function(input, strlen(cmd)) != 0)
        running = 0;
    }

    // cd command
    else if (!strcmp(cmd, "cd")) {
      if (input[3] != '-') {
        if (getcwd(prevdir, sizeof(prevdir)) == NULL)
          write(STDOUT_FILENO, GETCWD_ERROR_MSG, sizeof(GETCWD_ERROR_MSG));
      }
      if (cd_function(input, prevdir, args[1], strlen(cmd))) {
        history_add(hlsize, input, historylist);
        hlsize++;
      }
      if (input[3] == '-') {
        for (int i = 0; i < strlen(dir); i++) {
          prevdir[i] = dir[i];
        }
        prevdir[strlen(dir)] = 0;
      }
    }

    // help command
    else if (!strcmp(cmd, "help")) {
      if (args[1] != NULL)
      {
        write(STDOUT_FILENO, "help: ", strlen("help: "));
        write(STDOUT_FILENO, TMA_MSG, sizeof(TMA_MSG));
      }
      else if (help_function(input, args[0])) {
        history_add(hlsize, input, historylist);
        hlsize++;
      }
    }
    
    // history command
    else if (!strcmp(cmd, "history")) {
      if (args[0] != NULL) {
        char *errmsg = FORMAT_MSG("history", TMA_MSG);
        write(STDOUT_FILENO, errmsg, strlen(errmsg));
      }
      else {
        history_add(hlsize, input, historylist);
        hlsize++;
        history_print(hlsize - 1, historylist);
      }
    }

// EXTERNAL COMMAND---------------------------------------------------- 

    // For external commands
    else {

      int bground = 0;
      if (input[strlen(input) - 2] == '&')
      {
        bground = 1;
      }
      if (!strcmp(arg1, "&"))
      {
        args[0] = NULL;
      }
      if (!strcmp(arg2, "&"))
      {
        args[1] = NULL;
      }
      //printf("forking...\n");
/*
      if (input[strlen(input) - 2] == '&') {
        printf("background\n");
        bground = 1;
      }
*/
      int child = fork();

      if (bground && child == 0) {
        signal(SIGQUIT, stop);
        if (execlp(cmd, cmd, args[0], args[1], (char *)NULL) == -1)
        {
          exit(EXIT_FAILURE);
        }
        while (1)
        {
          //printf("...\n");
          sleep(1);
        }
        //write(STDOUT_FILENO, EXEC_ERROR_MSG, sizeof(EXEC_ERROR_MSG));
      } else if (!bground && child == 0) {
        /*
        sleep(1);
        if (execlp(cmd, cmd, args[0], args[1], (char *)NULL) == -1)
          write(STDOUT_FILENO, EXEC_ERROR_MSG, sizeof(EXEC_ERROR_MSG));
        return 0;
        */
        if (execlp(cmd, cmd, args[0], args[1], (char *)NULL) == -1)
        {
          exit(EXIT_FAILURE);
        }
      }

      int wstatus;
      if (bground) 
      {
        sleep(1);
        pidlist[plsize] = waitpid(child, &wstatus, WNOHANG);
      } 
      else 
      {
        if (waitpid(child, &wstatus, 0) == -1)
        {
          char *errmsg = FORMAT_MSG("shell", WAIT_ERROR_MSG);
          write(STDOUT_FILENO, errmsg, strlen(errmsg));
        }
      }

      if (WIFEXITED(wstatus)) {
        int exit = WEXITSTATUS(wstatus);
        if (exit == 0) {
          history_add(hlsize, input, historylist);
          hlsize++;
        }
        else {
          char *msg = FORMAT_MSG("shell", EXEC_ERROR_MSG);
          write(STDOUT_FILENO, msg, strlen(msg));
        }
        //printf("wstatus %d\n", wstatus);
        //printf("fork done\n");
      }
    }
  }

// CLEAN ZOMBIE PROCESSES

  for (int i = 0; i < plsize; i++) 
  {
    kill(pidlist[i], SIGQUIT);
  }
}
