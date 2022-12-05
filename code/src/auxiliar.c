#include "auxiliar.h"
#include "macros.h"

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

int clientTCP(char *address, int port){
    if (argc > 1)
        printf("**** No arguments needed. They will be ignored. Carrying ON.\n");
    int sockfd;
    struct sockaddr_in server_addr;
    char buf[] = "Mensagem de teste na travessia da pilha TCP/IP\n";
    size_t bytes;

    /*server address handling*/
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(address);    /*32 bit Internet address network byte ordered*/
    server_addr.sin_port = htons(port);        /*server TCP port must be network byte ordered */

    /*open a TCP socket*/
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        exit(-1);
    }
    /*connect to the server*/
    if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("connect()");
        exit(-1);
    }
    
    return sockfd;
}

int parseArguments(struct arguments *args, char *commandLineArg) {

    printf("Parsing command line arguments...\n");
    
    // verifying FTP protocol
    char *token = strtok(commandLineArg, ":");
    if ((token == NULL) || (strcmp(token, "ftp") != 0)) {
        printf("Protocol is not FTP\n");
        return -1;
    }

    token = strtok(NULL, "\0");
    char arguments[MAX_IP_LENGTH];
    strcpy(arguments, token);

    // parsing user name
    char aux[MAX_IP_LENGTH];
    strcpy(aux, arguments);
    token = strtok(aux, ":");

    if (token == NULL || (strlen(token) < 3) || (token[0] != '/') || (token[1] != '/')) {
        printf("Error parsing the user's name\n");
        return -1;
    }
    else if (strcmp(token, arguments) == 0) {
        memset(args->user, 0, sizeof(args->user));
        strcpy(args->user, "anonymous");
        memset(args->password, 0, sizeof(args->password));
        strcpy(args->password, "");

        strcpy(aux, &arguments[2]);
        strcpy(arguments, aux);
    }
    else {
        memset(args->user, 0, sizeof(args->user));
        strcpy(args->user, &token[2]);

        // parsing password
        token = strtok(NULL, "@");
        if (token == NULL || (strlen(token) == 0)) {
            printf("Error parsing the password\n");
            return -1;
        }
        memset(args->password, 0, sizeof(args->password));
        strcpy(args->password, token);

        token = strtok(NULL, "\0");
        strcpy(arguments, token);
    }

    // parsing hostname
    token = strtok(arguments, "/");    
    if (token == NULL) {
        printf("Error parsing the hostname\n");
        return -1;
    }
    memset(args->host_name, 0, sizeof(args->host_name));
    strcpy(args->host_name, token);

    // parsing path and name
    token = strtok(NULL, "\0");
    if (token == NULL) {
        printf("Error parsing the file path\n");
        return -1;
    }
    char* file_name = strrchr(token, '/');
    if (file_name != NULL) {
        memset(args->file_path, 0, sizeof(args->file_path));
        strncpy(args->file_path, token, file_name - token);
        memset(args->file_name, 0, sizeof(args->file_name));
        strcpy(args->file_name, file_name + 1);
    }
    else {
        memset(args->file_path, 0, sizeof(args->file_path));
        strcpy(args->file_path, "");
        memset(args->file_name, 0, sizeof(args->file_name));
        strcpy(args->file_name, token);
    }

    printf("Parsed command line arguments.\n\n");

    return 0;

}


