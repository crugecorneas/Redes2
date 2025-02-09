#include "../includes/socket_lib.h"


int lib_socket(int domain, int type, int protocol){
    int sockfd;

    sockfd = socket(domain, type, protocol);
    if(sockfd == -1){
        perror("lib_socket()");
        exit(EXIT_FAILURE);  
    }

    return sockfd;
}


int lib_bind(int sockfd, int address, int port){
    struct sockaddr_in addr_params;
    int socket_descriptor;

    /*addr_params.sin_len = 16;*/ /* length of structure (16)*/
    addr_params.sin_family = AF_INET; /* AF_INET*/
    addr_params.sin_port =  htons(port); /* 16 bit TCP or UDP port number */
    addr_params.sin_addr.s_addr = htonl(address); /* 32 bit IPv4 address*/
    bzero ((void*) &(addr_params.sin_zero), 8); /* not used but always set to zero */
    /* eso creo que es para llenar el array de 8 pos pero no se, daba error sino */

    socket_descriptor = bind(sockfd, (struct sockaddr*) &addr_params, sizeof(addr_params));
    if(socket_descriptor == -1){
        perror("lib_bind()");
        exit(EXIT_FAILURE);
    }

    return socket_descriptor;
}


int lib_accept(int sockfd){
    struct sockaddr address;
    socklen_t address_len;
    int socket_descriptor;

    address_len = sizeof(address);

    socket_descriptor = accept(sockfd, &address, &address_len);
    if(socket_descriptor == -1){
        perror("lib_accept()");
        exit(EXIT_FAILURE);
    }

    return socket_descriptor;
}

/* Creo que no hace falta listen() ni send() */
