#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>


#define BUFFER_SIZE 4992
#define DEST_PORT 9123
#define INTERVAL 10 // En millisecondes

void* launch_target(void* arg) {
    // Lancement du fichier data bridge
    char num1_str[10];
    char num2_str[10];
    char num3_str[10];

    snprintf(num1_str, sizeof(num1_str), "%d", BUFFER_SIZE);
    snprintf(num2_str, sizeof(num2_str), "%d", DEST_PORT);
    snprintf(num3_str, sizeof(num3_str), "%d", INTERVAL);

    char *args[] = {"./t1", num1_str, num2_str, num3_str, NULL};

    printf("Launching target program...\n");
    
    // Use execvp to launch the target program with arguments
    if (execvp(args[0], args) == -1) {
        perror("execvp failed");
    }
    return NULL;
}

int main() {
    size_t buffer_length = 0;
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    unsigned char buffer[BUFFER_SIZE];
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
    server_addr.sin_port = htons(DEST_PORT);

    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    
    /*
    // Lancement app dans un thread
    pthread_t thread_id;

    // Create a new thread to run the target executable
    if (pthread_create(&thread_id, NULL, launch_target, NULL) != 0) {
        perror("pthread_create failed");
        return 1;
    }
    */
   
    while(1)
    {
        printf("Waiting ..\n");
        ssize_t n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
        if (n < 0) {
            perror("Receive failed");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        buffer[n] = '\0';
        printf("Message number %s received\n", buffer);
    } 

}