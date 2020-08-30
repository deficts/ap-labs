#include <stdio.h>
#include <string.h>

int main(){
    int i = 0;
    char str[256] = "";
    char ch;
    while((ch = getchar()) != EOF){
        if(ch == '\n'){
            for(int j = strlen(str); j >=0 ;j--){
                printf("%c",str[j]);
            }
            printf("\n");
            memset(str, 0, sizeof str);
            i=0;
            continue;
        }
        str[i++] = ch;
    }
}
