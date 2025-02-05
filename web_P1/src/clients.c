#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define BUFFER_SIZE 4096

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char request[] = "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n";
    char buffer[BUFFER_SIZE];

    // 1. Crear el socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error al crear el socket");
        exit(1);
    }

    // 2. Configurar dirección del servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Dirección inválida o no soportada");
        exit(1);
    }

    // 3. Conectar al servidor
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error al conectar con el servidor");
        exit(1);
    }

    // 4. Enviar la solicitud HTTP
    if (send(sock, request, strlen(request), 0) == -1) {
        perror("Error al enviar la solicitud");
        exit(1);
    }

    // 5. Recibir la respuesta
    printf("Respuesta del servidor:\n");
    int bytes_received;
    while ((bytes_received = recv(sock, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';  // Agregar terminador de cadena
        printf("%s", buffer);
    }

    if (bytes_received == -1) {
        perror("Error al recibir datos");
    }

    // 6. Cerrar el socket
    close(sock);
    return 0;
}
