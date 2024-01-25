#ifdef ic
#define ic
/*
#define _POSIX_C_SOURCE 1

#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "msgs.h"
*/
int pwd_function(char *input, int cmd_len);

int exit_function(char *input, int cmd_len);

int cd_function(char *input, char *prevdir, int cmd_len);

int help_function(char *input);

void history_add(int size, char *input, char history[5][128]);

#endif
