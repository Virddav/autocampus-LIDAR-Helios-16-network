#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 7788
#define BUFFER_SIZE 2048

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // Créer le socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Définir l'adresse et le port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Associer le socket à l'adresse et au port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Mettre le socket en mode écoute
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Listening on port %d\n", PORT);

    // Accepter les connexions entrantes
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Lire les messages reçus et les afficher
    int valread;
    while ((valread = read(new_socket, buffer, BUFFER_SIZE)) > 0) {
        printf("Message received: %s\n", buffer);
        memset(buffer, 0, BUFFER_SIZE);  // Clear the buffer
    }

    if (valread < 0) {
        perror("read failed");
    }

    // Fermer le socket
    close(new_socket);
    close(server_fd);

    return 0;
}

