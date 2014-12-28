#include "common.h"
#include "logger.h"
#include "interface.h"


extern char   g_external_name[DEVICE_NAME_LEN];
extern char   g_internal_name[DEVICE_NAME_LEN];

static bool
if_list_have(inetface_t *head, const char *name)
{
    if (head == NULL)
        return false;

    while (head) {
        if (strcmp(head->name, name) == 0) return true;
        head = head->next;
    }

    return false;
}


static bool
if_list_write_mac_and_ip(inetface_t *head)
{
    if (head == NULL)
        return false;

    int i, j, rc;
    int sock;
    struct ifreq ifr;
    unsigned char *hwaddr;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        log_error("Socket() failed (%d: %s)", errno, strerror(errno));
        return false;
    }

    for (;head; head = head->next) {

        if (strlen(head->name) == 0)
            continue;

        /* Get hardware address. */
        memset(&ifr, 0, sizeof(ifr));
        strcpy(ifr.ifr_name, head->name);
        ifr.ifr_addr.sa_family = AF_INET;

        rc = ioctl(sock, SIOCGIFHWADDR, &ifr);
        if (rc < 0) {
            log_error("ioctl() failed for head->name (%d: %s)",
                      errno, strerror(errno));
            continue;
        }

        hwaddr = (unsigned char *)ifr.ifr_hwaddr.sa_data;

        for (i = 0, j = 0; i < 6; i++) {
            head->mac[j++] = LHHEXDIG(hwaddr[i]);
            head->mac[j++] = LLHEXDIG(hwaddr[i]);
            if (i != 5)
                head->mac[j++] = ':';
        }

        head->mac[17] = '\0';

        /* If the interface is up, the get IPv4 address. */
        if (head->up) { memset(&ifr, 0, sizeof(ifr));
            strcpy(ifr.ifr_name, head->name);
            ifr.ifr_addr.sa_family = AF_INET;

            rc = ioctl(sock, SIOCGIFADDR, &ifr);
            if (rc < 0) {
                log_error("ioctl() failed for head->name (%d: %s)",
                          errno, strerror(errno));
                continue;
            }

            strcpy(head->ip,
                   inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
        }
    }

    close(sock);

    return true;
}


inetface_t *
load_if_info(void)
{
    inetface_t *head = NULL, *new;
    struct ifaddrs *ifap = NULL, *it;
    int rc;

    rc = getifaddrs(&ifap);
    if (rc != 0) {
        log_error("getifaddrs() failed (%d: %s)", errno , strerror(errno));
        return NULL;
    }

    for (it = ifap; it; it = it->ifa_next) {

        if (it->ifa_flags & IFF_LOOPBACK)
            continue;

        if (if_list_have(head, it->ifa_name))
            continue;

        new = malloc(sizeof(inetface_t));
        memset(new, 0, sizeof(inetface_t));

        new->next = head;
        head = new;

        /* through the following form seems stupid, i like it. */
        if (it->ifa_flags & IFF_UP)
            new->up = true;
        else
            new->up = false;

        strcpy(new->name, it->ifa_name);
    }

    freeifaddrs(ifap);

    /* Now read hardware address and available IP. */
    if_list_write_mac_and_ip(head);

    return head;
}


void if_list_free(inetface_t *head)
{
    inetface_t *tmp;

    while (head) {
        tmp = head;
        head = head->next;
        free(tmp);
    }
}

/* Check an IP address (only for IPv4) address, tell
 * Where it's a public IP or not.
 *
 * TODO: maybe it's would be easier just using inet_addr().
 */

bool
is_ip_public(const char *ip)
{
    char buf[20];
    char *p;
    char *first, *second;

    strcpy(buf, ip);

    /* first section */

    first = buf;
    p = strchr(ip, '.');

    if (p == NULL) {
        errno = EINVAL;
        return false;
    }

    *p = '\0';

    /* second section */

    second = p + 1;
    p = strchr(second, '.');

    if (p == NULL) {
        errno = EINVAL;
        return false;
    }

    *p = '\0';

    /* check */

    int num_first = atoi(first);
    int num_second = atoi(second);

    /* RFC 1918 private ip address class */

    if (num_first == 10)
        return false;

    if (num_first == 172 && num_second >= 16 && num_second <= 31)
        return false;

    if (num_first == 192 && num_second == 168)
        return false;

    /* loopback 127.0.0.1 ~ 127.255.255.254 and 0.0.0.0 */
    /* TODO: ending 254 not checked */

    if (num_first == 127 || num_first == 0)
        return false;

    /* multi-broadcasting */

    if (num_first >= 224 && num_first <= 255)
        return false;

    return true;
}

bool get_name_by_mac(const char* mac, char* if_name)
{
    inetface_t *i;
    inetface_t * ifaces = NULL;    

    ifaces = load_if_info();

    
    if(NULL == ifaces)
        return false;

    for(i = ifaces;i;i = i->next)
    {
 
        if(strcmp(mac, i->mac) == 0) 
        {
             strcpy(if_name,i->name);
             return true;
        }
            
    }
    
   return false;
     
}

bool get_nic_name(void)
{
    inetface_t *i;
    inetface_t * ifaces = NULL;    

    ifaces = load_if_info();

    if(NULL == ifaces)
        return false;

    for(i = ifaces;i;i = i->next)
    {
        if(stringfind(i->mac, EXTERNAL_MAC_PREFIX) != -1 ||
            stringfind(i->mac, EXTERNAL_MAC_PREFIX_B) != -1) 
        {
             strcpy(g_external_name, i->name);
        }
        else
        {
             strcpy(g_internal_name, i->name);
        }
   
            
    }
    
   return true;
     
}


bool get_ip_by_name(char if_name[DEVICE_NAME_LEN], char ipaddress[IP_ADDR_LEN])
{

    inetface_t *i;
    inetface_t * ifaces = NULL;    

    ifaces = load_if_info();

    log_info("get ip by name, name:%s",if_name);

    if(NULL == ifaces)
    {
        log_error("input param is NULL");
        return false;
    }
        return false;

    for(i = ifaces;i;i = i->next)
    {
        if(strcmp(if_name, i->name) == 0) 
        {
             strcpy(ipaddress,i->ip);
             log_info("get ip by name success, ip:%s",ipaddress);
             return true;
        }
            
    }
    
   log_info("get ip by name fail,name:%s",if_name);
   return false;
}








