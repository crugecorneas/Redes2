#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "../includes/picohttpparser/picohttpparser.h"

#define PORT 8080
#define BUFFER_SIZE 4096
#define RESPONSE "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, world!"

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    int minor_version, status;
    const char *method, *path;
    size_t method_len, path_len, num_headers = 20;
    struct phr_header headers[20];

    // Recibir datos del cliente
    ssize_t recv_len = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (recv_len <= 0) {
        close(client_socket);
        return;
    }

    // Parsear la petición HTTP
    int ret = phr_parse_request(buffer, recv_len, &method, &method_len, &path, &path_len,
                                &minor_version, headers, &num_headers, 0);

    if (ret < 0) {
        printf("Error al parsear la petición HTTP\n");
        close(client_socket);
        return;
    }

    // Imprimir la petición recibida
    printf("Método: %.*s, Ruta: %.*s, HTTP/1.%d\n", (int)method_len, method, (int)path_len, path, minor_version);

    // Enviar respuesta simple
    send(client_socket, RESPONSE, strlen(RESPONSE), 0);
    close(client_socket);
}

void *thread_func(void *arg) {
    int client_socket = *(int *)arg;
    handle_client(client_socket);
    free(arg);
    return NULL;
}

int main() {
    int server_socket, *client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Crear el socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error al crear el socket");
        return EXIT_FAILURE;
    }

    // Configurar la dirección del servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Asociar el socket con la dirección
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error en bind");
        close(server_socket);
        return EXIT_FAILURE;
    }

    // Escuchar conexiones entrantes
    if (listen(server_socket, 10) == -1) {
        perror("Error en listen");
        close(server_socket);
        return EXIT_FAILURE;
    }

    printf("Servidor escuchando en el puerto %d...\n", PORT);

    while (1) {
        client_socket = malloc(sizeof(int));
        *client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
        if (*client_socket == -1) {
            perror("Error en accept");
            free(client_socket);
            continue;
        }

        // Crear un hilo para manejar la conexión
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, thread_func, client_socket);
        pthread_detach(thread_id);
    }

    close(server_socket);
    return EXIT_SUCCESS;
}
