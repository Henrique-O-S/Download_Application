#include <stdio.h>
#include "macros.h"
#include "auxiliar.h"

// TO DO
/* - unique use case: connect, login host, passive, get path, success (file saved in CWD) or un-success (indicating failing phase)
- challenging programming aspects: gethostbyname, sockets, control connection, passive, data connection */

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s %s\n", argv[0], "ftp://[<user>:<password>@]<host>/<url-path>");
        return -1;
    }
}