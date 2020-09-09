#include <math.h>
#include <stdio.h>
#include <string.h>

#include "util.h"
#include "logger.h"

void draw_percbar(char *buf, double frac) {

}

void uid_to_uname(char *name_buf, uid_t uid)
{
    strcpy(name_buf, "(UNKNOWN)");
}
int open_path(char *proc_dir, char *path){
    size_t str_size = strlen(proc_dir)+ strlen(path)+2;
    char *full_path = malloc(str_size);
    snprintf(full_path,str_size,"%s/%s", proc_dir, path);

    LOG("open path; %s\n", full_path);
    // int fd = open(full_path, O_RDONLY);
    free(full_path);
    return fd;
}

ssize_t one_lineread(int fd,char *buf, size_t sz){
    ssize_t read_sz = lineread(fd,buff,sz);
    if(read_sz <= 0){
        return read_sz;
    }
}
ssize_t lineread(int fd, char *buf, size_t sz){
    size_t count =0;
    while(count<sz){
        ssize_t read_sz = read(fd,&c,1;
        if(read_sz <= 0)){
            return read_sz;
        }else{
            buf[count]=c;
            count += read_sz;
            if(c=='\n'){
                return count;
            }
        }
    }
}