#include "shell.h"

extern int curr_pid;
int time_counter = 0;
pid_det pid_arr[3001];

void backproc()
{
    int pid;
    int status;
    int i=0;
    int flag=0;

    while(i < 3001)
    {
        if(pid_arr[i].id != 0)
        {
            pid = waitpid(pid_arr[i].id, &status, WNOHANG);
            if(pid)
            {
                fprintf(stderr,"shell: %s with PID %d exited normally\n", pid_arr[i].name, pid_arr[i].id);
                pid_arr[i].id = 0;
                pid_arr[i].crtime = -1;
                flag=1;
            }
        }
        if(flag==1){
          break;
        }
        else{
          i++;
        }
    }
}

void exec(char* parts[], int index, int bg, char* home) // FG ansd BG processes
{
    int pid = fork();
    curr_pid = pid;

    if(pid == -1)
    {
        perror("Fork Error");
    }
    else if(pid == 0) // Child process
    {
        if(bg)
        {
            signal(SIGINT, SIG_DFL);
            signal(SIGQUIT, SIG_DFL);
            signal(SIGTTIN, SIG_DFL);
            signal(SIGTTOU, SIG_DFL);
            signal(SIGCHLD, SIG_DFL);
            setpgid(getpid(), pid);
        }
        if(!strcmp(parts[0], "pwd")){
          char pwd[10001];
          getcwd(pwd,sizeof(pwd));
          printf("\n%s\n",pwd);
        }
        else if(!strcmp(parts[0], "ls")){
          printf("\n");
          ls(home, parts, index);
        }
        else{

          int err_check = execvp(parts[0], parts);
          if(err_check != 0 && index == 1)
          {
            printf("%s: command not found\n", parts[0]);
            return;
          }
          else{
            perror(parts[0]);
            return;
          }
        }
    }

    else
    {
        if(bg == 0)
        {
            waitpid(pid, NULL, WUNTRACED);
        }

        else if(bg == 1)
        {
          int i=0;
          int flag=0;

          while(i<2048)
            {
                if(pid_arr[i].id==0)
                {
                    pid_arr[i].id = pid;
                    pid_arr[i].crtime = time_counter++;
                    strcpy(pid_arr[i].name, parts[0]);
                    flag=1;
                }
                if(flag == 1){
                  break;
                }
                else{
                  i++;
                }
            }
        }
    }
    return;
}

void bg(int count, char* parts[])
{
    if(count!=2)
    {
        printf("bg: Usage: bg <jobnumbber>\n");
    }
    else{

    int i=0;
    int len1 = strlen(parts[1]);
    while(i<len1)
    {
        if(!isdigit(parts[1][i]))
        {
            printf("bg: Usage: bg <jobnumbber>\n");
            return;
        }
        i++;
    }

    qsort(pid_arr, 2048, sizeof(pid_det), proc_comp);

    char** left;
    long int jobnumb = strtol(parts[1], left, 10);
    long int jobn = jobnumb-1;

    if(!jobnumb || !pid_arr[jobn].id || jobnumb > 2048 )
    {
        printf("bg: invalid job number\n");
    }
    else{

      printf("[+] %d %s\n", pid_arr[jobn].id, pid_arr[jobn].name);
      kill(pid_arr[jobn].id, SIGCONT);
    }
  }
}

void fg(int count, char* parts[])
{
  char** left;
  int fg_pid;
  int status;

    if(count!=2)
    {
        printf("fg: Usage: fg <jobnumbber>\n");
    }
    else{

    int i=0;
    int len1=strlen(parts[1]);

    while(i<len1)
    {
        if(!isdigit(parts[1][i]))
        {
            printf("fg: Usage: fg <jobnumbber>\n");
            return;
        }
        i++;
    }

    qsort(pid_arr, 2048, sizeof(pid_det), proc_comp);

    long int jobnumb = strtol(parts[1], left, 10);
    int j = jobnumb - 1;

    if(!jobnumb || !pid_arr[j].id || jobnumb > 2048 )
    {
        printf("fg: invalid job number\n");
    }
    else{
      fg_pid = pid_arr[j].id, status;
      curr_pid =  pid_arr[j].id;

      pid_arr[j].crtime = INT_MAX;
      pid_arr[j].id = 0;

      kill(fg_pid, SIGCONT);
      waitpid(fg_pid, &status, WUNTRACED);
    }
  }
}

