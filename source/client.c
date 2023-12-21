#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define port 50000

void main(int argc,char ** argv){

    char * ip_serveur = argv[1];
    struct sockaddr_in adresse_serveur;
    
    if (argc != 2) {
        prtintf("Nombre de paramètres eronné\n");
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

    const char * message = "Hello, World!";
    if (send(socketPourClient, message, strlen(message), 0) == -1) {
        perror("Erreur lors de l'envoi du message");
    } else {
        printf("Message envoyé avec succès : %s\n", message);
    }

    close(socketPourClient);

    return 0;
}