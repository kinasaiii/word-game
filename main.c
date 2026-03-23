#include <stdio.h>      // Pour les fonctions d'entree/sortie (printf, scanf, fopen, etc...)
#include <stdlib.h>     // Pour les fonctions utilitaires (exit, rand, srand)
#include <string.h>     // Pour manipuler les chaines de caracteres (strcmp, strlen...)
#include <time.h>       // Pour initialiser le generateur de nombres aleatoires
#include <ctype.h>      // Pour les fonctions sur les caracteres (toupper...)
#include "raylib.h"

// Structure representant les informations d'un joueur
typedef struct {
    char nom[20];
    char prenom[20];
    int age;
    int niveau;
    int score;
} Joueur;

// Verifie si un joueur existe deja dans le fichier, sinon enregistre un nouveau joueur
Joueur userCheck(char *prenom) {
    FILE *fileJoueur = fopen("joueur.txt", "r");
    int trouve = 0;
    Joueur j;

    // Lecture du fichier pour chercher le joueur
    if (fileJoueur != NULL) {
        while (fscanf(fileJoueur, "%s %s : %d ans | niveau: %d | score: %d\n", j.prenom, j.nom, &j.age, &j.niveau, &j.score) != EOF) {
            if (strcmp(prenom, j.prenom) == 0) { // Compare le prenom entrer et le prenom dans le fichier
                trouve = 1;
                printf("\nBienvenue de retour %s!\n", j.prenom);
                break;
            }
        }
        fclose(fileJoueur);
    }

    // Si le joueur n'existe pas, on l'enregistre
    if (!trouve) {
        strcpy(j.prenom, prenom);
        printf("Nouveau utilisateur!\n");
        printf("Veuillez entrer votre nom: ");
        scanf("%s", j.nom);
        printf("Veuillez entrer votre age: ");
        scanf("%d", &j.age);
        j.niveau = 1;
        j.score = 0;

        FILE *fileJoueur2 = fopen("joueur.txt", "a");
        if (fileJoueur2 == NULL) {
            printf("ERREUR! Impossible de charger le fichier.");
            exit(1);
        }

        // Ecriture des infos dans le fichier
        fprintf(fileJoueur2, "%s %s : %d ans | niveau: %d | score: %d\n", j.prenom, j.nom, j.age, j.niveau, j.score);
        fclose(fileJoueur2);
        printf("Utilisateur enregistre avec succes!\n");
        printf("\nBienvenue dans le jeu!\n");
    }

    return j;
}

// Genere 10 lettres aleatoires (4 voyelles et 6 consonnes)
void genererLettres(char *lettres) {
    srand(time(NULL));  // Initialisation du generateur aleatoire

    char voyelle[] = "AEIOU";
    char alphabet[] = "BCDFGHJKLMNPQRSTVWXYZ";
    int i;

    // Ajout de 4 voyelles
    for (i = 0; i < 4; i++) {
        int index = rand() % 5;
        lettres[i] = voyelle[index];
    }

    // Ajout de 6 consonnes
    for (; i < 10; i++) {
        int index = rand() % 21;
        lettres[i] = alphabet[index];
    }

    // Melange des lettres
    for (i = 0; i < 10; i++) {
        int j = rand() % 10;
        char temp = lettres[i];
        lettres[i] = lettres[j];
        lettres[j] = temp;
    }

    lettres[10] = '\0'; // Fin de chaine
}

// Verifie si un mot peut etre forme avec les lettres proposees
int checkMot(char *mot, char *lettres) {
    for (int i = 0; i < strlen(mot); i++) {
        if (strchr(lettres, toupper(mot[i])) == NULL) { // Verifie si la lettre mot[i] convertie en majuscule ne se trouve pas dans la chaine lettres
            return 0; // Lettre non presente dans la liste
        }
    }
    return strlen(mot); // Mot valide, retourne sa longueur
}

// Verifie si le mot existe dans le fichier dictionnaire
int motDictionnaire(char *mot) {
    FILE *file = fopen("dictionnaire.txt", "r");
    if (file == NULL) {
        printf("ERREUR! Dictionnaire introuvable.\n");
        return 0;
    }

    char motDic[20];
    while (fscanf(file, "%s", motDic) != EOF) {
        if (strcmp(motDic, mot) == 0) { // Compare le mot entrer par l'utilisateur et les mots du dictionnaire
            fclose(file);
            return 1; // Mot trouve
        }
    }

    fclose(file);
    return 0; // Mot non trouve
}