void unsetenvv(char **parts,int count){

  if(count==1)
  {
    printf("Arguments expected: >= 1");
    return;
  }
    unsetenv(parts[1]);
}

void setenvv(char **parts, int count)
{
  if(count==0)
  {
    printf("Arguments expected: [1,2]");
    return;
  }
  if(count>3)
  {
    printf("Too many arguments");
    return;
  }
  if(count!=2)
    {
      setenv(parts[1] , parts[2] , 2);
    }
  else
    {
      setenv(parts[1] , " " , 2);
    }
}

void kjob(int count, char* parts[])
{
    if(count <= 2){
      printf("Too few arguments!\n");
      printf("kjob: Usage: kjob <jobnumbber> <signal_number>\n");
    }
    else if(count>3){
      printf("Too many arguments\n");
      printf("kjob: Usage: kjob <jobnumbber> <signal_number>\n");
    }
    else{
      int len1 = strlen(parts[1]);
      int i=0;
      while(i<len1)
      {
          if(!isdigit(parts[1][i]))
          {
              printf("kjob: Usage: kjob <jobnumbber> <signal_number>\n");
              return;
          }
          i++;
      }

      int len2 = strlen(parts[2]);
      i=0;
      while(i<len2)
      {
          if(!isdigit(parts[2][i]))
          {
              printf("kjob: Usage: kjob <jobnumbber> <signal_number>\n");
              return;
          }
          i++;
      }

      char** left;
      long int jobnumb = strtol(parts[1], left, 10);
      long int job_sig = strtol(parts[2], left, 10);

      qsort(pid_arr, 2048, sizeof(pid_det), proc_comp);

      if(!jobnumb || !pid_arr[jobnumb-1].id || jobnumb > 2048)
      {
          printf("kjob: invalid job number\n");
      }
      else{
        kill(pid_arr[jobnumb-1].id, job_sig);
      }
    }
}

void jobs(char ** str)
{
    qsort(pid_arr, 2048, sizeof(pid_det), proc_comp);
    int j = 0;
    int i = 0;
    int fd;
    int flag=0;

    char pidval[20];
    char* path = (char*) malloc(4096);
    char* filebuff = (char*) malloc(16384);

    while(i<2048)
	{
        if(pid_arr[i].id != 0)
        {
            j++;
            printf("[%d] ",j);

            sprintf(pidval,"%d", pid_arr[i].id);

            strcpy(path, "/proc/");
            strcat(path, pidval);
            strcat(path, "/stat");

            fd = open(path,O_RDONLY);
            read(fd, filebuff, 16384);

            char* split = strtok(filebuff, " \n\t");

            for(int k=0; split; k++)
            {
                if(k==2)
                {
                    char* st[1];
                    st[0] = split;

                    if(!strcmp(st[0],"R") || !strcmp(st[0],"D") || !strcmp(st[0],"S"))
                        printf("running ");
                    else if(!strcmp(st[0],"Z"))
                        printf("zombie ");
                    else if(!strcmp(st[0],"T"))
                        printf("stopped ");

                    flag=1;
                }

                if(flag == 1){
                  break;
                }
                else{
                  split = strtok(NULL, " \n\t");
                }
            }
            printf("%s [%d]\n", pid_arr[i].name, pid_arr[i].id);
        }
        i++;
	}
}

void okill()
{
  int i=0;
    while(i < 2048){
      if(pid_arr[i].id)
      {
          kill(pid_arr[i].id, 9);
          pid_arr[i].crtime = -1;
          pid_arr[i].id = 0;
      }
      i++;
    }
    printf("overkill: Overkill successful\n");
}
