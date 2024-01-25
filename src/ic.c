#include "../include/ic.h"

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

int pwd_function(char *input, int cmd_len) {
  char dir[128];
  if (getcwd(dir, sizeof(dir)) == NULL) {
    char *msg = FORMAT_MSG("pwd", GETCWD_ERROR_MSG);
    write(STDOUT_FILENO, msg, strlen(msg));
    return 0;
  } else if (cmd_len < strlen(input) - 1) {
    char *msg = FORMAT_MSG("pwd", TMA_MSG);
    write(STDOUT_FILENO, msg, strlen(msg));
    return 0;
  } else {
    write(STDOUT_FILENO, dir, strlen(dir));
    write(STDOUT_FILENO, "\n", strlen("\n"));
  }
  return 1;
}

int exit_function(char *input, int cmd_len) {
  char *errmsg = FORMAT_MSG("exit", TMA_MSG);

  if (cmd_len < strlen(input) - 1) {
    write(STDOUT_FILENO, errmsg, strlen(errmsg));
    return 0;
  }
  return 1;
}

int cd_function(char *input, char *prevdir, char *arg2, int cmd_len) {
  struct passwd *user;
  char *errmsg = FORMAT_MSG("cd", CHDIR_ERROR_MSG);
  uid_t uid;

  if ((user = getpwuid(uid = getuid())) == NULL) {
    printf("Fail\n");
    return 0;
  }
  else if (arg2 != NULL)
  {
    char *tma = FORMAT_MSG("cd", TMA_MSG);
    write(STDOUT_FILENO, tma, strlen(tma));
  }
  else if ((strlen(input) - 1 == 4 && input[3] == '~') || strlen(input) - 1 == 2) 
  {
    if (chdir(user->pw_dir) == -1)
    {
      write(STDOUT_FILENO, errmsg, strlen(errmsg));
      return 0;
    }
  } 
  else if (input[3] == '-') 
  {
    prevdir[strlen(prevdir)] = 0;
    if (chdir(prevdir) == -1)
    {
      write(STDOUT_FILENO, errmsg, strlen(errmsg));
      return 0;
    }
  } 
  else if (input[2] == ' ' && input[3] != '~') 
  {
    char dir[strlen(input) - 4];

    for (int i = 0; i < strlen(input) - 4; i++) 
    {
      dir[i] = input[i + 3];
    }
    dir[strlen(input) - 4] = 0;

    if (chdir(dir) == -1)
    {
      write(STDOUT_FILENO, errmsg, strlen(errmsg));
      return 0;
    }
  } 
  else if (input[3] == '~' && input[4] != 0) 
  {
    char dir[128] = {0};
    getcwd(dir, sizeof(dir));
    int size = strlen(dir);

    for (int i = 0; i < strlen(input) - 4; i++) {
      dir[size + i] = input[i + 4];
    }
    dir[strlen(dir) - 1] = 0;

    if (chdir(dir) == -1)
    {
      write(STDOUT_FILENO, errmsg, strlen(errmsg));
      return 0;
    }
  }
  return 1;
}

int help_function(char *input, char* cmd)
{
  int index = 0;
  int invalid = 0;
  char help[16];
  
  const char *tmamsg = FORMAT_MSG("help", TMA_MSG);
  const char *helpmsg = FORMAT_MSG("help", HELP_HELP_MSG);
  const char *pwdmsg = FORMAT_MSG("pwd", PWD_HELP_MSG);
  const char *exitmsg = FORMAT_MSG("exit", EXIT_HELP_MSG);
  const char *cdmsg = FORMAT_MSG("cd", CD_HELP_MSG);
  const char *historymsg = FORMAT_MSG("history", HISTORY_HELP_MSG);
  const char *extmsg = FORMAT_MSG("", EXTERN_HELP_MSG);

  while (input[5 + index] != 0)
  {
    help[index] = input[5 + index];
    if (help[index] == ' ')
    {
      invalid = 1;
    }
    index++;
  }
  help[index - 1] = 0;

  if (invalid)
  {
    write(STDOUT_FILENO, tmamsg, strlen(tmamsg));
    return 0;
  }
  else if (index == 0)
  {
    write(STDOUT_FILENO, helpmsg, strlen(helpmsg));
    write(STDOUT_FILENO, pwdmsg, strlen(pwdmsg));
    write(STDOUT_FILENO, exitmsg, strlen(exitmsg));
    write(STDOUT_FILENO, cdmsg, strlen(cdmsg));
    write(STDOUT_FILENO, historymsg, strlen(historymsg));
  }
  else if (!strcmp(help, "help"))
  {
    write(STDOUT_FILENO, helpmsg, strlen(helpmsg));
  }
  else if (!strcmp(help, "pwd"))
  {
    write(STDOUT_FILENO, pwdmsg, strlen(pwdmsg));
  }
  else if (!strcmp(help, "exit"))
  {
    write(STDOUT_FILENO, exitmsg, strlen(exitmsg));
  }
  else if (!strcmp(help, "cd"))
  {
    write(STDOUT_FILENO, cdmsg, strlen(cdmsg));
  }
  else if (!strcmp(help, "history"))
  {
    write(STDOUT_FILENO, historymsg, strlen(historymsg));
  }
  else 
  {
    write(STDOUT_FILENO, cmd, strlen(cmd));
    write(STDOUT_FILENO, extmsg, strlen(extmsg));
  }
  return 1;
}

