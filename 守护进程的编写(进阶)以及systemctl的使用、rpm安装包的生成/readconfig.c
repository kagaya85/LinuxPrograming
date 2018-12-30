#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int readConfig(char *fileName){
    char buf[100];
    const char *split = "=";
    char *p;
    int childNum;
    FILE* fp = fopen(fileName, "r");
    
    if(fp == NULL){
        fprintf(stderr, "open file error\n");
        exit(-1);
    }
    if(fscanf(fp, "%s", buf) == EOF){
        fprintf(stderr, "read file error\n");
        exit(-1);
    }
    
    p = strtok(buf, split);
    if(p = strtok(NULL, split))
        childNum = atoi(p);
    
    fclose(fp);
    
    if(childNum < 5 || childNum > 20)
        childNum = 5;

    return childNum;
}