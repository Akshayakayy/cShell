#include "shell.h"

void cd(char *home, char **argarr, int count)
{
  if(count > 1)
  {
    if(argarr[1][0] == '~')
    {
    	int i;
  	char temp[1005];

      for(i=1; argarr[1][i]; i++)
        temp[i-1]=argarr[1][i];

      int appendlen = i-1;

      for(i=0; home[i]; i++)
        argarr[1][i]=home[i];

      int homelen = i;

      for(i=0; i<appendlen; i++)
        argarr[1][homelen+i]=temp[i];

      argarr[1][homelen+appendlen] = '\0';
    }

    if(argarr[1][0])
    {
      if(chdir(argarr[1]) != 0)
      {
        char errstring[1005];
        strcpy(errstring, "cshell: cd: ");
  	    strcat(errstring, argarr[1]);
        perror(errstring);
      }
    }
  }
  else{
    chdir(home);
  }
}
