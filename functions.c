#include "shell.h"

extern int shell_pid;

void functions(char* parts[], int index, int background, char* home, char *hist[], int current){

  if(!strcmp(parts[0],"quit")){
    exit(0);
    return 0;
  }
  else if(!strcmp(parts[0], "pwd") && background != 1){
    char pwd[10001];
    getcwd(pwd,sizeof(pwd));
    printf("%s\n",pwd);
  }
  else if(!strcmp(parts[0], "history"))
    history(hist, current, parts, index);
  // else if(!strcmp(parts[0], "echo") && background != 1)
  //   echo(index, parts);
  else if(!strcmp(parts[0], "ls") && background != 1){
    ls(home, parts, index);
  }
  else if(!strcmp(parts[0], "cd")){
    cd(home, parts, index);
  }
  else if(!strcmp(parts[0], "pinfo"))
    pinfo(home, parts, index);
  else if(!strcmp(parts[0], "nightswatch"))
    nightswatch(parts, index);
  else if(!strcmp(parts[0], "jobs"))
      jobs(parts);
  else if(!strcmp(parts[0], "kjob"))
      kjob(index, parts);
  else if(!strcmp(parts[0], "bg"))
      bg(index, parts);
  else if(!strcmp(parts[0], "fg"))
      fg(index, parts);
  else if(!strcmp(parts[0], "setenv"))
      setenvv(parts, index);
  else if(!strcmp(parts[0], "unsetenv"))
      unsetenvv(parts, index);
  else if(!strcmp(parts[0], "overkill"))
      okill();
  else if(!strcmp(parts[0], "quit"))
      kill(shell_pid, SIGKILL);
  else{
      exec(parts, index, background, home);
  }
}
