#include "logger.h"
#include "procfs.h"
#include <string.h>
#include "util.h"
#include <sys/sysinfo.h>
#include <stdlib.h>

int pfs_hostname(char *proc_dir, char *hostname_buf, size_t buf_sz)
{
    int fd = open_path(proc_dir, "sys/kernel/hostname");
    if (fd <= 0)
    {
        perror("open_path");
        return -1;
    }
    one_lineread(fd, hostname_buf, buf_sz);

    int ret = close(fd);
    if (ret == -1)
    {
        perror("close");
    }
    return 0;
}

int pfs_kernel_version(char *proc_dir, char *version_buf, size_t buf_sz)
{
    int fd = open_path(proc_dir, "sys/kernel/osrelease");
    if (fd <= 0)
    {
        perror("open_path");
        return -1;
    }
    one_lineread(fd, version_buf, buf_sz);
    size_t dash_pos = strcspn(version_buf, "-");
    version_buf[dash_pos] = '\0';
    return 0;
}

int pfs_cpu_model(char *proc_dir, char *model_buf, size_t buf_sz)
{
    int fd = open_path(proc_dir, "cpuinfo");
    if (fd <= 0)
    {
        perror("open_path");
        return -1;
    }

    size_t line_sz = 0;
    char line[256] = {0};
    while ((line_sz = lineread(fd, line, 256)) > 0)
    {
        int tokens = 0;
        char *next_tok = line;
        char *curr_tok;
        while ((curr_tok = next_token(&next_tok, "\n\t:")) != NULL)
        {
            if (strcmp(curr_tok, "model name") == 0)
            {
                curr_tok = next_token(&next_tok, "\n\t:");
                printf("Token %02d: '%s'\n", tokens++, curr_tok);
                strncpy(model_buf, curr_tok + 1, 128);
            }
        }
    }

    close(fd);

    return 0;
}

int pfs_cpu_units(char *proc_dir)
{
    int units = 0;
    int fd = open_path(proc_dir, "stat");
    if (fd <= 0)
    {
        perror("open_path");
        return 0;
    }
    size_t line_sz = 0;
    char line[1000] = {0};
    while ((line_sz = lineread(fd, line, 256)) > 0)
    {
        
        char *next_tok = line;
        char *curr_tok;
        while ((curr_tok = next_token(&next_tok, "\n\t:")) != NULL)
        {
            if (strncmp(curr_tok, "cpu", 3) == 0)
            {
                // curr_tok = next_token(&next_tok, "\n\t:");
                units++;
            }
        }
    }
            printf("units: %d: \n", units-1);
    
    return units-1;
}

double pfs_uptime(char *proc_dir)
{
    int fd = open_path(proc_dir, "uptime");
    if (fd <= 0)
    {
        perror("open_path");
        return 0;
    }
    // int uptime_found;
    size_t line_sz = 1000;
    char line[line_sz];
    one_lineread(fd, line, line_sz);
    printf(" this is just line: %s\n", line);
    double test= atof(line);
    printf(" this is atof %f\n", test);
    char *buf = malloc(sizeof(char) * 1024);  
     pfs_format_uptime(atof(line),  buf);
    
    return 0.0;
    
}

int pfs_format_uptime(double time, char *uptime_buf)
{
    
    // double day = ceil(time)/ 60 / 60 / 24;
    // double hour = ceil(time)/60/ 60 % 24;
    // double min = ceil(time)/60 % 60;
    // double sec = ceil(time)%60; // remember to add%
    // if (day == 0 && hour == 0 && min == 0)
    // {
    //     printf("%02d", sec);
    //     printf(" seconds");
    // }
    // else if (day == 0 && hour == 0)
    // {
    //     printf("%d", min);
    //     printf(" minutes");
    //     printf("%02d", sec);
    //     printf(" seconds");
    // }
    // else if (day == 0)
    // {
    //     printf("%02d", hour);
    //     printf(" hour");
    //     printf("%02d", min);
    //     printf(" minutes");
    //     printf("%02d", sec);
    //     printf(" seconds");
    // }
    // else
    // {
    //     printf("%02d", day);
    //     printf(" day");
    //     printf("%02d", hour);
    //     printf(" hour");
    //     printf("%02d", min);
    //     printf(" minutes");
    //     printf("%02d", sec);
    //     printf(" seconds");
    // }

    return 0;
}

struct load_avg pfs_load_avg(char *proc_dir)
{
    //     int counter = 0;
    struct load_avg lavg = {0};
    //     double * pch;
        int fd = open_path(proc_dir, "loadavg");
            if(fd <= 0){
                perror("open_path");
                return lavg;
    }
    size_t line_sz = 0;
    char line[1000] = {0};
    while ((line_sz = lineread(fd, line, 256)) > 0)
    {   
        int counter=0;
        char *next_tok = line;
        char *curr_tok;
        while ((curr_tok = next_token(&next_tok, "' '")) != NULL)
        {   if(counter==0){
                        lavg.one = *(curr_tok);
                        counter++;
        }
        if(counter==1){
                        lavg.five = *(curr_tok);
                        counter++;
        }
        if(counter==2){
                        lavg.fifteen = *(curr_tok);
                        counter++;
        }
        }
    }

    return lavg;
}

double pfs_cpu_usage(char *proc_dir, struct cpu_stats *prev, struct cpu_stats *curr)
{
    float load;
    int fd = open_path(proc_dir, "loadavg");
    if (fd <= 0)
    {
        perror("open_path");
        return -1;
    }
    char fb[1024]; //max lol
    read(fd, fb, sizeof(fb));
    sscanf(fb, "%f", &load);
    close(fd);
    return (int)load * 100;
}

struct mem_stats pfs_mem_usage(char *proc_dir)
{
    struct mem_stats mstats = {0};
    return mstats;
}

struct task_stats *pfs_create_tstats()
{
    return NULL;
}

void pfs_destroy_tstats(struct task_stats *tstats)
{
}

int pfs_tasks(char *proc_dir, struct task_stats *tstats)
{
    return -1;
}
