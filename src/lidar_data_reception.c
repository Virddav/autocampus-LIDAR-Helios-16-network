#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

int main() {
    int sequence_number = 0;
    int buffer_count = 0;
    size_t buffer_length = 0;
    // Thread loop
        int sockfd, send_sockfd;
        struct sockaddr_in server_addr, client_addr;
        unsigned char buffer[4992];
        socklen_t addr_len = sizeof(client_addr);

        // Créer la socket pour recevoir les données
        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            perror("Socket creation failed");
            exit(EXIT_FAILURE);
        }
        // Configurer l'adresse du serveur
        memset(&server_addr, 0, sizeof(server_addr));
        memset(&client_addr, 0, sizeof(client_addr));

        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(9000);

        if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            perror("bind failed");
            close(sockfd);
            exit(EXIT_FAILURE);
        }

    while(1)
    {
        ssize_t n = recvfrom(sockfd, buffer, 4992, 0, (struct sockaddr *)&client_addr, &addr_len);
        if (n < 0) {
            perror("Receive failed");
            close(sockfd);
            //close(send_sockfd);
            exit(EXIT_FAILURE);
        }
        sequence_number ++;
        buffer[n] = '\0';
        printf("Message number %d received\n", sequence_number);
    }

}