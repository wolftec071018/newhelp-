#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "util.h"
#include "logger.h"

void draw_percbar(char *buf, double frac) {

}

void uid_to_uname(char *name_buf, uid_t uid)
{
    strcpy(name_buf, "(UNKNOWN)");
}
int open_path(char *proc_dir, char *path){
    if(proc_dir == NULL || path==NULL){
        errno = EINVAL;
        return -1;
    }
    size_t str_size = strlen(proc_dir)+ strlen(path)+2;
    char *full_path = malloc(str_size);
    if(full_path == NULL){
        return -1;
    }
    snprintf(full_path,str_size,"%s/%s", proc_dir, path);

    LOG("open path; %s\n", full_path);
    int fd = open(full_path, O_RDONLY);
    free(full_path);
    return fd;
}

ssize_t one_lineread(int fd,char *buf, size_t sz){
    ssize_t read_sz = lineread(fd,buf,sz);
    if(read_sz <= 0){
        return read_sz;
    }

    buf[read_sz -1] = '\0';
    return read_sz;
}


ssize_t lineread(int fd, char *buf, size_t sz)
{
    size_t count = 0;
    while(count<sz){
        char c;
        ssize_t read_sz = read(fd,&c,1);
        if(read_sz <= 0){
            return read_sz;
        }else{
            buf[count]=c;
            count += read_sz;
            if(c=='\n'){
                return count;
            }
        }
    }
    return count;
}

char *next_token(char **str_ptr, const char *delim)
{
    if (*str_ptr == NULL) {
        return NULL;
    }

    size_t tok_start = strspn(*str_ptr, delim);
    size_t tok_end = strcspn(*str_ptr + tok_start, delim);

    /* Zero length token. We must be finished. */
    if (tok_end  == 0) {
        *str_ptr = NULL;
        return NULL;
    }

    /* Take note of the start of the current token. We'll return it later. */
    char *current_ptr = *str_ptr + tok_start;

    /* Shift pointer forward (to the end of the current token) */
    *str_ptr += tok_start + tok_end;

    if (**str_ptr == '\0') {
        /* If the end of the current token is also the end of the string, we
         * must be at the last token. */
        *str_ptr = NULL;
    } else {
        /* Replace the matching delimiter with a NUL character to terminate the
         * token string. */
        **str_ptr = '\0';

        /* Shift forward one character over the newly-placed NUL so that
         * next_pointer now points at the first character of the next token. */
        (*str_ptr)++;
    }

    return current_ptr;
}