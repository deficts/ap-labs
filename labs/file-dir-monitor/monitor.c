#include <stdio.h>
#include <stdlib.h>
#include "logger.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ftw.h>
#include <sys/inotify.h>
#include <string.h>

#define FDMAX 100
#define MSKSLEN 3
#define NAMEMAX 1000
#define BUFLEN (10 *(sizeof(struct inotify_event) + NAMEMAX + 1))

int ifd, wd;

struct wds {
    char name[NAMEMAX];
    int wd;
};

struct wds wd_structs[(10 * (sizeof(struct wds) + NAMEMAX + 1))];
int wd_index = 0;
int wd_size = 0;

void createWdStruct(const char * name, int watchDescriptor){
    struct wds wd_struct;
    strcpy(wd_struct.name, name);
    wd_struct.wd = wd;
    wd_structs[wd_index++] = wd_struct;
    wd_size++;
}

static int add_watcher(const char *filePath, const struct stat *sb, int type){
    if(type != FTW_D){
        return 0;
    }

    int watchDescriptor;
    if((watchDescriptor = inotify_add_watch(ifd, filePath, IN_CREATE | IN_DELETE | IN_MODIFY)) == -1){
        errorf("Error: failed creating watch descriptor.");
        return -1;
    }
   createWdStruct(filePath, watchDescriptor);
    return 0;
}

char * findWatchDescriptor(int wd) {
    int i;
    for (i = 0; i < wd_size; i++) {
        if (wd_structs[i].wd == wd) {
            return wd_structs[i].name;
        }
    }

    return NULL;
}

static void printEvent(struct inotify_event *i) {
    infof("wd: %d detected event: %s in file %s", i->wd, 
        i->mask & IN_CREATE ? "IN CREATE" : i->mask & IN_DELETE ? "IN DELETE" :
        i->mask & IN_MODIFY ? "IN MODIFY" : "???", i->name);   
}

int main(int argc, char** argv){
    if(argc < 2){
        errorf("Error: usage ./monitor <path>");
        exit(1);
    }

    const char *path;
    char buf[BUFLEN];
    ssize_t nrd;
    char *p;
    struct inotify_event *event;

    infof("Init inotify");
    path = argv[1];
    ifd = inotify_init();
    
    if(ifd == -1){
        errorf("Error: failure at the inotify initialization");
        exit(1);
    }

    if(ftw(path, add_watcher, FDMAX) == -1){
        errorf("Error: failed at traversing the file tree.");
        exit(1);
    }

    for(;;){
        if((nrd = read(ifd, buf, BUFLEN)) == 0){
            errorf("Error: failed reading inotify file descriptor.");
            exit(1);
        }

        if(nrd == -1){
            errorf("Error: failed reading inotify file descriptor.");
            exit(1);
        }

        for(p = buf; p < buf + nrd;){
            event = (struct inotify_event * ) p;
            printEvent(event);
            p += sizeof(struct inotify_event) + event->len;
        }
    }

    return 0;
}