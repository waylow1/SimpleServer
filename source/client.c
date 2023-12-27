#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define port 50000
#define MAX_BUFFER_SIZE 1024

int main(int argc,char ** argv){

    char * ip_serveur = argv[1];
    struct sockaddr_in adresse_serveur;
    
    if (argc != 2) {
        printf("Nombre de paramètres eronné\n");
        exit(EXIT_FAILURE);
    }

    printf("Début pour le client\n");

    int socketPourClient = socket(AF_INET, SOCK_STREAM, 0);
    if (socketPourClient == -1) {
        perror("Erreur lors de la création de la socket");
        exit(EXIT_FAILURE);
    }

    adresse_serveur.sin_family = AF_INET;
    adresse_serveur.sin_port = htons(port);
    if (inet_pton(AF_INET, ip_serveur, &adresse_serveur.sin_addr) <= 0) {
        perror("Erreur lors de la conversion de l'adresse IP");
        close(socketPourClient);
        exit(EXIT_FAILURE);
    }

    if (connect(socketPourClient, (struct sockaddr *)&adresse_serveur, sizeof(adresse_serveur)) == -1) {
        perror("Erreur lors de la connexion au serveur");
        close(socketPourClient);
        exit(EXIT_FAILURE);
    }
    int choix;
    do {
        printf("Veuillez saisir un nombre entre 1 et 6 : ");
        scanf("%d", &choix);

        if (choix < 1 || choix > 6) {
            printf("La saisie n'est pas valide. Veuillez réessayer.\n");
        }
    }while (choix < 1 || choix > 6); 

    char message[2];  
    snprintf(message, sizeof(message), "%d", choix);

    if (send(socketPourClient, message, strlen(message), 0) == -1) {
        perror("Erreur lors de l'envoi du message");
    } else {
        printf("Message envoyé avec succès : %s\n", message);
    }

    char buffer[MAX_BUFFER_SIZE];
    ssize_t received_bytes = recv(socketPourClient, buffer, sizeof(buffer), 0);

    if (received_bytes == -1) {
        perror("Error receiving response");
    } else {
        printf("%s\n", buffer);
    }

    close(socketPourClient);

    return 0;
}