# Socket & Tubes

Ce programme fusionne ce qu'on avait vu dans les tubes et dans la partie précédente des sockets, en
effet dans cette partie, le serveur ne subira aucun changement par rapport à la précédente, nous allons
donc nous focaliser sur les changements que nous avons apporté au client.
Alors que le client s'occupait de l'incrémentation du jeton dans le programme précédent, nous allons
délèguer cette tâche à un processus fils que le client va génèrer.

    pID = fork();

On génère donc un processus

    if(pID > 0) { (1)
      if(connect(sock, (SOCKADDR*)&sin, sizeof(sin)) != SOCKET_ERROR) { (2)
      do {
        n = recv(sock, &JETON, sizeof(JETON), 0); (3)
        ...
        if(JETON != -2) {
          close(pipeParent[READ]); (4)
          write(pipeParent[WRITE], &JETON, sizeof(JETON));
          close(pipeChild[WRITE]); (5)
          read(pipeChild[READ], &JETON, sizeof(JETON));
          if(((JETON) % 3) == 0) { (6)
            if(JETON > 50) JETON = -2;
          }
        }

        if(JETON != -2) printf("client> JETON = %d\n", JETON);
        n = send(sock, &JETON, sizeof(JETON), 0);
        ...
      } while(JETON != -2);
      exit(1);
    }

traitement que le père doit effectuer
connexion au serveur
reception du jeton depuis le serveur
envoie du jeton au fils pour l'incrémenter
reception du jeton incrémenté
vérification si c'est le premier client pour gérer ou non la terminaison des processus
envoie du jeton au serveur

    if(pID == 0) (1) {
      do {
        close(pipeParent[WRITE]); (2)
        read(pipeParent[READ], &JETON, sizeof(JETON));
        if(JETON != -2) {
          JETON++; (3)
          close(pipeChild[READ]); (4)
          write(pipeChild[WRITE], &JETON, sizeof(JETON));
        }
      } while(JETON != -2); (5)
      exit(1);
    }

traitement concernant le fils
réception du jeton envoyé par le père
incrémentation
envoie du jeton au client/père
tanque la valeur du jeton est différente de -2, le cas échéant, on gère la terminaison du fils

