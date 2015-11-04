#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int Search(char* currPath, char* target, char* result, int n);

int main(int argc, char *argv[], char *envp[])
{
    char startPath[255];
    strcat(startPath, argv[1]);
    strcat(startPath, "/");
    //printf("starting at %s\n", startPath);
    char result[255] = "";
    
    if(Search(startPath, argv[3], result, atoi(argv[2])))
    {
        printf("found at %s\n", result);
    }
    else
    {
        printf("file not found\n");
    }
    
    return 0;
}

int Search(char* currPath, char* target, char* result, int n)
{
    if (n < 0)
    {
        return 0;
    }
       
    //printf("Starting to scan %s\n", currPath);
    DIR* currDir = opendir(currPath);
     
    struct dirent* direntRes = NULL;
    while(direntRes = readdir(currDir))
    {
        struct stat* buf = (struct stat*)malloc(sizeof(struct stat));
        char* pathname = (char*)calloc(255, sizeof(char));
        
        strcpy(pathname, currPath);
        strcat(pathname, direntRes->d_name);
        
        stat(pathname, buf);
        if(S_ISREG(buf->st_mode))
        {
            //printf("checking %s\n", pathname);
            if(!strcmp(target, direntRes->d_name))
            {
                strcpy(result, pathname);
                free(buf);
                free(pathname);
                return 1;
            }
        }
        if(S_ISDIR(buf->st_mode) && strcmp(direntRes->d_name, ".") && strcmp(direntRes->d_name, ".."))
        {
            strcat(pathname, "/");
            if(Search(pathname, target, result, n-1))
            {   
                free(buf);
                free(pathname);
                return 1;
            }
        }
        free(buf);
        free(pathname);
    }
    return 0;
}
