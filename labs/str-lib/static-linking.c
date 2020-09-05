#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int mystrlen(char *);
char *mystradd(char *,char *);
char mystrfind(char *,char *);
int main(int argc, char* argv[]) {
    if(argc == 4){
        if(strcmp(argv[1], "-add") == 0){
            int initialLen = mystrlen(argv[2]);
            char* result = mystradd(argv[2],argv[3]);
            int newLen = mystrlen(result);
            printf("Initial Length: %d\n", initialLen);
			printf("New String: %s\n", result);
			printf("New length: %d\n", newLen);
        }else if(strcmp(argv[1], "-find") == 0){
            int result = mystrfind(argv[2],argv[3]);
            if(result == -1){
                printf("The string [%s] was not found\n",argv[3]);
            }else{
                printf("The string [%s] was found at [%d] index\n",argv[3],result);
            }
            
        }
    }else{
        printf("Error, you entered an invalid set of parameters\n");
    }
}
