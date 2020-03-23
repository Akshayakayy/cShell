#include "shell.h"

void init();
void ls(char* home, char *argv1[], int argc1);
void cd(char *home, char **argarr, int count);
void pinfo(char *home, char **argarr, int cnt);
void history(char *hist[], int current, char **argarr, int count);
void exec(char* argv1[], int argc1, int bg, char* home);

#define HISTORY_COUNT 20

int curr_pid, shell_pid;
extern pid_det pid_arr[3001];
extern int time_counter;

int pipe_array[2];

int proc_comp(const void *a, const void *b)
{
    pid_det *ia = (pid_det*)a;
    pid_det *ib = (pid_det*)b;
    return ia->crtime - ib->crtime;
}

void init()
{
  int i=0;
  while(i < 3001)
    {
        pid_arr[i].id = 0;
        pid_arr[i].crtime = INT_MAX;
        i++;
    }
}

const char* get_process_name_by_pid(const int pid)
{
    char* name = (char*)malloc(1024);
    if(name)
    {
        sprintf(name, "/proc/%d/cmdline",pid);
        FILE* f = fopen(name,"r");
        if(f)
        {
            size_t size;
            size = fread(name, sizeof(char), 1024, f);
            if(size>0)
            {
                if('\n'==name[size-1])
                    name[size-1]='\0';
            }
            fclose(f);
        }
    }
    return name;
}

void sigint_handler(int sig)
{
    if(getpid()!=shell_pid)
        return;

    if(curr_pid != -1)
        kill(curr_pid, SIGINT);

    signal(SIGINT, sigint_handler);
}

void sigstp_handler()
{
    if(getpid()!=shell_pid)
        return;

	if(curr_pid != -1)
    {
		kill(curr_pid, SIGTTIN);
		kill(curr_pid, SIGTSTP);
        int i;
        for(i=0; i<2048; ++i)
        {
            if(pid_arr[i].id==0)
            {
                pid_arr[i].id = curr_pid;
                pid_arr[i].crtime = time_counter++;
                strcpy(pid_arr[i].name, get_process_name_by_pid(curr_pid));
                break;
            }
        }

        printf("[+] stopped %d %s\n" , curr_pid, pid_arr[i].name);
	}
    signal(SIGTSTP, sigstp_handler);
}