void history_add(int size, char *input, char history[5][128])
{
  if (size < 10)
  {
    for (int i = 0; i < strlen(input); i++)
    {
      history[size][i] = input[i];
    }
  }
  else
  {
    for (int i = 0; i < 9; i++)
    {
      strcpy(history[i], history[i + 1]);
      history[i][strlen(history[i+1])] = 0;
    }
    for (int i = 0; i < strlen(input); i++)
    {
      history[9][i] = input[i];
    }
    history[9][strlen(input)] = 0;
  }
}


void history_print(int size, char list[10][128])
{
  char *tab = "\t";
  char tens = '0';
  char ones = '0';

  tens += size / 10;
  ones += size % 10;

  char strnum[3] = {tens, ones, 0};

  if (size <= 9)
  {
    for (int i = size; i >= 0; i--)
    {
      char num = '0' + i;
      char strnum[3] = {0};
      strnum[0] = num;
      strnum[1] = 0;

      write(STDOUT_FILENO, strnum, strlen(strnum));
      write(STDOUT_FILENO, tab, strlen(tab));
      write(STDOUT_FILENO, list[i], strlen(list[i]));
    }
  }
  else
  {
    for (int i = 0; i < 10; i++)
    {
      if (strnum[1] > '9' || strnum[1] < '0')
      {
        strnum[0] = strnum[0] - 1;
        strnum[1] = '9';
      }
      if (strnum[0] == '0')
      {
        char onenum[2] = {0};
        onenum[0] = strnum[1];
        onenum[1] = 0;
        write(STDOUT_FILENO, onenum, strlen(onenum));
        write(STDOUT_FILENO, tab, strlen(tab));
        write(STDOUT_FILENO, list[9 - i], strlen(list[9 - i]));
      }
      else
      {
        write(STDOUT_FILENO, strnum, strlen(strnum));
        write(STDOUT_FILENO, tab, strlen(tab));
        write(STDOUT_FILENO, list[9 - i], strlen(list[9 - i]));
      }
      strnum[1] = strnum[1] - 1;
    }
  }
}

int history_function(int hlsize, char *input, char *cmd, char *arg)
{
  char num[4] = {0};
  int index = 0;

  if (arg != NULL)
  {
    char *errmsg = FORMAT_MSG("history", TMA_MSG);
    write(STDOUT_FILENO, errmsg, strlen(errmsg));
    return -1;
  }
  else if (!strcmp(cmd, "!!"))
  {
    if (hlsize == 0)
    {
      char *errmsg = FORMAT_MSG("history", HISTORY_NO_LAST_MSG);
      write(STDOUT_FILENO, errmsg, strlen(errmsg));
      return -1;
    }
    else
      return hlsize - 1;
  }
  else
  {
    int ipindex = 1;
    int result = 0;
    while (ipindex < strlen(input))
    {
      if (input[ipindex] < '0' && input[ipindex] > '9')
        return -1;
      num[ipindex - 1] = input[ipindex];
      ipindex++;
    }
    num[strlen(num) - 1] = 0;
    
    for (int i = 0; i < strlen(num); i++)
    {
      result = result * 10 + (num[i] - 48);
    }
    return result;
  }

  char *errmsg = FORMAT_MSG("history", HISTORY_INVALID_MSG);
  write(STDOUT_FILENO, errmsg, strlen(errmsg));
  return -1;
}
