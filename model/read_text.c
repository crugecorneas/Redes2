/* TEST PARA VER COMO LEER DE SERVER.CONF */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(){
    FILE *fd = NULL;
    int length = 64; /* con menos no guarda el 1.1 de server_signature */
    char aux[length], server_root[length], server_signature[length];
    int max_clients, listen_port;

    /* LEER MAX_CLIENTS Y LISTEN_PORT */
    if(!(fd = fopen("server.conf", "r"))){
        perror("fopen");
        return 1;
    }
    while(fgets(aux, length, fd)){
        if (strncmp("max_clients", aux, strlen("max_clients")) == 0){
            strtok(aux, " \n");
            strtok(NULL, " \n");
            max_clients = atoi(strtok(NULL, " \n"));
            printf("Deberia ser max_clients = 10: %d\n", max_clients);
        }
        if(strncmp("listen_port", aux, strlen("listen_port")) == 0){
            strtok(aux, " \n");
            strtok(NULL, " \n");
            listen_port = atoi(strtok(NULL, " \n"));
            printf("Deberia ser listen_port = 8080: %d\n", listen_port);
        }
    }
    fclose(fd);

    /* LEER SERVER_ROOT Y SERVER_SIGNATURE */
    if(!(fd = fopen("server.conf", "r"))){
        perror("fopen");
        return 1;
    }
    while (fgets(aux, length, fd)) {
        if (strncmp("server_root", aux, strlen("server_root")) == 0){
            strtok(aux, " \n");
            strtok(NULL, " \n");
            sprintf(server_root, "%s", strtok(NULL, " \n"));
            printf("Deberia ser server_root = htmlfiles/: %s\n", server_root);
        }
        if (strncmp("server_signature", aux, strlen("server_signature")) == 0) {
            strtok(aux, "=");
            char *value = strtok(NULL, "\n");
            sscanf(value, " %[^\n]", server_signature);
            printf("Deberia ser server_signature = MyCoolServer 1.1: %s\n", server_signature);
        }
    }
    fclose(fd);
}
