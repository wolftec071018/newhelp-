/**
 * @author Andrew Dao...and well alot was given in class
 * @version 1??? 
 * its a program to let us explore Proc in our vm
*/

#include "logger.h"
#include "procfs.h"
#include <string.h>
#include "util.h"
#include <sys/sysinfo.h>
#include <stdlib.h>
#include <math.h>
/**
 this takes care of out host name... opens the proc then read one line 
 @one_lineread basically takes a fd and than a buf then a size
*/

int pfs_hostname(char *proc_dir, char *hostname_buf, size_t buf_sz)
{
    int fd = open_path(proc_dir, "sys/kernel/hostname");
    if (fd == -1)
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
/**
this takes care of out kernel version... opens the proc then read one line
this @returns a kernal version
*/

int pfs_kernel_version(char *proc_dir, char *version_buf, size_t buf_sz)
{
    int fd = open_path(proc_dir, "sys/kernel/osrelease");
    if (fd == -1)
    {
        perror("open_path");
        return -1;
    }
    one_lineread(fd, version_buf, buf_sz);
    size_t dash_pos = strcspn(version_buf, "-");
    version_buf[dash_pos] = '\0';
    return 0;
}
/** 
 this takes care of out cpu model.. opens the proc then read lines till it finds model name 
 @returns a cup model
*
*/

int pfs_cpu_model(char *proc_dir, char *model_buf, size_t buf_sz)
{
    int fd = open_path(proc_dir, "cpuinfo");
    if (fd == -1)
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
/**
 this takes care of out cpu units. opens the proc then read lines till it finds the word cpu and then adds it to a counter. returns the counter
 @returns cpu units active?
*/
int pfs_cpu_units(char *proc_dir)
{
    int units = 0;
    int fd = open_path(proc_dir, "stat");
    if (fd == -1)
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
    printf("units: %d: \n", units - 1);

    return units - 1;
}
/**
   this takes care of out uptime. opens the proc then read one line. atof takes the 1st number and return as a double.
   @return uptime
 */
double pfs_uptime(char *proc_dir)
{
    int fd = open_path(proc_dir, "uptime");
    if (fd == -1)
    {
        perror("open_path");
        return 0;
    }
    // int uptime_found;
    size_t line_sz = 200;
    char line[line_sz];
    one_lineread(fd, line, 100);
    char *ptr = line;
    char *tok = next_token(&ptr, " ");
    return atof(tok);
}
/**
* this formats the seconds given to us. makes it into days hours mins and seconds.
 @returns uptimes
*/
int pfs_format_uptime(double time, char *uptime_buf)
{

    double year = time / 31556952;
    time = (int)time % 31556952;
    double day = (int)time / 86400;
    time = (int)time % 86400;
    double hour = time / 3600;
    time = (int)time % 3600;
    double min = time / 60;
    time = (int)time % 60;
    double sec = time;
    if ((int)year == 0 && (int)day == 0 && (int)hour == 0 && (int)min == 0)
    {
        snprintf(uptime_buf, 100, "%d seconds", (int)sec);
    }
    else if ((int)year == 0 && (int)day == 0 && (int)hour == 0)
    {
        snprintf(uptime_buf, 100, "%d minutes, %d seconds", (int)min, (int)sec);
    }
    else if ((int)year == 0 && (int)day == 0)
    {
        snprintf(uptime_buf, 100, "%d hours, %d minutes, %d seconds", (int)hour, (int)min, (int)sec);
    }
    else if ((int)year == 0)
    {
        snprintf(uptime_buf, 100, "%d days, %d hours, %d minutes, %d seconds", (int)day, (int)hour, (int)min, (int)sec);
    }
    else
    {
        snprintf(uptime_buf, 100, "%dyears,%d days,%d hours,%d minutes, %d seconds", (int)year, (int)day, (int)hour, (int)min, (int)sec);
    }

    return 0.0;
}

/**
* this finds the load avg basically it is reading a line and then adding it to the correct value in lavg.
 @return load avg.
*
*
*/

struct load_avg pfs_load_avg(char *proc_dir)
{

    struct load_avg lavg = {0};

    int fd = open_path(proc_dir, "loadavg");
    if (fd <= -1)
    {
        perror("open_path");
        return lavg;
    }
    char line[1000] = {0};

    char *ptr = line;
    one_lineread(fd, line, 1000);
    char *tok = next_token(&ptr, " ");
    lavg.one = atof(tok);
    printf("this is lav one %f\n", lavg.one);
    tok = next_token(&ptr, " ");
    lavg.five = atof(tok);
    tok = next_token(&ptr, " ");
    lavg.fifteen = atof(tok);

    return lavg;
}
/**
this is cpu usage, save all the numberes than find the right math thats somewhere on line and do math..
 @return cpu usage
*/
double pfs_cpu_usage(char *proc_dir, struct cpu_stats *prev, struct cpu_stats *curr)
{
    // {
    //     int counter = 0;
    //     long user;
    //     long nice;
    //     long system;
    //     long idle;
    //     long iowait;
    //     long irq;
    //     long softirq;
    //     long guest;
    //     long guest_nice;
    //     long steal;
    //     int fd = open_path(proc_dir, "stat");
    //     if (fd <= 0)
    //     {
    //         perror("open_path");
    //         return -1;
    //     }
    //     size_t line_sz = 0;
    //     char line[1000] = {0};
    //     while ((line_sz = lineread(fd, line, 1000)) > 0)
    //     {
    //         int tokens = 0;
    //         char *next_tok = line;
    //         char *curr_tok;
    //         while ((curr_tok = next_token(&next_tok, "' '\n\t:")) != NULL)
    //         {
    //             if (counter == 0)
    //             {
    //                 user = curr_tok;
    //                 counter++;
    //             }
    //             if (counter == 1)
    //             {
    //                 nice = curr_tok;
    //                 counter++;
    //             }
    //             if (counter == 2)
    //             {
    //                 system = curr_tok;
    //                 counter++;
    //             }
    //             if (counter == 3)
    //             {
    //                 idle = curr_tok;
    //                 curr->idle=idle;
    //                 counter++;
    //             }
    //             if (counter == 4)
    //             {
    //                 iowait = curr_tok;
    //                 counter++;
    //             }
    //             if (counter == 5)
    //             {
    //                 irq = curr_tok;
    //                 counter++;
    //             }
    //             if (counter == 6)
    //             {
    //                 softirq = curr_tok;
    //                 counter++;
    //             }
    //             if (counter == 7)
    //             {
    //                 steal = curr_tok;
    //                 counter++;
    //             }
    //             if (counter == 8)
    //             {
    //                 guest = curr_tok;
    //                 counter++;
    //             }
    //             if (counter == 9)
    //             {
    //                 guest_nice = curr_tok;
    //                 counter++;
    //             }
    //         }
    //     }
    //     prev->idle;
    //     curr->idle;
    //     prev->total;
        return 0;
    // }
}
/**
this takes care of the memory usage. proc into meminfo taks the total and substract the available to get the useage 
 @return mem usage

*/
struct mem_stats pfs_mem_usage(char *proc_dir)
{ // total and avi
    //total - avi......

    int fd = open_path(proc_dir, "meminfo");
    struct mem_stats mstats = {0};

    if (fd == -1)
    {
        perror("open_path");
        return mstats;
    }

    size_t line_sz = 0;
    char line[256] = {0};
    while ((line_sz = lineread(fd, line, 256)) > 0)
    {
        char *next_tok = line;
        char *curr_tok;
        while ((curr_tok = next_token(&next_tok, "\n\t:")) != NULL)
        {
            if (strcmp(curr_tok, "MemTotal") == 0)
            {
                curr_tok = next_token(&next_tok, "\n\t:");

                printf("%f", mstats.total);
                mstats.total = atof(curr_tok);
            }
            if (strcmp(curr_tok, "MemAvailable") == 0)
            {
                curr_tok = next_token(&next_tok, "\n\t:");

                printf("%f", mstats.used);
                mstats.used = mstats.total - atof(curr_tok);
            }
        }
    }

    return mstats;
}
/**
this takes care of the task stats. makes two mallocs
 @return tstats
 @param pfs_create
*/
struct task_stats *pfs_create_tstats()
{ //malloc and return stats.. tstats ,actStats
    struct task_stats *tstats = (struct task_stats *)malloc(sizeof(struct task_stats));
        tstats->active_tasks = malloc(sizeof(struct task_stats));
    return tstats;
}
/**
this takes care of the task stats and free the two malloc we make
 @return a free malloc
 @param tstats
*/
void pfs_destroy_tstats(struct task_stats *tstats)
{ //free the mallocs at tStats and active stats
    free(tstats->active_tasks);
    free(tstats);
}

/**
this takes care of the task
 @return task
 @param proc_dir
 @param tstats
*/
int pfs_tasks(char *proc_dir, struct task_stats *tstats)
{  
    int fd = open_path(proc_dir, "cpuinfo");
    if (fd == -1)
    {
        perror("open_path");
        return -1;
    }
    return -1;
}
/**
 * I really hate this project
*/
