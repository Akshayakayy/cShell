#include "shell.h"

void ls(char* home, char *parts[], int index)
{
	int l = 0;
	int a = 0;
	int dirC = 0;
	char dirList[dirC][501];

    for(int i = 1; i<index; i++)
    {
        if(!strcmp(parts[i],"-l"))
            l = 1;
        else if(!strcmp(parts[i],"-a"))
            a = 1;
        else if(!strcmp(parts[i],"-la") || !strcmp(parts[i],"-al"))
        {
            l = 1;
            a = 1;
        }
		else
			dirC++;
	}

	if(dirC == 0)
	{
		dirC++;
		strcpy(dirList[0],".");
	}

	int j = 0;
	int i = 1;
	while(i < index)
	{
		if(!strcmp(parts[i],"~"))
		{
			strcpy(dirList[j],home);
			j++;
		}
		else if(strcmp(parts[i], "-l") != 0 && strcmp(parts[i], "-a") != 0){
				 if(strcmp(parts[i], "-la") != 0 && strcmp(parts[i], "-al") != 0){
					 strcpy(dirList[j],parts[i]);
	 				j++;
				 }
			 }
		i++;
	}

	DIR *open_dir;
	for(int i = 0; i<dirC; i++)
	{
		open_dir = opendir(dirList[i]);
		if(!open_dir)
		{
			printf("ls: cannot access '%s': No such file or directory\n", dirList[i]);
		}
		else
		{
			printf("%s:\n", dirList[i]);
			struct dirent *file_cwd = readdir(open_dir);
			while(file_cwd != NULL)
			{
					if(l==0 && a==0 )
					{
						if(file_cwd->d_name[0] != '.')
							printf("%s   ", file_cwd->d_name);
					}
					if(l==0 && a==1 )
					{
						printf("%s   ", file_cwd->d_name);
					}
					if(l==1 && a==0 )
					{
						if(file_cwd->d_name[0] != '.')
							lcmd(dirList[i], file_cwd->d_name);
					}
					if(l==1 && a==1 )
					{
						lcmd(dirList[i], file_cwd->d_name);
					}

				file_cwd = readdir(open_dir);
			}
			printf("\n");
			closedir(open_dir);
		}
	}
}

void lcmd(char *directory, char *filename)
{
		char path[1001];
		strcpy(path,directory);
		strcat(path,"/");
		strcat(path,filename);

		struct stat fileinfo;

		int err_check = stat(path,&fileinfo);
		if(err_check < 0)
		{
			perror("Error");
		}
		else{
			char string[255];

			struct passwd *pw = getpwuid(fileinfo.st_uid);
			struct group *gr = getgrgid(fileinfo.st_gid);

			printf((S_ISDIR(fileinfo.st_mode)) ? "d" : "-");
	    printf((fileinfo.st_mode & S_IRUSR) ? "r" : "-");
	    printf((fileinfo.st_mode & S_IWUSR) ? "w" : "-");
	    printf((fileinfo.st_mode & S_IXUSR) ? "x" : "-");
	    printf((fileinfo.st_mode & S_IRGRP) ? "r" : "-");
	    printf((fileinfo.st_mode & S_IWGRP) ? "w" : "-");
	    printf((fileinfo.st_mode & S_IXGRP) ? "x" : "-");
	    printf((fileinfo.st_mode & S_IROTH) ? "r" : "-");
	    printf((fileinfo.st_mode & S_IWOTH) ? "w" : "-");
	    printf((fileinfo.st_mode & S_IXOTH) ? "x " : "- ");

	    printf("%2lu\t", fileinfo.st_nlink);
	    printf("%s\t", pw->pw_name);
	    printf("%s\t", gr->gr_name);
	    printf("%ld\t",fileinfo.st_size);
			strftime(string, 24, "\t%b %d %Y %H:%M\t", localtime(&(fileinfo.st_mtime)));
      printf("%s\t%s\n", string, filename);
		}
}
