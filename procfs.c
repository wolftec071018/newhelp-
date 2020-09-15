#include "logger.h"
#include "procfs.h"
#include <string.h>
#include "util.h"
#include <sys/sysinfo.h>
#include <stdlib.h>
    /* this takes care of out host name... opens the proc then read one line */

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
    /* this takes care of out kernel version... opens the proc then read one line */

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
    /* this takes care of out cpu model.. opens the proc then read lines till it finds model name 
    *
    */

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
/* this takes care of out cpu units. opens the proc then read lines till it finds the word cpu and then adds it to a counter. returns the counter
    *
    */
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
                units++;
            }
        }
    }
            printf("units: %d: \n", units-1);
    
    return units-1;
}
/* this takes care of out uptime. opens the proc then read one line. atof takes the 1st number and return as a double.
    *
    */
double pfs_uptime(char *proc_dir)
{
    int fd = open_path(proc_dir, "uptime");
    if (fd <= 0)
    {
        perror("open_path");
        return 0;
    }
    // int uptime_found;
    size_t line_sz = 200;
    char line[line_sz];
    one_lineread(fd, line, 100);
    double test= atof(line);
    char* ptr = line;
    char* tok= next_token(&ptr," ");
    return atof(tok);
    
}

int pfs_format_uptime(double time, char *uptime_buf)
{
    // float day= time%60/60/24;
    // double day = time/ 60 / 60 / 24;
    // double hour = time/60/ 60 % 24;
    // double min = time/60 % 60;
    // double sec = time%60; // remember to add%
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

    return 0.0;
}

struct load_avg pfs_load_avg(char *proc_dir)
{

    struct load_avg lavg = {0};

        int fd = open_path(proc_dir, "loadavg");
            if(fd <= 0){
                perror("open_path");
                return lavg;
    }
        char line[10] = {0};

    char* ptr = line;
    char* tok= next_token(&ptr," ");
    lavg.one= atof(tok);
    tok=next_token(&ptr, " ");
    lavg.five=atof(tok);
    tok=next_token(&ptr, " ");
    lavg.fifteen=atof(tok);

  

    return lavg;
}

double pfs_cpu_usage(char *proc_dir, struct cpu_stats *prev, struct cpu_stats *curr)
{
// {   int counter=0;
// int user;
// int nice;
// int system;
// int idle;
// int iowait;
// int irq;
// int softirq;
// int guest;
// int guest_nice;
// int steal;
    // int fd = open_path(proc_dir, "stat");
    // if (fd <= 0)
    // {
    //     perror("open_path");
    //     return -1;
    // }
    // size_t line_sz = 0;
    // char line[1000] = {0};
    // while ((line_sz = lineread(fd, line, 1000)) > 0)
    // {
    //     int tokens = 0;
    //     char *next_tok = line;
    //     char *curr_tok;
    //     while ((curr_tok = next_token(&next_tok, "' '\n\t:")) != NULL)
    //     {
    //         if(counter==0){
    //             user = curr_tok;
    //             counter++;
    //         }
    //         if(counter==1){
    //             nice= curr_tok;
    //             counter++;
    //         }
    //         if(counter==2){
    //             system = curr_tok;
    //             counter++;
    //         }
    //         if(counter==3){
    //             idle= curr_tok;
    //             counter++;
    //         }
    //         if(counter==4){
    //             iowait=curr_tok;
    //             counter++;
    //         }
    //         if(counter==5){
    //             irq= curr_tok;
    //             counter++;
    //         }
    //         if(counter==6){
    //             softirq =curr_tok;
    //             counter++;
    //         }
    //         if(counter==7){
    //             steal=curr_tok;
    //             counter++;
    //         }
    //         if(counter==8){
    //             guest=curr_tok;
    //             counter++;
    //         }
    //         if(counter==9){
    //             guest_nice=curr_tok;
    //             counter++;
    //         }
    //     }

    
    // }
    
    return 0;
}

struct mem_stats pfs_mem_usage(char *proc_dir)
{// total and avi
//total - avi......
    struct mem_stats mstats = {0};
    return mstats;
}

struct task_stats *pfs_create_tstats()
{//malloc and return stats.. tstats ,actStats
    return NULL;
}

void pfs_destroy_tstats(struct task_stats *tstats)
{//free the mallocs at tStats and active stats
}

int pfs_tasks(char *proc_dir, struct task_stats *tstats)
{
    return -1;
}
