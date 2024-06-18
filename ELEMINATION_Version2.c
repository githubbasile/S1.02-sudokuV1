#include <stdio.h>
#include <stdbool.h>

#define N 3
#define TAILLE (N*N)

typedef struct {
    int valeur;
    bool candidats[TAILLE + 1];
    int nbCandidats;
} tCase2;

typedef tCase2 tGrille[TAILLE][TAILLE];

void afficherGrille(tGrille grille);
void initialiserGrille(tGrille grille, const char* fichier);
bool resoudreSudoku(tGrille grille);
bool estUniqueLigne(tGrille grille, int ligne, int valeur);
bool estUniqueColonne(tGrille grille, int colonne, int valeur);
bool estUniqueRegion(tGrille grille, int ligne, int colonne, int valeur);
bool grilleComplete(tGrille grille);
bool grilleComplete(tGrille grille);
bool resoudreSudokuAvecGuess(tGrille grille);
void copierGrille(tGrille source, tGrille destination);


int main() {
    tGrille grille;
    char nomg[20];
    printf("donner le nom du fichier de la grille");
    scanf("%s", nomg);
    initialiserGrille(grille, nomg);

    printf("Grille initiale :\n");
    afficherGrille(grille);

    if (resoudreSudoku(grille)) {
        printf("\nGrille résolue :\n");
        afficherGrille(grille);
    } else {
        printf("\nLa grille n'a pas de solution.\n");
    }

    return 0;
}

// Fonction pour afficher la grille
void afficherGrille(tGrille grille) {
    for (int i = 0; i < TAILLE; i++) {
        for (int j = 0; j < TAILLE; j++) {
            printf("%2d ", grille[i][j].valeur);
        }
        printf("\n");
    }
}

// Fonction pour initialiser la grille à partir d'un fichier .sud
void initialiserGrille(tGrille grille, const char* fichier) {
    FILE* file;
    file = fopen(fichier, "rb");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }

    for (int i = 0; i < TAILLE; i++) {
        for (int j = 0; j < TAILLE; j++) {
            fread(&grille[i][j].valeur, sizeof(int), 1, file);
            if (grille[i][j].valeur != 0) {
                // Si la valeur est non nulle, éliminer tous les candidats
                for (int k = 1; k <= TAILLE; k++) {
                    grille[i][j].candidats[k] = false;
                }
                grille[i][j].nbCandidats = 0;
            }
            else {
                // Si la valeur est nulle, initialiser les candidats de 1 à N*N
                for (int k = 1; k <= TAILLE; k++) {
                    grille[i][j].candidats[k] = true;
                }
                grille[i][j].nbCandidats = TAILLE;
            }
        }
    }

    fclose(file);
}

// Fonction pour résoudre la grille en utilisant la méthode du singleton nu
bool resoudreSudoku(tGrille grille) {
    bool changement = true;

    while (changement) {
        changement = false;

        // Parcourir la grille
        for (int i = 0; i < TAILLE; i++) {
            for (int j = 0; j < TAILLE; j++) {
                // Si la case est vide
                if (grille[i][j].valeur == 0) {
                    // Parcourir les candidats
                    for (int k = 1; k <= TAILLE; k++) {
                        // Si un candidat est unique dans sa ligne, colonne et région
                        if (grille[i][j].candidats[k] &&
                            estUniqueLigne(grille, i, k) &&
                            estUniqueColonne(grille, j, k) &&
                            estUniqueRegion(grille, i, j, k)) {
                            // Remplir la case avec la valeur unique
                            grille[i][j].valeur = k;
                            grille[i][j].nbCandidats = 0;
                            changement = true;
                        }
                    }
                }
            }
        }
    }

    // Vérifier si la grille est entièrement remplie
    if (grilleComplete(grille)) {
        return true; // La grille est résolue
    }

    // Si la grille n'est pas résolue, essayer de deviner une valeur
    return resoudreSudokuAvecGuess(grille);
}

// Fonction pour vérifier si une valeur est unique dans une ligne
bool estUniqueLigne(tGrille grille, int ligne, int valeur) {
    for (int j = 0; j < TAILLE; j++) {
        if (grille[ligne][j].valeur == valeur) {
            return false;
        }
    }
    return true;
}

// Fonction pour vérifier si une valeur est unique dans une colonne
bool estUniqueColonne(tGrille grille, int colonne, int valeur) {
    for (int i = 0; i < TAILLE; i++) {
        if (grille[i][colonne].valeur == valeur) {
            return false;
        }
    }
    return true;
}

// Fonction pour vérifier si une valeur est unique dans une région
bool estUniqueRegion(tGrille grille, int ligne, int colonne, int valeur) {
    int debutLigne = ligne - ligne % N;
    int debutColonne = colonne - colonne % N;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (grille[debutLigne + i][debutColonne + j].valeur == valeur) {
                return false;
            }
        }
    }
    return true;
}

// Fonction pour vérifier si la grille est entièrement remplie
bool grilleComplete(tGrille grille) {
    for (int i = 0; i < TAILLE; i++) {
        for (int j = 0; j < TAILLE; j++) {
            if (grille[i][j].valeur == 0) {
                return false;
            }
        }
    }
    return true;
}

// Fonction pour résoudre la grille en utilisant la méthode du singleton nu avec la supposition
bool resoudreSudokuAvecGuess(tGrille grille) {
    // Trouver une case vide avec au moins un candidat
    int ligne = -1, colonne = -1;
    for (int i = 0; i < TAILLE; i++) {
        for (int j = 0; j < TAILLE; j++) {
            if (grille[i][j].valeur == 0 && grille[i][j].nbCandidats > 0) {
                ligne = i;
                colonne = j;
                break;
            }
        }
        if (ligne != -1) {
            break;
        }
    }

    // Si aucune case vide avec un candidat n'est trouvée, la grille est probablement incorrecte
    if (ligne == -1) {
        return false;
    }

    // Essayer chaque candidat dans la case
    for (int k = 1; k <= TAILLE; k++) {
        if (grille[ligne][colonne].candidats[k]) {
            // Faire une copie de la grille
            tGrille grilleCopie;
            copierGrille(grille, grilleCopie);

            // Remplir la case avec le candidat
            grilleCopie[ligne][colonne].valeur = k;
            grilleCopie[ligne][colonne].nbCandidats = 0;

            // Résoudre la grille modifiée
            if (resoudreSudoku(grilleCopie)) {
                // Si la grille est résolue, copier la solution dans la grille d'origine
                copierGrille(grilleCopie, grille);
                return true;
            }
        }
    }

    // Si aucun candidat ne fonctionne, la supposition était incorrecte
    return false;
}

// Fonction pour copier une grille
void copierGrille(tGrille source, tGrille destination) {
    for (int i = 0; i < TAILLE; i++) {
        for (int j = 0; j < TAILLE; j++) {
            destination[i][j].valeur = source[i][j].valeur;
            destination[i][j].nbCandidats = source[i][j].nbCandidats;
            for (int k = 1; k <= TAILLE; k++) {
                destination[i][j].candidats[k] = source[i][j].candidats[k];
            }
        }
    }
}

