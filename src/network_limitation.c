#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 6699
#define BUFFER_SIZE 1248
#define ALPHA_MOY 10
srand(time(0));

float calcul_alpha(int cpt, float * tab_alpha){
    float signal_quality = tab_alpha[(cpt-1)%ALPHA_MOY] + (rand() % 7) - 3;
    float new_alpha = 1.5 * signal_quality + 145;
    tab_alpha[cpt%ALPHA_MOY] = new_alpha;
    float alpha = 0;
    for (int i = 0; i < ALPHA_MOY; i++){
        alpha += tab_alpha[i];
    }
    current alpha = alpha / ALPHA_MOY;
    cpt++;
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
    int cpt = 0; // we recalcul alpha every 100 packets
    int tab_alpha[sizeof(ALPHA_MOY*int)]; // array with previous alpha 
    int main_cpt = 0; // how many packet we skipped 
    int test = 0; // number of treatment 
    int test2 = 0; // number of connexion
    int cond = 0; // condition d'arrêt
    
    clock_t start, end;
    start = clock();

    while cond {
        
        if (cpt > 100){ 
            cpt = 0;
            alpha = calcul_alpha(cpt, tab_alpha);
        }
        ssize_t n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
        if (n < 0) {
            perror("Receive failed");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        buffer[n] = '\0'; // Null-terminate the received data

        if (main_cpt >= alpha){
            // Traitement de la data ici
            test++;
            main_cpt = 0;
        }
        else{
            main_cpt++;
        }
        test2++;
        if (test2>100000){
            cond = 1;
        }
    }
    // Capture du temps de fin
    end = clock();

    // Calcul du temps CPU utilisé
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("Number of packet read : %d", test2)
    printf("Number of packet processed : %d", test)
    printf("Temps d'éxecution : %f", cpu_time_used)


    // Fermer la socket (en réalité, ce point ne sera jamais atteint à cause de la boucle infinie)
    close(sockfd);
    return 1;
}
