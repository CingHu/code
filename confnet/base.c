#include "common.h"

#define REDHAT_LEN   7
#define SUSE_LEN     3
#define DEBAIN_LEN   6


#define RED_HAD_CENTOS_FLAG "Red Hat Centos centos CENTOS RedHat"
#define DEBAIN_UBUNTU_FLAG  "DEBAIN debain ubuntu Ubuntu UBUNTU"

const char* redhat_str[REDHAT_LEN] = {
                         "Red Hat",
                         "redhat",
                         "RedHat",
                         "REDHAT",
                         "Centos",
                         "centos",
                         "CENTOS"
                         };

const char* debain_str[DEBAIN_LEN] = {
                          "DEBAIN",
                          "debain",
                          "Debain",
                          "ubuntu",
                          "Ubuntu",
                          "UBUNTU"
                          };


const char* suse_str[SUSE_LEN] = {
                          "SUSE",
                          "suse",
                          "Suse"
                          };


int  get_version(int *version)
{
   FILE *fp;
   int i = 0;
   int file_size;
   char *buf;
    
   system("cp /proc/version /tmp/version");
    
   fp = fopen("/tmp/version","r");
   if(NULL == fp)
   {
       return 0;
   }

   fseek( fp , 0 , SEEK_END);
   file_size = ftell(fp);  
   fseek( fp , 0 , SEEK_SET);
   buf =  (char *)malloc( (file_size+1) * sizeof( char ) );
   fread( buf , sizeof(char),file_size, fp);
   buf[file_size] = '\0';
   fclose(fp);

   for(i = 0; i < REDHAT_LEN;i++)
   {
       if(strstr(buf,redhat_str[i]) != 0)
       {
          *version = REDHAT;
          free(buf);
          return 0;
       }
   }
   for(i = 0; i < DEBAIN_LEN;i++)
   {
       if(strstr(buf,debain_str[i]) != 0)
       {
          *version = DEBAIN;
          free(buf);
          return 0;
       }
   }
   for(i = 0; i < SUSE_LEN;i++)
   {
       if(strstr(buf,suse_str[i]) != 0)
       {
          *version = SUSE;
          free(buf);
          return 0;
       }
   }
 
   free(buf);
   return 1;
}