// Sauvegarde les informations du joueur dans le fichier
void sauvegarderJoueur(Joueur joueur) {
    FILE *fancien = fopen("joueur.txt", "r");
    FILE *fnouveau = fopen("temp.txt", "w");

    if (fancien == NULL || fnouveau == NULL) {
        printf("ERREUR! Impossible de charger le fichier.\n");
        return;
    }

    Joueur j;
    int trouve = 0;

    // Copier tous les joueurs sauf celui a mettre a jour
    while (fscanf(fancien, "%s %s : %d ans | niveau: %d | score: %d\n", j.prenom, j.nom, &j.age, &j.niveau, &j.score) != EOF) {
        if (strcmp(j.prenom, joueur.prenom) == 0) {
            // Ecrase les anciennes infos par les nouvelles
            fprintf(fnouveau, "%s %s : %d ans | niveau: %d | score: %d\n", joueur.prenom, joueur.nom, joueur.age, joueur.niveau, joueur.score);
            trouve = 1;
        } else {
            fprintf(fnouveau, "%s %s : %d ans | niveau: %d | score: %d\n", j.prenom, j.nom, j.age, j.niveau, j.score);
        }
    }

    // Si joueur non trouve, l'ajouter
    if (!trouve) {
        fprintf(fnouveau, "%s %s : %d ans | niveau: %d | score: %d\n", joueur.prenom, joueur.nom, joueur.age, joueur.niveau, joueur.score);
    }

    fclose(fancien);
    fclose(fnouveau);
    remove("joueur.txt");              // Supprime l' ancien fichier
    rename("temp.txt", "joueur.txt");  // Renomme le fichier temporaire
    printf("Progression sauvegardee au niveau %d.\nAu revoir!\n", joueur.niveau);
}
int main() {
    // Initialization
    const int screenWidth = 800; // width de la fenetre
    const int screenHeight = 800; // height de la fenetre
    InitWindow(screenWidth, screenHeight, "Jeu de mots"); // Initialisation de la fenetre du jeu
    SetTargetFPS(60); // 60 images par seconde

    int choix, continuer, longueurMot, nombreMots, motsValides, motExiste, points, dejaUtilise;
    char finalChoix, lettres[11], prenom[20], mot[20];
    char motsJoues[10][20];
    Joueur joueur;

    // main loop
    while (!WindowShouldClose()) {
        // Dessiner le menu ou les elements du jeu ici
        BeginDrawing();
        ClearBackground(RAYWHITE);
         // === Phase d'accueil du joueur ===
    printf("\n=== LETTRES A MOTS ===\n");
    printf("Entrer votre prenom: ");
    scanf("%s", prenom);

    // Appel a userCheck() pour charger les infos du joueur ou l'enregistrer
    joueur = userCheck(prenom);

    // === Menu principal (choix de la partie) ===
    do {
        printf("=== Menu de jeu ===\n");
        printf("1. Nouvelle partie\n");
        printf("2. Reprendre partie\n");
        printf("3. Quitter le jeu\n");
        printf("Entrer votre choix (1/2/3): ");
        scanf("%d", &choix);
        printf("\n");

        switch (choix) {
            case 1:
                // Initialisation pour une nouvelle partie
                joueur.niveau = 1;
                joueur.score = 0;
                printf("Nouvelle partie commence!\n");
                printf("Vous etes au niveau %d avec un score %d\n", joueur.niveau, joueur.score);
                break;
            case 2:
                // Reprise de la partie precedente
                printf("Reprendre la derniere partie!\n");
                printf("Vous etes au niveau %d avec un score %d\n", joueur.niveau, joueur.score);
                break;
            case 3:
                // Sortie du jeu
                printf("Au revoir!\n");
                return 0;
            default:
                printf("Choix invalide! Reessayer.\n");
        }
    } while (choix != 1 && choix != 2 && choix != 3);

    // === Boucle principale du jeu ===
    continuer = 1;
    while (continuer) {
        // Generation aleatoire des 10 lettres pour ce niveau
        genererLettres(lettres);

        // Calcul du nombre de mots a trouver selon le niveau
        nombreMots = 1 + ((joueur.niveau - 1) / 4);

        // Definir la longueur des mots pour ce niveau
        longueurMot = 2 + ((joueur.niveau - 1) % 4);

        // Affichage des infos du niveau actuel
        printf("\n--- Niveau %d ---\n", joueur.niveau);
        printf("Lettres disponibles: ");
        for (int i = 0; i < 10; i++) {
            printf("%c ", lettres[i]);
        }

        printf("\nFormer %d mot(s) de %d lettres pour chacun. Entrer (stop) pour quitter.\n\n", nombreMots, longueurMot);

        motsValides = 0; // Compteur de mots valides saisis

        // Boucle pour que le joueur entre les bons mots
        while (motsValides < nombreMots) {
            motExiste = 0;

            do {
                printf("Mot %d/%d: ", motsValides + 1, nombreMots);
                scanf("%s", mot);

                // Le joueur peut quitter le jeu a tout moment
                if ((strcmp(mot, "stop") == 0) || (strcmp(mot, "STOP") == 0)) {
                    printf("Jeu termine. Score final: %d\n", joueur.score);
                    sauvegarderJoueur(joueur);
                    return 0;
                }

                // Convertir le mot en majuscules
                for (int i = 0; mot[i] != '\0'; i++) { // parcourir jusqu'a la fin de la chaine
                    mot[i] = toupper(mot[i]);
                }

                // Verifier que le mot n'a pas deja ete utilise
                dejaUtilise = 0;
                for (int k = 0; k < motsValides; k++) {
                    if (strcmp(motsJoues[k], mot) == 0) { //compare les mots deja utilises avec le mot entrer
                        dejaUtilise = 1;
                        break;
                    }
                }

                if (dejaUtilise) {
                    printf("Vous avez deja utilise ce mot. Essayez un autre.\n");
                    continue;
                }

                // Verifier la validite du mot : lettres + dictionnaire
                points = checkMot(mot, lettres);
                if (points == longueurMot && motDictionnaire(mot)) {
                    printf("Mot valide! +%d points.\n", points);
                    joueur.score += points;
                    motsValides++;

                    // Sauvegarder le mot utilise
                    strcpy(motsJoues[motsValides - 1], mot);

                    motExiste = 1;
                } else {
                    printf("Mot invalide! Reessayer.\n");
                }

            } while (!motExiste);
        }

        // Fin du niveau
        printf("Niveau %d termine! score: %d\n", joueur.niveau, joueur.score);

        // Demander au joueur s'il veut continuer
        do {
            printf("Passez au niveau suivant? (o/n): ");
            while (getchar() != '\n'); // Vider le buffer
            scanf("%c", &finalChoix);

            switch (toupper(finalChoix)) {
                case 'O': // oui, continue au niveau suivant
                    joueur.niveau++;
                    printf("\n--> Au niveau suivant:\n");
                    break;
                case 'N': //non, quitter
                    joueur.niveau++;
                    sauvegarderJoueur(joueur);
                    printf("\nProgression sauvegardee.\nAu revoir!");
                    continuer = 0;
                    break;
                default:
                    printf("Choix invalide! Veuillez entrer O ou N.\n");
            }
        } while (toupper(finalChoix) != 'O' && toupper(finalChoix) != 'N');
    }
    }
    
    // === Phase d'accueil du joueur ===
    /* printf("\n=== LETTRES A MOTS ===\n");
    printf("Entrer votre prenom: ");
    scanf("%s", prenom);

    // Appel a userCheck() pour charger les infos du joueur ou l'enregistrer
    joueur = userCheck(prenom);

    // === Menu principal (choix de la partie) ===
    do {
        printf("=== Menu de jeu ===\n");
        printf("1. Nouvelle partie\n");
        printf("2. Reprendre partie\n");
        printf("3. Quitter le jeu\n");
        printf("Entrer votre choix (1/2/3): ");
        scanf("%d", &choix);
        printf("\n");

        switch (choix) {
            case 1:
                // Initialisation pour une nouvelle partie
                joueur.niveau = 1;
                joueur.score = 0;
                printf("Nouvelle partie commence!\n");
                printf("Vous etes au niveau %d avec un score %d\n", joueur.niveau, joueur.score);
                break;
            case 2:
                // Reprise de la partie precedente
                printf("Reprendre la derniere partie!\n");
                printf("Vous etes au niveau %d avec un score %d\n", joueur.niveau, joueur.score);
                break;
            case 3:
                // Sortie du jeu
                printf("Au revoir!\n");
                return 0;
            default:
                printf("Choix invalide! Reessayer.\n");
        }
    } while (choix != 1 && choix != 2 && choix != 3);

    // === Boucle principale du jeu ===
    continuer = 1;
    while (continuer) {
        // Generation aleatoire des 10 lettres pour ce niveau
        genererLettres(lettres);

        // Calcul du nombre de mots a trouver selon le niveau
        nombreMots = 1 + ((joueur.niveau - 1) / 4);

        // Definir la longueur des mots pour ce niveau
        longueurMot = 2 + ((joueur.niveau - 1) % 4);

        // Affichage des infos du niveau actuel
        printf("\n--- Niveau %d ---\n", joueur.niveau);
        printf("Lettres disponibles: ");
        for (int i = 0; i < 10; i++) {
            printf("%c ", lettres[i]);
        }

        printf("\nFormer %d mot(s) de %d lettres pour chacun. Entrer (stop) pour quitter.\n\n", nombreMots, longueurMot);

        motsValides = 0; // Compteur de mots valides saisis

        // Boucle pour que le joueur entre les bons mots
        while (motsValides < nombreMots) {
            motExiste = 0;

            do {
                printf("Mot %d/%d: ", motsValides + 1, nombreMots);
                scanf("%s", mot);

                // Le joueur peut quitter le jeu a tout moment
                if ((strcmp(mot, "stop") == 0) || (strcmp(mot, "STOP") == 0)) {
                    printf("Jeu termine. Score final: %d\n", joueur.score);
                    sauvegarderJoueur(joueur);
                    return 0;
                }

                // Convertir le mot en majuscules
                for (int i = 0; mot[i] != '\0'; i++) { // parcourir jusqu'a la fin de la chaine
                    mot[i] = toupper(mot[i]);
                }

                // Verifier que le mot n'a pas deja ete utilise
                dejaUtilise = 0;
                for (int k = 0; k < motsValides; k++) {
                    if (strcmp(motsJoues[k], mot) == 0) { //compare les mots deja utilises avec le mot entrer
                        dejaUtilise = 1;
                        break;
                    }
                }

                if (dejaUtilise) {
                    printf("Vous avez deja utilise ce mot. Essayez un autre.\n");
                    continue;
                }

                // Verifier la validite du mot : lettres + dictionnaire
                points = checkMot(mot, lettres);
                if (points == longueurMot && motDictionnaire(mot)) {
                    printf("Mot valide! +%d points.\n", points);
                    joueur.score += points;
                    motsValides++;

                    // Sauvegarder le mot utilise
                    strcpy(motsJoues[motsValides - 1], mot);

                    motExiste = 1;
                } else {
                    printf("Mot invalide! Reessayer.\n");
                }

            } while (!motExiste);
        }

        // Fin du niveau
        printf("Niveau %d termine! score: %d\n", joueur.niveau, joueur.score);

        // Demander au joueur s'il veut continuer
        do {
            printf("Passez au niveau suivant? (o/n): ");
            while (getchar() != '\n'); // Vider le buffer
            scanf("%c", &finalChoix);

            switch (toupper(finalChoix)) {
                case 'O': // oui, continue au niveau suivant
                    joueur.niveau++;
                    printf("\n--> Au niveau suivant:\n");
                    break;
                case 'N': //non, quitter
                    joueur.niveau++;
                    sauvegarderJoueur(joueur);
                    printf("\nProgression sauvegardee.\nAu revoir!");
                    continuer = 0;
                    break;
                default:
                    printf("Choix invalide! Veuillez entrer O ou N.\n");
            }
        } while (toupper(finalChoix) != 'O' && toupper(finalChoix) != 'N');
    } */

    CloseWindow(); // Fermer la fenetre a la fin du jeu

    return 0;
}
