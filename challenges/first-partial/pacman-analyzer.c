#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define REPORT_FILE "packages_report.txt"
#define MAX_PACKAGES 10000

char *keywords[3] = {"installed","upgraded","removed"};

struct package{
    char name[256];
    char installDate[256];
    char upgradeDate[256];
    int upgradeCount;
    char removeDate[256];
};

struct package packages[MAX_PACKAGES];
int installs = 0;
int removes = 0;
int updates = 0;
int currentInstalled = 0;

int myGetLine(FILE *file, char *buffer, size_t size);
void analizeLog(char *logFile, char *report);
void analizeLine(char line[]);
void save(char *date, int mode, char *package);
void writeResults(char *file);

int main(int argc, char **argv) {

    if (argc < 5 || strcmp(argv[1],"-input") != 0 || strcmp(argv[3],"-report") != 0) {
        printf("Error: wrong arguments\n");
        printf("Usage: ./pacman-analizer.o -input <inputName.txt> -report <reportName.txt>\n");
        return 1;
    }

    analizeLog(argv[2], argv[4]);

    return 0;
}

void analizeLog(char *logFile, char *report) {
    printf("Generating Report from: [%s] log file\n", logFile);

    FILE *file;
    file = fopen(logFile, "r");
    if(file == NULL){
        perror("Can't open file");
        exit(1);
    }

    int x; 
    char line [BUFSIZ];

    while((x = myGetLine(file, line, 255)) > 0){
        analizeLine(line);
    }

    writeResults(report);

    fclose(file);

    printf("Report is generated at: [%s]\n", report);
}

int myGetLine(FILE *file, char *buffer, size_t size){
    if (size == 0){
        return 0;
    }
    size_t currentSize = 0;
    int c;
    while ((c = (char) getc(file)) != '\n' && currentSize < size){
        if (c == EOF){
            break;
        }
        buffer[currentSize] = (char) c;
        currentSize++;
        
    }
    if (currentSize == 0){
        return 0;
    }
    buffer[currentSize] ='\0';
    return currentSize;
}

void analizeLine(char line[]){
    char bracket[10] = "]";
    char space[10] = " ";
    char *token = strtok(line,bracket);
    char date[256],package[256],mode[256];

    for(int i = 0; i<2; i++){
        if(token == NULL) return;
        if(i == 0) strcpy(date,token);
        if(token != NULL){
            token = strtok(NULL,bracket);
        }   
    }

    char *command = strtok(token,space);
    
    for(int i = 0; i<2; i++){
        if(command == NULL) return;
        if(i == 0) strcpy(mode,command);
        if(i == 1) strcpy(package,command);
        if(command != NULL){
            command = strtok(NULL,space);
        }
    }

    if(strcmp(mode,keywords[0]) == 0)save(date,0,package);
    else if(strcmp(mode,keywords[1]) == 0) save(date,1,package);
    else if(strcmp(mode,keywords[2]) == 0) save(date,2,package);
}

void save(char *date, int mode, char *package){
    for(int i = 0; i < MAX_PACKAGES; i++){
        if(strcmp(packages[i].name,"") == 0){
            if(mode == 2){
                strcpy(packages[i].name, package);
                strcpy(packages[i].installDate, "-");
                strcpy(packages[i].upgradeDate, "-");
                strcpy(packages[i].removeDate, date);
                removes++;
                currentInstalled--;
                break;
            }else if(mode == 1){
                strcpy(packages[i].name, package);
                strcpy(packages[i].installDate, "-");
                strcpy(packages[i].upgradeDate, date);
                strcpy(packages[i].removeDate, "-");
                packages[i].upgradeCount++;
                updates++;
                break;
            }else{
                strcpy(packages[i].name, package);
                strcpy(packages[i].installDate, date);
                strcpy(packages[i].upgradeDate, "-");
                strcpy(packages[i].removeDate, "-");
                installs++;
                currentInstalled++;
                break;
            }
        }
        else if(strcmp(packages[i].name, package) == 0){
            if(mode == 2){
                strcpy(packages[i].removeDate, date);
                removes++;
                currentInstalled--;
                break;
            }else if(mode == 1){
                strcpy(packages[i].upgradeDate, date);
                packages[i].upgradeCount++;
                updates++;
                break;
            }else{
                strcpy(packages[i].name, package);
                strcpy(packages[i].installDate, date);
                installs++;
                currentInstalled++;
                break;
            }
        }
    }
}

void writeResults(char *file){
    int output = open(file,O_RDWR | O_CREAT | O_TRUNC, 0644);
    if(output == -1){
        printf("Error: could not create output file\n");
        exit(EXIT_FAILURE);
    }
    char extra[10];
    write(output,"Pacman Packages Report\n",strlen("Pacman Packages Report\n"));
    write(output,"======================\n",strlen("======================\n"));
    write(output, "Installed packages : ", strlen("Installed packages : "));
    sprintf(extra, "%d\n",installs);
    write(output, extra, strlen(extra));
    write(output, "Upgraded packages  : ",strlen("Upgraded packages  : "));
    sprintf(extra, "%d\n", updates);
    write(output, extra, strlen(extra));
    write(output, "Removed packages   : ",strlen("Removed packages   : "));
    sprintf(extra, "%d\n", removes);
    write(output, extra, strlen(extra));
    write(output, "Current installed  : \n",strlen("Current installed  : "));
    sprintf(extra, "%d\n", currentInstalled);
    write(output, extra, strlen(extra));
    
    write(output, "\n\nList of packages\n", strlen("\n\nList of packages\n"));
    write(output,"----------------------\n",strlen("----------------------\n"));
    for(int i = 0; i < MAX_PACKAGES; i++){
        if(strcmp(packages[i].name, "") != 0){
            write(output, "- Package name         : ",strlen("- Package name         : "));
            write(output,packages[i].name, strlen(packages[i].name));
            write(output, "\n   - Install date      : ",strlen("\n   - Install date      : "));
            write(output,packages[i].installDate, strlen(packages[i].installDate));
            write(output, "\n   - Last update date  : ",strlen("\n   - Last update date  : "));
            write(output,packages[i].upgradeDate, strlen(packages[i].upgradeDate));
            write(output, "\n   - How many updates  : ",strlen("\n   - How many updates  : "));
            sprintf(extra, "%d", packages[i].upgradeCount);
            write(output,extra, strlen(extra));
            write(output, "\n   - Removal date      : ",strlen("\n   - Removal date      : "));
            write(output,packages[i].removeDate, strlen(packages[i].removeDate));
            write(output, "\n",strlen("\n"));
        } else if (strcmp(packages[i].name, "") == 0){
            break;
        }
    }


    if (close(output) < 0)  
    { 
        perror("Error: could not close the output file\n"); 
        exit(1); 
    } 
}