int main()
{
  char *hist[HISTORY_COUNT];
  int current = 0;

  for (int i = 0; i < HISTORY_COUNT; i++) hist[i] = NULL;

  init();
  curr_pid = -1;

  char* home = (char*) malloc (1005);
  getcwd(home,1005);
  int homelen = strlen(home);

  char *user = getenv("USER");
  char* hostname = (char*) malloc (1005);
  gethostname(hostname,sizeof(hostname));

  char path[1005], presentPath[1005];

  shell_pid = getpid();
  setpgid(shell_pid, shell_pid);
  tcsetpgrp(0, shell_pid);

  signal(SIGINT, sigint_handler);
  signal(SIGQUIT, SIG_IGN);
  signal(SIGTSTP, sigstp_handler);

  while(1){
    curr_pid = -1;
    backproc();

    getcwd(presentPath,sizeof(presentPath));

    //to print the path relative to home
    int flag = 0;
    int i = 0;
    for(;i<homelen; i++)
    {
        if(presentPath[i] != home[i]){
          strcpy(path, presentPath);
          flag = 1;
          break;
        }
    }
    if(flag == 0){
      path[0] = '~';
      for(;i<strlen(presentPath);i++)
          path[i-homelen+1] = presentPath[i];
      path[i-homelen+1] = '\0';
    }

    printf("<%s%s@%s%s:%s%s%s>","\x1B[1;33m",user,hostname,"\x1B[0m","\x1B[1;36m",path,"\x1B[0m");

    //to read input
    char* command;
    size_t bufsize = 0;
    getline(&command, &bufsize, stdin);

    if(!strcmp(command,"\n")) continue;

    //splitting commands
    char **commands = malloc(1000 * sizeof(char*));

    int index = 0;
    char* token = strtok(command, ";");
    while (token != NULL) {
        commands[index] = token;
        index++;
        token = strtok(NULL, ";");
    }
    commands[index] = NULL;
    if(!commands[0])
        continue;

    for(i=0;commands[i]!=NULL;i++){
      char **parts = malloc(100 * sizeof(char*));

      hist[current] = strdup(commands[i]);
      current = (current + 1) % HISTORY_COUNT;

      // PIPE PROCESSING
      index = 0;
      int pipe_flag = 0;
      token = strtok(commands[i], "|\n");
      while (token != NULL) {
          parts[index] = token;
          index++;
          token = strtok(NULL, "|\n");
      }
      parts[index] = NULL;
      if(!parts[0])
          continue;

      if(index > 1)
      {
        int pdarr[2];
        pdarr[0] = 0;
        pdarr[1] = 1;

        int stdarr[2];
        stdarr[0]  = dup(0);
        stdarr[1] = dup(1);

        int count=index;
        int i=0;
        while(i<count)
        {
            int wordc = 0;
            char* cmd[4096];
            char* token = strtok(parts[i], " \t\n");
            while (token != NULL) {
                cmd[wordc] = token;
                wordc++;
                token = strtok(NULL, " \t\n");
            }
            cmd[wordc] = NULL;

            if(dup2(pdarr[0], 0) == -1)
            {
                perror("PipeDup2Error");
                return;
            }

            if(pipe(pipe_array) != 0)
                perror("pipe_error");

            close(pdarr[0]);

            dup2(pipe_array[1], 1);
            close(pipe_array[1]);

            if(i == count - 1)
            {
                if(dup2(stdarr[1], 1) == -1)
                {
                    perror("PipeDup2Error");
                    return;
                }
            }

            int redirf = 0;
            for (int q = 0; q < wordc; q++)
            {
                if(!strcmp(cmd[q], ">")|| !strcmp(cmd[q], "<") || !strcmp(cmd[q], ">>"))
                {
                    redirf = 1;
                    redirect(cmd, wordc, home, hist, current);
                    break;
                }
            }

            if(redirf)
            {
                pdarr[0] = pipe_array[0];
                continue;
            }

            int background = 0;                              // Flag for background
            if(!strcmp(cmd[wordc-1], "&"))
            {
                background = 1;
                strcpy(cmd[wordc-1], "\0");
            }

            // functions(cmd, wordc, background, home, hist, current);

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
              history(hist, current, cmd, wordc);
            // else if(!strcmp(cmd[0], "echo") && background != 1)
            //   echo(wordc, cmd);
            else if(!strcmp(cmd[0], "ls") && background != 1){
              ls(home, cmd, wordc);
            }
            else if(!strcmp(cmd[0], "cd")){
              cd(home, cmd, wordc);
            }
            else if(!strcmp(cmd[0], "pinfo"))
              pinfo(home, cmd, wordc);
            else if(!strcmp(cmd[0], "nightswatch"))
              nightswatch(cmd, wordc);
            else
                exec(cmd, wordc, background, home);

            pdarr[0] = pipe_array[0];
            i++;
        }
        dup2(stdarr[0], 0);
        close(stdarr[0]);
        dup2(stdarr[1], 1);
        close(stdarr[1]);
      }
      else{
        index = 0;
        token = strtok(commands[i], " \t\n");
        while (token != NULL) {
            parts[index] = token;
            index++;
            token = strtok(NULL, " \t\n");
        }
        parts[index] = NULL;
        if(!parts[0])
            continue;

        // REDIRECTION PROCESSING
        int redir_flag = 0;
        for (int i = 0; i<index; i++)
        {
            if(!strcmp(parts[i], "<")|| !strcmp(parts[i], ">") || !strcmp(parts[i], ">>"))
            {
                redir_flag = 1;
                redirect(parts, index, home, hist, current);
                break;
            }
        }

        if(redir_flag) continue;

        int background = 0;
        if(!strcmp(parts[index-1], "&"))
        {
            background = 1;
            strcpy(parts[index-1], "\0");
            index = index-1;
        }

        functions(parts, index, background, home, hist, current);
      }

    }
  }

  return 0;
}
