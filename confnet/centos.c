#include "common.h"
#include "interface.h"
#include "logger.h"
#include "utils.h"


//#define NETWORK_PATH "/etc/sysconfig/network-scripts/"
#define NETWORK_PATH "/tmp/"

extern int    g_dev_counter;

void _centos_get_if_file_name(const char* name, int flag, char *if_name)
{
     printf("%s\n", name);
    printf("%d\n",g_dev_counter);
   
    if(flag == MASTER_FLAG)
       sprintf(if_name, "%s%s%s",NETWORK_PATH, "ifcfg-",name);
    else
    {
       g_dev_counter++;
       sprintf(if_name, "%s%s%s:%d",NETWORK_PATH, "ifcfg-",name, g_dev_counter);
    }

    return; 
}

void _centos_get_if_file_name2(const char* name, int flag, char *if_name)
{
   
    if(flag == MASTER_FLAG)
       sprintf(if_name, "%s",name);
    else
    {
       sprintf(if_name, "%s:%d", name, g_dev_counter);
    }

    return; 
}


bool centos_config_if_file()
{
    FILE *fp;
    struct inet_t *inet;
    int i = 0;
    char name[DEVICE_NAME_LEN] = {0};

    g_dev_counter = 0;

    for(;i < g_inet_num; i++)
    {
       inet = &g_inet_array[i];

       if(strlen(inet->name) == 0)
           continue;

        _centos_get_if_file_name(inet->name, inet->flag, name);

        fp = fopen(name, "w");
        if(NULL == fp)
        {
            log_error("Could not open file %s: %s", name, strerror(errno));
            return false;
        }
  
        memset(name, 0, DEVICE_NAME_LEN);         

        _centos_get_if_file_name2(inet->name, inet->flag, name);

        centos_external_if(fp, inet, name);

        memset(name, 0, DEVICE_NAME_LEN);         
        fclose(fp);
    }

    g_dev_counter = 0;
    
    return true; 
}


void centos_external_if(FILE* fp, struct inet_t *inet, const char* name)
{
    struct pair_t* pos;
    bool flag = false;

    fprintf(fp, "DEVICE=%s\n", name);
    fprintf(fp, "ONBOOT=YES\n");
    fprintf(fp, "BOOTPROTO=static\n");

    list_for_each_entry(pos, &(inet->pair.list), list)
    {
        if(stringfind(pos->key,"DNS") != -1)        
        {
           flag = true;
        }
        else
        {
            fprintf(fp,"%s=%s\n", pos->key, pos->value);
        }
    }

   if(!flag)
   {
        fprintf(fp, "DNS1 = %s\n", DEFAULT_DNS1);
        fprintf(fp, "DNS2 = %s\n", DEFAULT_DNS2);
        flag = false;
   }

   return;
}
