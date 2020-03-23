#include "shell.h"

#define HISTC 20

void history(char *hist[], int current, char **argarr, int count)
{
        if(count > 2){
          printf("history: Usage: history [num]\n");
          return;
        }
        else if(count == 1){
          int i = current;
          int max = 10;

          if(max>20){
            max = 20;
          }

          char* temp[20];
          int index = 0;

          do {
                  if (hist[i]) {
                          temp[index] = hist[i];
                          index = (index + 1) % max;
                  }

                  i = (i + 1) % HISTC;

          } while (i != current);

          i = index;
          do{
            if(temp[i]){
              printf("%s", temp[i]);
            }
            i = (i + 1) % max;
          }while (i != index);
        }
        else{
          int i = current;
          int max = atoi(argarr[1]);

          if(max>20){
            max = 20;
          }

          char* temp[20];
          int index = 0;

          do {
                  if (hist[i]) {
                          temp[index] = hist[i];
                          index = (index + 1) % max;
                  }

                  i = (i + 1) % HISTC;

          } while (i != current);

          i = index;
          do{
            if(temp[i]){
              printf("%s", temp[i]);
            }
            i = (i + 1) % max;
          }while (i != index);
        }

}
