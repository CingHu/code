#include "common.h"

#include "logger.h"
#include "inet.h"
#include "interface.h"
#include "utils.h"
#include "external.h"
#include "basefunc.h"

extern int    g_version;


void print_func()
{
    printf("get_name_by_mac mac\n");
    printf("interface\n");
    printf("reset_interface mac\n");
    printf("reset_all_interface\n");
}

int main(int argc, char *argv[])
{

    bool rt = false;
    char name[DEVICE_NAME_LEN];
    char mac[6];
    void* func[128];

    if(argc < 2)
    {
        printf("input param error!\n");
        print_func();
        return false;
    }
   
    strcpy(func, argv[1]);

    if(get_version(&g_version))
    {
        log_error("get version failed\n");
        return RT_ERROR;
    }

    log_init(LOG_NAME, true);

    get_nic_name();

    if(strcmp(func, "get_name_by_mac") == 0 && argc == 3)
    {
        strcpy(mac, argv[2]);
        ex_get_name_by_mac(mac, name);
        printf("%s\n",name);
        return RT_SUCCESS;
    }

    if( RT_SUCCESS != parase_dir_file())
    {
       rt = log_info("parase dif faild \n"); 
       goto end;
    }

    if(strcmp(func, "interface") == 0 && argc == 2)
    {
       rt = config_if();
       goto end;
    }
    else if(strcmp(func, "reset_interface") == 0 && argc == 3)
    {
        strcpy(mac, argv[2]);
        rt = ex_restart_interface(mac);
        goto end;
    }
    else if(strcmp(func, "reset_all_interface") == 0 && argc == 2)
    {
        rt = ex_restart_network();
        goto end;
    }
    else
    {
        printf("input param error!\n");
        print_func();
        rt = RT_ERROR;
        goto end;
    }


end:

    log_inet_info();
    free_mem();

    return rt;
}

