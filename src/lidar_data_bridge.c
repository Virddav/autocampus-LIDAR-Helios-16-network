#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#define PORT 6699
#define BUFFER_SIZE 4992
#define DEST_IP "127.0.0.1"
#define DEST_PORT 9000

int main() {
    int sequence_number = 0;
    int sockfd, send_sockfd;
    struct sockaddr_in server_addr, client_addr, dest_addr;
    unsigned char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(client_addr);

    // Créer la socket pour recevoir les données du LIDAR
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // Configurer l'adresse du LIDAR
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    // Lier la socket à l'adresse du LIDAR
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    
    // Créer la socket pour envoyer les données à raw_its
    if ((send_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Send socket creation failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Configurer l'adresse du destinataire (raw-its)
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(DEST_PORT);
    dest_addr.sin_addr.s_addr = INADDR_ANY;

    // Boucle pour recevoir et envoyer les paquets
    while (sequence_number<10000) {
        ssize_t n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
        if (n < 0) {
            perror("Receive failed");
            close(sockfd);
            close(send_sockfd);
            exit(EXIT_FAILURE);
        }

        buffer[n] = '\0'; // Null-terminate the received data

        //usleep(667); 

        // Envoyer le buffer au destinataire
        if (sendto(send_sockfd, buffer, BUFFER_SIZE, 0, (const struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
            perror("Send failed");
            close(sockfd);
            close(send_sockfd);
            exit(EXIT_FAILURE);
        }
        sequence_number ++;
    }

    // Fermer les sockets (en réalité, ce point ne sera jamais atteint à cause de la boucle infinie)
    close(sockfd);
    close(send_sockfd);
    printf("Number of messages sent : %d\n", sequence_number);
    return 0;
}