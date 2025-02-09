/**
 * @file server.c
 * @author Alejandro Monterrubio & Esther Perez
 * @brief Crea conexion del servidor.
 * @version 0.3
 * @date 13/03/24
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "../includes/server_functs.h"
#include "../includes/socket_lib.h"

int socketfd, connectionfd; /* Descriptor de socket y conexion */
request_struct client_request; /* Estructura de peticion de cliente */

/**
 * @brief Lee server.conf y empieza conexion.
 *
 * @return 0 si correcto, 1 si error.
 */
int start_server(){
    FILE *fd = NULL;
    int max_clients, listen_port;
    char aux[SIZE_128];

    /* Abrir fichero de configuracion */
    if(!(fd = fopen("server.conf", "r"))){
        perror("fopen");
        return 1;
    }

    /* Leer max_clients y listen_port */
    while(fgets(aux, SIZE_128, fd)){
        if (strncmp("max_clients", aux, strlen("max_clients")) == 0){
            strtok(aux, " \n");
            strtok(NULL, " \n");
            max_clients = atoi(strtok(NULL, " \n"));
        }
        if(strncmp("listen_port", aux, strlen("listen_port")) == 0){
            strtok(aux, " \n");
            strtok(NULL, " \n");
            listen_port = atoi(strtok(NULL, " \n"));
        }
    }

    fclose(fd);

    /* Prepara sockets */
    socketfd = lib_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    lib_bind(socketfd, INADDR_ANY, listen_port);
    if(listen(socketfd, max_clients) == -1){
        perror("listen()");
        exit(EXIT_FAILURE);
    }
    fprintf(stdout, "Accepting connections, using port %d\n", listen_port);

    return 0; /* habia exit(EXIT_SUCESS) pero entonces cerraba el servidor*/
}

/**
 * @brief Termina la conexion.
 *
 */
void handler(){
    close(socketfd);
    close(connectionfd);
    fprintf(stdout, "\nServer closed\n");
    exit(EXIT_SUCCESS);
}

int main(){
    pid_t pid;
    struct sigaction act;

    /* Pasos al recibir Ctrl + C, SIGINT*/
    act.sa_handler = handler;
    act.sa_flags = SIGINT;
    sigemptyset(&(act.sa_mask));
    if(sigaction(SIGINT, &act, NULL) < 0){
        perror("sigaction()");
        exit(EXIT_FAILURE);
    }

    /* Funcion auxiliar inicializadora */
    if(start_server() != 0){
        perror("start_server()");
        exit(EXIT_FAILURE);
    }

    /* Empieza a aceptar conexiones */
    while(true){
        connectionfd = lib_accept(socketfd);
        pid = fork();
        if(pid == 0){
            request(connectionfd, &client_request);
            close(connectionfd);
            exit(EXIT_SUCCESS);
        }
        else if(pid == -1){
            perror("fork");
            close(connectionfd);
            exit(EXIT_FAILURE);
        }
        close(connectionfd);
    }

    close(socketfd);
    return 0;
}
