#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>
#include <stdlib.h>
#include <ncurses.h>

int nightswatchWatch(char **, int*);
void nwDirty(int);

int nightswatch(char **parts, int count)
{
	if(count == 1)
	{
		fprintf(stderr,"nightswatch: Use nightswatch [options] <valid command>\n");
		return 0;
	}

	int opt;
	int index = 0;
	int p = 63;
	int q = 48;
	int r = 57;
	int i=0; char c;
	int time_int = 2;

	while( (opt = getopt(count, parts, "n")) != -1 )
		if(opt == 'n')index = optind;

	if(index)
	{
		while(i<strlen(parts[index]))
		{
			c = parts[index][i];
			if(!(c >= q))
			{
				if(!(c <= r)){
					fprintf(stderr,"nightswatch: failed argument: '%s'\n", parts[index] );
					return 0;
				}
			}
			i++;
		}
		time_int =  atoi(parts[index]);
	}
	if(index + 1 == count)
	{
		fprintf(stderr,"nightswatch: Use nightswatch [options] <valid command>\n");
		return 0;
	}

	if(!strcmp(parts[index + 1], "dirty"))
	{
		nwDirty(time_int);
	}
	else
	{
		fprintf(stderr,"nightswatch: Use nightswatch [options] <valid command>\n");
		return 0;
	}

	optind = 1;
}
void nwDirty(int time_int)
{
	char label[200];
	sprintf(label, "\t%s\tTime specified = %d s","NIGHTSWATCH -- dirty",time_int);

	WINDOW* curr = initscr();
	WINDOW * win;
	win = newwin(800,600,1,1);
	keypad(win, TRUE);
	noecho();
	curs_set(0);
	nodelay(win,1);

	char c;
	int k = 5;
	int start = time(NULL) , current,prevcurrent = time(NULL);
	unsigned long bufsize = 0;
	char *buffer;

	FILE * fd;

	while(1)
	{
		if(wgetch(win) == 'q')
		{
			wclear(win);
			noecho();
			curs_set(1);
			delwin(win);
			endwin();
			return;
		}
		current = time(NULL);

		mvwaddstr(win,1,10,label);
		int max = 17;
		if((current- start)%time_int == 0)
		{
			if(current!=prevcurrent){
				fd = fopen("/proc/meminfo", "r");
				prevcurrent = current;
				k += 1;
				fseek(fd, 0, SEEK_SET);
				for(int i=0;i<max;i++)
					getline(&buffer, &bufsize, fd);
				mvwaddstr(win, k, 10, buffer);
				fclose(fd);
			}
		}
		int p = 25;
		if(k>p)
		{
			wclear(win);
			k=5;
		}
		wrefresh(win);
	}
	noecho();
	curs_set(1);
	delwin(win);
	endwin();
}
