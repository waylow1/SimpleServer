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

#include "matiere.c"

#define MAX 256
#define IPSIZE 16

Matiere listeMatieres[] =
{
    { 1,"Anglais", 12.5},
    { 2,"Mathematiques", 10.8},
    { 3,"Informatique", 9.3},
    { 4,"Culturte generale", 12.6},
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
 * @brief Read and display message from a client socket
 * @param sockfd the socket to read
 */
void readContent(int sockfd)
{
    char buff[MAX]; 
    bzero(buff, MAX); 
    read(sockfd, buff, sizeof(buff)); 
    printf("[LOG] - Client Message: \n \t%s\n", buff); 
    bzero(buff, MAX); 
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
        printf("[ERROR] - Socket Creation Failed \n");
        exit(0);
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
        printf("[ERROR] - Socket Binding Failed \n");
        exit(0);
    }else{
        printf("[LOG] - Socket Bind Successully \n"); 
    }

    //Make server ready to listen
    if((listen(sockfd, 5)) != 0){
        printf("[ERROR] - Listen Failed \n");
    }else{
        printf("[LOG] - Server Listening \n");
    }

    len = sizeof(client);
    getCurrentIp(ip);

    printf("Server configuration : \n \t IP : %s \n",ip);


    //Create connection for accept datas /!\ Log only when client trying to connect
    connection = accept(sockfd, (struct sockaddr*)&client, &len);
    if(connection < 0){
        printf("[ERROR] - Server Acceptation Failed \n");
    }else{
        printf("[LOG] - Server Ready To Accept Connection \n");
        readContent(connection);
    }
    
    return sockfd;
}