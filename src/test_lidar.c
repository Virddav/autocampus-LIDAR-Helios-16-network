#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#define PORT 6699
#define BUFFER_SIZE 1248

/**
 *
 * @file config.c
 * @brief This file represents a "C" application that catches UDP packet from HELIOS16 to extract and interpret the relevant data
 *
 *
 **/



/**
 *
 * @brief This function extracts the time stamp from the UDP packet
 * @param packet A string of hexadecimal values to extract the time stamp from
 *
 **/

void read_header(unsigned char* packet){
	unsigned char* timestamp = malloc(sizeof(unsigned char) * 10);
	memcpy(timestamp,packet+20,10);
    int t_sec = 0;
    int t_msec = 0;
    for (int i=0; i<6; i++){
            t_sec += (unsigned char)timestamp[i];
    }

    for (int i=6;i<10;i++){
        t_msec += (unsigned char) timestamp[i];
    }
        printf("Temps en sec : %d\n",t_sec);
        printf("Temps en microsec : %d\n",t_msec);

}


/**
 *
 *@brief This function processes the MSOP packets to extract azimuth from the Data Block
 * @param packet A string of hexadecimal values to extract the Data Block from
 *
 *
 **/

void read_datablock (unsigned char* packet){
	for(int y = 0; y<12; y++){
		unsigned char* azimuth = malloc(sizeof(unsigned char) * 2);
		memcpy(azimuth,packet+42+100*y,2);
        unsigned char* cloudpoint = malloc(sizeof(unsigned char) * 96);
        memcpy(cloudpoint,packet+42+4+100*y,96);

        printf("Datablock %d : \nAzimuth = %02X%02X\n", y, azimuth[0], azimuth[1]);
	    for (int i=0; i<96;i++){
	      	printf("%02X ",cloudpoint[i]);
	    }
	    printf("\n\n\n");
	}
}

/**
 *
 *@brief The Main function where the connection is set between the HELIOS16 and the Host, and initiates the reception of the UDP packets.
 *
 * :
 *
 *
 **/

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

    // Configurer l'adresse du serveurThis function extracts the time stamp from the UDP paquet
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
      	read_header(buffer);
      	//read_datablock(buffer);
    }

    // Fermer la socket (en réalité, ce point ne sera jamais atteint à cause de la boucle infinie)
    close(sockfd);
    return 0;
}

