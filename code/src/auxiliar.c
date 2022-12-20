#include "../include/auxiliar.h"

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

    if ((h = gethostbyname(hostName)) == NULL) {
        herror("gethostbyname()");
        return -1;
    }

    strcpy(ipAddress, inet_ntoa(*((struct in_addr *) h->h_addr)));

    /* printf("Host name  : %s\n", h->h_name);
    printf("IP Address : %s\n", inet_ntoa(*((struct in_addr *) h->h_addr))); */

    return 0;
}

int clientTCP(char *address, int port){
    int sockfd;
    struct sockaddr_in server_addr;

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

    printf("Parsing command line arguments\n");
    
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

    printf("Parsed command line arguments.\n");

    return 0;

}

int receiveFromControlSocket(struct ftp *ftp, char *response, size_t size) {
    printf("Receiving from control socket: ");
    FILE *fp = fdopen(ftp->control_socket_fd, "r");
    do {
        memset(response, 0, size);
        response = fgets(response, size, fp);
        printf("%s", response);
    } while (('1' > response[0] || response[0] > '5') || response[3] != ' ');
    return 0;
}

int sendToControlSocket(struct ftp *ftp, char *cmdHeader, char *cmdBody) {
    printf("Sending to control Socket: %s %s\n", cmdHeader, cmdBody);
    int bytes = write(ftp->control_socket_fd, cmdHeader, strlen(cmdHeader));
    if (bytes != strlen(cmdHeader))
        return -1;
    bytes = write(ftp->control_socket_fd, " ", 1);
    if (bytes != 1)
        return -1;
    bytes = write(ftp->control_socket_fd, cmdBody, strlen(cmdBody));
    if (bytes != strlen(cmdBody))
        return -1;
    bytes = write(ftp->control_socket_fd, "\n", 1);
    if (bytes != 1)
        return -1;
    return 0;
}

int sendCommandReceiveResponse(struct ftp *ftp, char *cmdHeader, char *cmdBody, char *response, size_t responseLength, int readingFile) {
    if (sendToControlSocket(ftp, cmdHeader, cmdBody) < 0) {
        printf("Error Sending Command  %s %s\n", cmdHeader, cmdBody);
        return -1;
    }
    int code;
    while (1) {
        receiveFromControlSocket(ftp, response, responseLength);
        code = response[0] - '0';
        switch (code) {
            case 1:
                // expecting another reply
                if (readingFile) 
                    return 2;
                else 
                    break;
            case 2:
                // request action success
                return 2;
            case 3:
                // needs aditional information
                return 3;
            case 4:
                // try again
                if (sendToControlSocket(ftp, cmdHeader, cmdBody) < 0) {
                    printf("Error Sending Command  %s %s\n", cmdHeader, cmdBody);
                    return -1;
                }
                break;
            case 5:
                // error in sending command, closing control socket , exiting application
                printf("Command wasn\'t accepted\n");
                close(ftp->control_socket_fd);
                exit(-1);
                break;
            default:
                break;
        }
    }
}

int login(struct ftp *ftp, char *username, char *password) {
    printf("Sending Username\n");
    char response[MAX_IP_LENGTH];
    int rtr = sendCommandReceiveResponse(ftp, "user", username, response, MAX_IP_LENGTH, 0);
    if (rtr == 3 || rtr == 2) {
        printf("Sent Username\n");
    }
    else {
        printf("Error sending Username\n");
        return -1;
    }
    if (rtr == 3){
        printf("Sending Password\n");
        rtr = sendCommandReceiveResponse(ftp, "pass", password, response, MAX_IP_LENGTH, 0);
        if (rtr == 2) {
            printf("Sent Password\n");
        }
        else {
            printf("Error sending Password\n");
            return -1;
        }
    }
    return 0;
}

int cwd(struct ftp* ftp, char* path) {
    char response[MAX_IP_LENGTH];
    if(sendCommandReceiveResponse(ftp, "CWD", path, response, MAX_IP_LENGTH, 0) < 0){
        printf("Error sending cwd command\n");
        return -1;
    }
	return 0;
}

int getServerPortForFile(struct ftp *ftp) {
    char firstByte[4];
    char secondByte[4];
    memset(firstByte, 0, 4);
    memset(secondByte, 0, 4);
    char response[MAX_IP_LENGTH];
    int ipPart1, ipPart2, ipPart3, ipPart4;
    int port1, port2;
    int rtr = sendCommandReceiveResponse(ftp, "pasv", "", response, MAX_IP_LENGTH, 0);
    if (rtr < 0) {
        printf("Error sending pasv command\n");
        return -1;
    }
    else if (rtr == 2) {
        // starting to process information
        if ((sscanf(response, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)",
                    &ipPart1, &ipPart2, &ipPart3, &ipPart4, &port1, &port2)) < 0) {
            printf("ERROR: Cannot process information to calculating port.\n");
            return -1;
        }
    }
    else {
        printf("Error receiving pasv command response from server\n");
        return -1;
    }

    char ip[MAX_IP_LENGTH];
    sprintf(ip, "%d.%d.%d.%d", ipPart1, ipPart2, ipPart3, ipPart4);
    int port = port1 * 256 + port2;
    printf("Port number %d\n", port);
    if ((ftp->data_socket_fd = clientTCP(ip, port)) < 0) {
        printf("Error creating new socket\n");
        return -1;
    }
    return 0;
}

int retr(struct ftp* ftp, char* fileName){
    char response[MAX_IP_LENGTH];
    if(sendCommandReceiveResponse(ftp, "RETR", fileName, response, MAX_IP_LENGTH, 1) < 0){
        printf("Error sending retr command\n");
        return -1;
    }
	return 0;
}

int downloadFile(struct ftp* ftp, char * fileName){
    FILE *fp = fopen(fileName, "w");
    if (fp == NULL){
        printf("Error opening or creating file\n");
        return -1;
    }
    char buf[1024];
    int bytes;
    printf("Starting to download file with name %s\n", fileName);
    while((bytes = read(ftp->data_socket_fd, buf, sizeof(buf)))){
        if(bytes < 0){
            printf("Error reading from data socket\n");
            return -1;
        }
        if((bytes = fwrite(buf, bytes, 1, fp)) < 0){
            printf("Error writing data to file\n");
            return -1;
        }
    }

    printf("Finished dowloading file\n");

    if(fclose(fp)){
        printf("Error closing file\n");
        return -1;
    }
    close(ftp->data_socket_fd);
    char response[MAX_IP_LENGTH];
    receiveFromControlSocket(ftp, response, MAX_IP_LENGTH);
    if (response[0] != '2')
        return -1;
    return 0;
}

int disconnect(struct ftp* ftp) {
    char response[MAX_IP_LENGTH];
    if(sendCommandReceiveResponse(ftp, "QUIT", "", response, MAX_IP_LENGTH, 0) != 2){
        printf("Error sending quit command\n");
        return -1;
    }
	return 0;
}

