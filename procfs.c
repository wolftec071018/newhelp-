#include "logger.h"
#include "procfs.h"
#include <string.h>
#include "util.h"

int pfs_hostname(char *proc_dir, char *hostname_buf, size_t buf_sz)
{
	int fd = open_path(proc_dir, "sys/kernel/hostname");
	if(fd<=0){
		perror("open_path");
		    return -1;
}
one_lineread(fd, hostname_buf ,buf_sz);
return 0;
}

int pfs_kernel_version(char *proc_dir, char *version_buf, size_t buf_sz)
{
	int fd= open_path(proc_dir, "sys/kernel/osrelease");
	if(fd<=0){
	perror("open_path"); 
    return -1;
}
	one_lineread(fd, version_buf,buf_sz);
	size_t dash_pos = strcspn(version_buf,"-");
	version_buf[dash_pos]='\0';
return 0;
}

int pfs_cpu_model(char *proc_dir, char *model_buf, size_t buf_sz)
{
    int fd = open_path(proc_dir, "cpuinfo");
	if(fd <= 0){
	    perror("open_path"); 
        return -1;
    }
    
    size_t line_sz=0;
    char line[128];
    while((line_sz = lineread(fd, line , 128))>0){
        int tokens = 0;
        char *next_tok = line;
        char *curr_tok;
        while ((curr_tok = next_token(&next_tok, " \n\t:")) != NULL) {
            if(strcmp(curr_tok, "model name")== 0){
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
    return 0;
}

double pfs_uptime(char *proc_dir)
{
    return 0.0;
}

int pfs_format_uptime(double time, char *uptime_buf)
{
    return -1;
}

struct load_avg pfs_load_avg(char *proc_dir)
{
   struct load_avg lavg = { 0 };
   return lavg;
}

double pfs_cpu_usage(char *proc_dir, struct cpu_stats *prev, struct cpu_stats *curr)
{
    return 0.0;
}

struct mem_stats pfs_mem_usage(char *proc_dir)
{
    struct mem_stats mstats = { 0 };
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

