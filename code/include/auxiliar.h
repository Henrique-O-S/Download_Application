#ifndef _AUXILIAR_H_
#define _AUXILIAR_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "macros.h"

/**
 * Struct that contains the necessary fields to parse the command line arguments passed
 */
struct arguments {
    char user[MAX_IP_LENGTH]; /**< user string */
    char password[MAX_IP_LENGTH]; /**< password string */
    char host_name[MAX_IP_LENGTH]; /**< host name string */
    char file_path[MAX_IP_LENGTH]; /**< file path string */   
    char file_name[MAX_IP_LENGTH]; /**< file name string */ 
};

/**
 * Struct that contains the control and data file descriptors for the FTP
 */
struct ftp {
    int control_socket_fd; /**< file descriptor to control socket */
    int data_socket_fd; /**< file descriptor to data socket */
};

int getIPAddress(char *ipAddress, char *hostName);

int clientTCP(char *address, int port);

int parseArguments(struct arguments* args, char* commandLineArg);

int receiveFromControlSocket(struct ftp *ftp, char* string, size_t size);

int sendToControlSocket(struct ftp* ftp, char* cmdHeader, char* cmdBody);

int sendCommandInterpretResponse(struct ftp* ftp, char* cmdHeader,  char* cmdBody, char* response, size_t responseLength, int readingFile);

int login(struct ftp* ftp, char* username, char* password);

int cwd(struct ftp* ftp, char* path);

int getServerPortForFile(struct ftp *ftp);

int retr(struct ftp* ftp, char* fileName);

int downloadFile(struct ftp* ftp, char* fileName);

int disconnectFromSocket(struct ftp* ftp);


#endif // _AUXILIAR_H_