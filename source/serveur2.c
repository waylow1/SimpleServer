#include <stdio.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <string.h>
#include <fcntl.h> 
#include <unistd.h> 
#include <pthread.h>
#include <sys/wait.h>

#include "matiere.c"

#define MAX 256
#define IPSIZE 16
#define MAX_RESPONSE 1024
#define LIST_MATIERE_SIZE 6

Matiere listeMatieres[] =
{
    { 1,"Anglais", 12.5},
    { 2,"Mathematiques", 10.8},
    { 3,"Informatique", 9.3},
    { 4,"Culture generale", 12.6},
    { 5,"Physiques", 11.8},
    { 6,"Programmation systeme", 12.3}
};

/**
 * @brief fetch the current device Ip into a string
 * @param str the char[] to fill
 */
void getCurrentIp(char str[])
{
    int fd;
    struct ifreq ifr;

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    ifr.ifr_addr.sa_family = AF_INET;

    strncpy(ifr.ifr_name, "wlo1", IFNAMSIZ-1);

    ioctl(fd, SIOCGIFADDR, &ifr);

    close(fd);

    strcpy(str, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
}

/**
 * @brief Generate the response to send to the client
 * @param matiere the matiere to send  
 * @param response the response to fill
 */
void generateResponse(Matiere matiere, char response[])
{
    snprintf(response, MAX_RESPONSE, "Subject: %s, Moyenne: %f", matiere.nom, matiere.moyenne);
}


/**
 * @brief Read and display message from a client socket. After : reply to the client
 * @param sockfd the socket to read
 */
void readContent(int sockfd)
{
    char buff[MAX]; 
    
    bzero(buff, MAX); 
    
    read(sockfd, buff, sizeof(buff)); 
    
    printf("[LOG] - Client Message: %s\n", buff); 
    
    int subjectId = atoi(buff);
    
    printf("[LOG] - Subject ID: %d \n", subjectId);
    
    Matiere selected;
    
    for(int i = 0; i<LIST_MATIERE_SIZE; i++)
    {
        if(listeMatieres[i].type == subjectId)
        {
            selected = listeMatieres[i];
            break;
        }
    }


    char response[MAX_RESPONSE];

    generateResponse(selected, response);

    // sleep(5); For the test

    write(sockfd, response, sizeof(response));

    bzero(buff, MAX); 
}

/**
 * @brief Handle the connection of a client for a specific thread
 * 
 * @param args arguments of the connection
 */
void *handleConnection(void *args)
{
    int connection = *((int *) args);
    readContent(connection);
    close(connection);
    pthread_exit(NULL);
}

/**
 * @brief Create a server and run it
 * @return the sockfd value
 */
int run()
{
    //Init variables
    int binder;
    int sockfd;   
    int connection;

    char ip[IPSIZE];

    socklen_t len;

    struct sockaddr_in sockaddress;
    struct sockaddr_in client;

    //Create the socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1){
        perror("[ERROR] - Socket Creation Failed \n");
    }else{
        printf("[LOG] - Socket Create ! \n");
    }

    //Attributes the IP to the socket
    sockaddress.sin_family = AF_INET;
    sockaddress.sin_port = htons(50000);
    sockaddress.sin_addr.s_addr = htonl(INADDR_ANY);
    
    //Bind the socket to the IP
    binder = bind(sockfd, (struct sockaddr *) &sockaddress, sizeof(struct  sockaddr_in)); 
    if(binder != 0)
    {
        perror("[ERROR] - Socket Binding Failed \n");
        exit(0);
    }else{
        printf("[LOG] - Socket Bind Successully \n"); 
    }

    //Make server ready to listen
    if((listen(sockfd, 5)) != 0){
        perror("[ERROR] - Listen Failed \n");
    }else{
        printf("[LOG] - Server Listening \n");
    }

    len = sizeof(client);
    getCurrentIp(ip);

    printf("------------------------------------\n");
    printf("[LOG] - Server configuration : \n \t IP : %s \n",ip);
    printf("------------------------------------\n");

    //Create connection for accept datas /!\ Log only when client trying to connect
    while(1)
    {
        connection = accept(sockfd, (struct sockaddr*)&client, &len);

        if(connection < 0)
        {
            perror("[ERROR] - Server Acceptation Failed \n");
        }
        else
        {
            printf("[LOG] - Server Ready To Accept Connection \n");
            
            pthread_t thread; 
            int *newsockfd = malloc(sizeof(int)); //Allocate memory for the new socket
            *newsockfd = connection;

            if (pthread_create(&thread, NULL, handleConnection, (void *)newsockfd) != 0) //Thread creation
            {
                perror("[ERROR] - Could not create thread");
                free(newsockfd);
            }
            else
            {

                pthread_detach(thread); //Detach the thread
            }

            readContent(connection);
            close(connection);
        }
    }
    return sockfd;
}