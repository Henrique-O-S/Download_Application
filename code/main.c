#include <stdio.h>
#include "include/auxiliar.h"

//./main ftp://ftp.up.pt/pub/...

// TO DO
/* - unique use case: connect, login host, passive, get path, success (file saved in CWD) or un-success (indicating failing phase)
- challenging programming aspects: gethostbyname, sockets, control connection, passive, data connection */

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s %s\n", argv[0], "ftp://[<user>:<password>@]<host>/<url-path>");
        return -1;
    }

    // parse command line arguments
    struct arguments args;
    if (parseURL(&args, argv[1]) != 0) {
        return -1;
    }
    printf("User: %s\n", args.user);
    printf("Password: %s\n", args.password);
    printf("Host name: %s\n", args.host_name);
    printf("File path: %s\n", args.file_path);
    printf("File name: %s\n", args.file_name);
    
    struct ftp ftp;
    char response[MAX_IP_LENGTH];    // buffer to read commands

    // get IP Address
    char ipAddress[MAX_IP_LENGTH];
    if (getIPAddress(ipAddress, args.host_name) < 0) {
        return -1;
    }

    // create and connect socket to server
    if ((ftp.control_socket_fd = clientTCP(ipAddress, FTP_PORT_NUMBER)) < 0) {
        printf("Error creating new socket\n");
        return -1;
    }

    // receive confirmation from server
    receiveFromControlSocket(&ftp, response, MAX_IP_LENGTH);

    // checking confirmation from server
    if (response[0] == '2') {
        printf("Expecting username\n");
    }
    else
    {
        printf("Error in conection\n");
        return -1;
    }

    // login in the server
    if (login(&ftp, args.user, args.password) < 0) {
        printf("Login failed\n");
        return -1;
    }

    // change working directory in server
    if (strlen(args.file_path) > 0) {
        if (cwd(&ftp, args.file_path) < 0)
        {
            printf("Error changing directory\n");
            return -1;
        }
    }

    // sends pasv command to get ip address and port to receive the file
    if (getServerPortForFile(&ftp) < 0){
        printf("Error getting server Port for file\n");
        return -1;
    }

    // sends retr command to begin file transfer
    if(retr(&ftp, args.file_name) < 0){
        printf("Error sending comand retr\n");
        return -1;
    }

    // downloads file
    if(downloadFile(&ftp, args.file_name) < 0){
        printf("Error downloading file\n");
        return -1;
    }

    // disconnects from server
    if(disconnect(&ftp) < 0){
        printf("Error disconnecting from server\n");
        return -1;
    }

    return 0;
}