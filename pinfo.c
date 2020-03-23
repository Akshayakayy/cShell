#include "shell.h"

void pinfo(char *home, char **argarr, int cnt)
{
  if(cnt > 2)
  {
      printf("pinfo: Usage: pinfo [pid]\n");
      return;
  }

  char exec[1001];
  char stat1[1001] = "/proc/";

  if(cnt == 2)
  {
    strcat(stat1,argarr[1]);
    strcpy(exec, stat1);
    strcat(stat1,"/stat");
    strcat(exec,"/exe");
  }
  else
  {
    int pid = getpid();
    sprintf(stat1, "/proc/%d/stat", pid);
    sprintf(exec, "/proc/%d/exe", pid);
  }

  FILE *fp = fopen(stat1,"r");
  if(fp == NULL)
  {
    printf("Error: process doesn't exist\n");
  }
  else
  {
    int pid;
    char runstatus;
    long unsigned int mem;
    fscanf(fp,"%d %*s %c %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %lu %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d",&pid,&runstatus,&mem);
    fclose(fp);

    char* execname = (char*) malloc(4096);
    int a = readlink(exec,execname,4095);
    char path[1005];

    if(a<0){
      printf("Error: process doesn't exist\n");
    }
    else{
      execname[a]='\0';
      int flag = 0;
      int i = 0;
      int homelen = strlen(home);

      for(;i<homelen; i++)
      {
          if(execname[i] != home[i]){
            strcpy(path, execname);
            flag = 1;
            break;
          }
      }
      if(flag == 0){
        path[0] = '~';
        for(;i<strlen(execname);i++)
            path[i-homelen+1] = execname[i];
        path[i-homelen+1] = '\0';
      }
      printf("Process ID -- %d\n",pid);
      printf("Process Status -- %c\n",runstatus);
      printf("Memory -- %lu\n",mem);
      printf("Executable Path -- %s\n",path);
    }
  }
}
