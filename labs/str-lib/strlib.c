#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int mystrlen(char *str){
    int i;
    for(i = 0; str[i]!='\0'; i++);
    return i;
}

char *mystradd(char *origin, char *addition){
    char *newStr = malloc(mystrlen(origin)+mystrlen(addition)+1);
    strcpy(newStr,origin);
    strcat(newStr,addition);
    return newStr;
}

int mystrfind(char *origin, char *substr){
    int originLen = mystrlen(origin);
    int subLen = mystrlen(substr);
    for(int i = 0; i<originLen; i++){
        if(origin[i] == substr[0]){
            int j, k;
            for(j = i, k = 0; j<originLen && k<subLen; j++,k++){
                if(origin[j] != substr[k]){
                    break;
                }
            }
            if(k==subLen){
                return i;
            }
        }
    }
    return -1;
}
