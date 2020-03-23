#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <limits.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <syscall.h>
#include <time.h>
#include <unistd.h>
#include <getopt.h>
#include <ncurses.h>

typedef struct {
    int id;
    int crtime;
    char name[256];
    int status;
} pid_det;

int proc_comp(const void *a, const void *b);
void backproc();
void exec(char* parts[], int index, int bg, char* home);
void bg(int count, char* parts[]);
void fg(int count, char* parts[]);
void unsetenvv(char **parts,int count);
void setenvv(char **parts, int count);
void kjob(int count, char* parts[]);
void jobs(char ** str);
void okill();
void redirect(char* parts[], int count,  char* home, char *hist[], int current);
void functions(char* parts[], int index, int background, char* home, char *hist[], int current);
void sigstp_handler();
void sigint_handler(int sig);
const char* get_process_name_by_pid(const int pid);
void init();
int proc_comp(const void *a, const void *b);
void cd(char *home, char **argarr, int count);
void history(char *hist[], int current, char **argarr, int count);
void ls(char* home, char *parts[], int index);
void lcmd(char *directory, char *filename);
int nightswatch(char **parts, int count);
void pinfo(char *home, char **argarr, int cnt);
