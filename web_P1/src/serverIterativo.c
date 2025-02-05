#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8081
#define IP "127.0.0.1"
int main()
{

    // create
    int server_socket,client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    char buffer[1024];

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        perror("Error al crear el socket");
        exit(1);
    }

    // accept
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);          // El puerto en el que el servidor está escuchando
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Dirección IP del servidor (localhost)
    // Asociar el socket con la dirección
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Error al hacer bind");
        exit(1);
    }

    // listen
    if (listen(server_socket, 5) < 0)
    {
        perror("Error al escuchar");
        exit(1);
    }

    printf("Escuchando en la ip %s en el puerto %d", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    // Aceptar conexiones entrantes en un bucle infinito
    while (1)
    {
        client_len = sizeof(client_addr);

        // Aceptar la conexión (bloqueante)
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket < 0)
        {
            perror("Error al aceptar la conexión");
            continue;
        }

        printf("Conexión entrante de %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Leer la solicitud del cliente
        bzero(buffer, 1024);
        read(client_socket, buffer, sizeof(buffer) - 1);
        printf("Solicitud recibida: %s\n", buffer);

        // Enviar una respuesta al cliente
        const char *response = "ACK!";
        write(client_socket, response, strlen(response));

        // Cerrar el socket del cliente
        close(client_socket);
    }

    // Cerrar el socket del servidor
    close(server_socket);

    return 0;
}