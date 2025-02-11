#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>

int maxfd =  0, count = 0;
char rbuf[300000], wbuf[300000];
fd_set rfds, wfds, afds;

typedef struct s_client {
	int id;
	char msg[290000];
} t_client;

t_client clients[1024];

void fatal_error(int num) {
	switch (num) {
		case 1:
		    write(2, "Wrong number of arguments\n", 26);
			break;
		case 2:
			write(2, "Fatal error\n", 12);
			break;
	}
	exit (1);
}

void broadcast(int sender) {
	for (int fd = 0; fd <= maxfd; fd++) {
		if (FD_ISSET(fd, &wfds) && fd  != sender) {
			if (send(fd, wbuf, strlen(wbuf), 0) == -1)
				fatal_error(2);
		}
	}
}

int main(int ac, char **av) {
	if (ac != 2)
		fatal_error(1);
	FD_ZERO(&afds);
	int sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1)
		fatal_error(2);
	maxfd = sockfd;
	FD_SET(sockfd, &afds);
	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	bzero(clients, sizeof(clients));
	socklen_t len = sizeof(servaddr);

	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	servaddr.sin_port = htons(atoi(av[1])); 

	if ((bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0)
		fatal_error(2);
	if (listen(sockfd, 128) != 0)
		fatal_error(2);

	while(1) {
		rfds = wfds = afds;
		if (select(maxfd + 1, &rfds, &wfds, 0, 0) == -1)
			continue;
		for (int fd  = 0; fd <= maxfd; fd++) {
			if (FD_ISSET(fd, &rfds)) {
				if (fd == sockfd) {
					int new = accept(sockfd, (struct sockaddr *)&servaddr, &len);
					if (new >= 0) {
						if (maxfd < new)
							maxfd = new;
						clients[new].id = count++;
						FD_SET(new, &afds);
						sprintf(wbuf, "server: client %d just arrived\n", clients[new].id);
						broadcast(new);
					}
				} else  {
					int ret = recv(fd, rbuf, sizeof(rbuf), 0);
					if (ret <= 0) {
						sprintf(wbuf, "server: client %d just left\n", clients[fd].id);
						broadcast(fd);
						FD_CLR(fd, &afds);
						close(fd);
						bzero(clients[fd].msg, strlen(clients[fd].msg));
					} else {
						for (int i = 0, j = strlen(clients[fd].msg); i < ret; i++, j++) {
							clients[fd].msg[j] = rbuf[i];
							if (clients[fd].msg[j] == '\n') {
								clients[fd].msg[j] = 0;
								sprintf(wbuf, "client %d: %s\n", clients[fd].id, clients[fd].msg);
								broadcast(fd);
								bzero(clients[fd].msg, strlen(clients[fd].msg));
								j = -1;
							}
						}
					}
				}
				break;
			}
		}
	}
	return (0);
}

/*
Ce code est un programme en C qui implémente un serveur TCP simple capable de gérer plusieurs clients en utilisant les appels système de sockets et la fonction `select` pour la gestion des entrées/sorties. Voici une explication détaillée de chaque partie du code :

### Bibliothèques Incluses

- **`#include <string.h>`** : Pour les fonctions de manipulation de chaînes de caractères.
- **`#include <unistd.h>`** : Pour les appels système POSIX, comme `write` et `close`.
- **`#include <sys/socket.h>`** : Pour les fonctions de création et de manipulation de sockets.
- **`#include <netinet/in.h>`** : Pour les structures et constantes nécessaires aux sockets Internet.
- **`#include <stdio.h>`** : Pour les fonctions d'entrée/sortie standard.
- **`#include <stdlib.h>`** : Pour les fonctions de gestion de la mémoire et de contrôle de processus.
- **`#include <sys/select.h>`** : Pour la fonction `select`, qui permet de surveiller plusieurs descripteurs de fichiers.

### Variables Globales

- **`maxfd`** : Le plus grand descripteur de fichier actuellement utilisé.
- **`count`** : Compteur pour attribuer un identifiant unique à chaque client.
- **`rbuf`** et **`wbuf`** : Buffers pour la lecture et l'écriture des données.
- **`rfds`, `wfds`, `afds`** : Ensembles de descripteurs de fichiers pour `select`.
- **`clients`** : Tableau de structures pour stocker les informations des clients.

### Structure `t_client`

- **`id`** : Identifiant unique du client.
- **`msg`** : Message reçu du client.

### Fonctions

- **`fatal_error(int num)`** : Affiche un message d'erreur et termine le programme.
- **`broadcast(int sender)`** : Envoie un message à tous les clients connectés, sauf à l'expéditeur.

### Fonction `main`

1. **Vérification des Arguments** : Le programme nécessite exactement un argument (le numéro de port).

2. **Initialisation des Sockets** :
   - Crée un socket TCP avec `socket()`.
   - Configure l'adresse du serveur avec `bzero` et `htonl`/`htons`.
   - Lie le socket à l'adresse et au port spécifiés avec `bind()`.
   - Met le socket en mode écoute avec `listen()`.

3. **Boucle Principale** :
   - Utilise `select` pour surveiller les descripteurs de fichiers.
   - Accepte les nouvelles connexions avec `accept()`.
   - Reçoit les données des clients avec `recv()`.
   - Diffuse les messages reçus à tous les clients avec `broadcast()`.
   - Gère la déconnexion des clients en fermant les descripteurs de fichiers et en nettoyant les ressources.

### Fonctionnement Général

Le serveur accepte les connexions des clients, lit les messages qu'ils envoient, et les diffuse à tous les autres clients connectés. Lorsqu'un client se déconnecte, un message est également diffusé.

Ce code est un bon exemple de gestion de serveur multi-clients en C en utilisant des sockets et la fonction `select` pour gérer les entrées/sorties de manière non bloquante.*/