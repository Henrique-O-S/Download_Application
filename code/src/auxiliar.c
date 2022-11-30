#include "auxiliar.h"

int getIPAddress(char *ipAddress, char *hostName){
    struct hostent *h;
    
/**
 * The struct hostent (host entry) with its terms documented

    struct hostent {
        char *h_name;    // Official name of the host.
        char **h_aliases;    // A NULL-terminated array of alternate names for the host.
        int h_addrtype;    // The type of address being returned; usually AF_INET.
        int h_length;    // The length of the address in bytes.
        char **h_addr_list;    // A zero-terminated array of network addresses for the host.
        // Host addresses are in Network Byte Order.
    };

    #define h_addr h_addr_list[0]	The first address in h_addr_list.
*/
    if (h = gethostbyname(hostName) == NULL) {
        herror("gethostbyname()");
        return -1;
    }

    strcpy(ipAddress, inet_ntoa(*((struct in_addr *) h->h_addr)));

    /* printf("Host name  : %s\n", h->h_name);
    printf("IP Address : %s\n", inet_ntoa(*((struct in_addr *) h->h_addr))); */

    return 0;
}
