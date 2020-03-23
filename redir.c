#include "shell.h"

void redirect(char* parts[], int count,  char* home, char *hist[], int current)
{
    int redirf[2];
    redirf[0] = 0;
    redirf[1] = 0;

    int filedesc[2];
    filedesc[0] = 0;
    filedesc[1] = 1;

    int temp[2];
    char* cmd[count];
    int j = 0;
    int i = 0;
    while(i<count)
    {
      if(!strcmp(parts[i], ">")|| !strcmp(parts[i], "<")|| !strcmp(parts[i], ">>")){
        if(i>=count-1){
          printf("csh: no file found\n");
          return;
        }
      }
        if(!strcmp(parts[i], "<"))
        {
            redirf[0] = 1;
                filedesc[0] = open(parts[++i], O_RDONLY);
                if(filedesc[0] == -1)
                {
                    perror("File Error");
                    return;
                }
        }
        else if(!strcmp(parts[i], ">"))
        {
            redirf[1] = 1;
              filedesc[1] = open(parts[++i], O_WRONLY | O_CREAT, 0755);
              if(filedesc[1] == -1)
              {
                  perror("File Error");
                  return;
              }
        }
        else if(!strcmp(parts[i], ">>"))
        {
            redirf[1] = 1;
              filedesc[1] = open(parts[++i], O_WRONLY | O_APPEND | O_CREAT, 0755);
              if(filedesc[1] == -1)
              {
                  perror("File Error");
                  return;
              }
        }
        i++;
    }

    i=0;
    while(i<count)
    {
        if(!strcmp(parts[i], ">")|| !strcmp(parts[i], "<")|| !strcmp(parts[i], ">>")) break;
        else
        {
            cmd[j] = (char*) malloc(4096);
            strcpy(cmd[j++], parts[i]);
        }
        i++;
    }

    if(redirf[0])
    {
        temp[0] = dup(0);
        if(dup2(filedesc[0], 0) == -1)
        {
            perror("RedirError");
            return;
        }
    }

    if(redirf[1])
    {
        temp[1] = dup(1);
        if(dup2(filedesc[1], 1) == -1)
        {
            perror("RedirError");
            return;
        }
    }

    int background = 0;
    if(!strcmp(cmd[j-1], "&"))
    {
        background = 1;
        strcpy(cmd[j-1], "\0");
    }

    if(!strcmp(cmd[0],"quit")){
      exit(0);
      return 0;
    }
    else if(!strcmp(cmd[0], "pwd") && background != 1){
      char pwd[10001];
      getcwd(pwd,sizeof(pwd));
      printf("%s\n",pwd);
    }
    else if(!strcmp(cmd[0], "history"))
      history(hist, current, cmd, j);
    // else if(!strcmp(cmd[0], "echo") && background != 1)
    //   echo(j, cmd);
    else if(!strcmp(cmd[0], "ls") && background != 1){
      ls(home, cmd, j);
    }
    else if(!strcmp(cmd[0], "cd")){
      cd(home, cmd, j);
    }
    else if(!strcmp(cmd[0], "pinfo"))
      pinfo(home, cmd, j);
    else if(!strcmp(cmd[0], "nightswatch"))
      nightswatch(cmd, j);
    else{
      cmd[j]=NULL;
      exec(cmd, j, background, home);
    }

    if(redirf[0]) dup2(temp[0], 0);
    if(redirf[1]) dup2(temp[1], 1);
}
