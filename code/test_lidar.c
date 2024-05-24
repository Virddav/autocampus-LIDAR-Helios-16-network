#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#define PORT 6699
#define BUFFER_SIZE 1248

long hex_to_decimal(const char *hex_str) {
    char *endptr;
    errno = 0;  // Pour distinguer l'erreur de strtol
    long decimal = strtol(hex_str, &endptr, 16);

    // Vérification des erreurs
    if (errno != 0) {
        perror("strtol");
        return -1;
    }

    if (endptr == hex_str) {
        fprintf(stderr, "Aucune conversion n'a été effectuée : %s\n", hex_str);
        return -1;
    }

    return decimal;
}

void read_header(unsigned char* packet){
	unsigned char* timestamp = malloc(sizeof(unsigned char) * 10);
	memcpy(timestamp,packet+20,10);
	
      	for (int i=0; i<10;i++){
      		printf("%02X ",timestamp[i]);
      	}
}

void read_datablock (unsigned char* packet){
	for(int i = 0; i<12; i++){
		unsigned char* timestamp = malloc(sizeof(unsigned char) * 100);
		memcpy(timestamp,packet+48+100*i,100);
		
	      	for (int i=0; i<100;i++){
	      		printf("%02X ",timestamp[i]);
	      	}
	      	printf("\n\n\n");
	}
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    unsigned char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(client_addr);

    // Créer une socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configurer l'adresse du serveur
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    // Lier la socket à l'adresse du serveur
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Listening on port %d...\n", PORT);

    // Boucle pour recevoir les paquets
    {
        ssize_t n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
        if (n < 0) {
            perror("Receive failed");
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        buffer[n] = '\0'; // Null-terminate the received data
      	//for (int i=42; i<BUFFER_SIZE-6;i++){
      	//	printf("%02X ",buffer[i]);
      	//}
      	//printf("\n\n");
      	//read_header(buffer);
      	read_datablock(buffer);
    }

    // Fermer la socket (en réalité, ce point ne sera jamais atteint à cause de la boucle infinie)
    close(sockfd);
    return 0;
}
