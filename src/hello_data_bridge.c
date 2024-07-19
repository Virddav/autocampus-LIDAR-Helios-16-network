#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#define DEST_IP "127.0.0.1" // Localhost

int main(int argc, char *argv[]) {

    /*
    int hello_size = atoi(argv[1]);
    int dest_port = atoi(argv[2]);
    int interval = atoi(argv[3]);
    printf("hello_size %d, dest_port %d, interval%d", hello_size, dest_port, interval);
    */
    int hello_size = 4992;
    int dest_port = 9123;
    int interval = 10;

    int sequence_number = 0;
    int send_sockfd;
    struct sockaddr_in dest_addr;
    
    // Créer la socket pour envoyer les données à raw_its
    if ((send_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Send socket creation failed");
        close(send_sockfd);
        exit(EXIT_FAILURE);
    }

    // Configurer l'adresse du destinataire (raw-its)
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(dest_port);
    dest_addr.sin_addr.s_addr = INADDR_ANY;

    char hello_data[hello_size];
    int hello_count = 0;
    size_t hello_length = 0;
    
    // Boucle pour recevoir et envoyer les paquets
    while (1) {
        snprintf(hello_data, hello_size, "Hello, World! (%d)", hello_count);
        hello_length = strlen(hello_data) + 1;
        
        usleep(10*1000);

        // Envoyer le buffer au destinataire
        if (sendto(send_sockfd, hello_data, hello_size, 0, (const struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
            perror("Send failed");
            close(send_sockfd);
            exit(EXIT_FAILURE);
        }

        hello_count++;
    }

    // Fermer les sockets (en réalité, ce point ne sera jamais atteint à cause de la boucle infinie)
    close(send_sockfd);
    printf("Number of messages sent : %d\n", sequence_number);
    return 0;
}
