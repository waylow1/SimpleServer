### Fonctions du serveur

```c
/**
 * @brief fetch the current device Ip into a string
 * @param str the char[] to fill
 */
void getCurrentIp(char str[])
```

Retourne l'IP du serveur afin de l'afficher. Est utile lors de l'envoie du message au client. 

---

```c
/**
 * @brief Generate the response to send to the client
 * @param matiere the matiere to send  
 * @param response the response to fill
 */
void generateResponse(Matiere matiere, char response[])
```

génère la réponse a envoyé au client.

---

```c
/**
 * @brief Read and display message from a client socket. After : reply to the client
 * @param sockfd the socket to read
 */
void readContent(int sockfd)
{
```

Lis le contenu du message client, l'affiche dans la console, et renvoie la réponse au client avec la fonction `generateResponse`

---

```c
/**
 * @brief Create a server and run it
 * @return the sockfd value
 */
int run()
```

Créer un serveur à partir d'un socket et l'éxecute jusqu'à la réception d'un message de la part d'un client. 
