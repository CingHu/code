#include "common.h"

#include "logger.h"


int main(void)
{
    log_init(LOG_NAME, true);

    get_nic_name();

    if( RT_SUCCESS != parase_dir_file())
    {
       log_info("parase dif faild \n"); 
       return RT_ERROR;
    }

    config_if();

    log_inet_info();
    
    free_mem();

    return RT_SUCCESS;
}

#if 0
int main()
{
    
    log_init(LOG_NAME, true);
    
    return RT_ERROR;
}
#endif
