#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<dirent.h>
#include<sys/stat.h>
#include<unistd.h>
#include<time.h>
#include<regex.h>


void nom_avec_caracteres_generiques(char str[256], char str2[256]) { // creation du modele utilise par l'expression requliere
    strcat(str2, "^"); // str2 est utilise pour stocker le modele
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == '?') {
            strcat(str2, ".");
        } else if (str[i] == '*') { // ca marche bien
            strcat(str2, ".*");
        } else if (str[i] == '.') {
            strcat(str2, "\\.");
        }
        else {
            str2[strlen(str2)] = str[i];
        }
    }
    strcat(str2, "$");
}


char* formatdate(char* str, time_t val) {
    strftime(str, 36, "%d.%m.%Y %H:%M:%S", localtime(&val));
    return str;
}


void search_from_rep(char pathname[PATH_MAX], char fname[256], regex_t r) {
    char path[PATH_MAX]; // utilisee pour afficher le chemin complet des fichiers
    DIR *rep = opendir(pathname); // ouverture du repertoire
    if (rep != NULL) {
        struct dirent *entree = readdir(rep); // lecture d'une entree
        while (entree != NULL) {
            if ((strcmp(entree->d_name, ".") != 0) && (strcmp(entree->d_name, "..") != 0)) {
                // creation du chemin
                strcpy(path, pathname);
                if (strcmp(path, "/") != 0) {
                    strcat(path, "/");
                }
                strcat(path, entree->d_name);
                // fin de la creation du chemin
                char str[256] = "";
                strcpy(str, entree->d_name);
                int check = regexec(&r, str, 0, NULL, 0); // comparaison entre le nom du fichier et le modele
                if (check == 0) { // nom du fichier correspend au modele
                    printf("Path: %s\n", path); // affichage du chemin
                }
                struct stat buf;
                stat(path, &buf);                
                if (S_ISDIR(buf.st_mode) != 0) { // recherche dans les sous-repertoires
                    search_from_rep(path, fname, r);
                }
            }
            entree = readdir(rep); // passage a la prochaine entree
        }
        closedir(rep); // fermeture du repertoire
    }
}


void search_from_rep_with_levels(char pathname[PATH_MAX], char fname[256], int niveau, regex_t r) {
    char path[PATH_MAX]; // utilisee pour afficher le chemin complet des fichiers
    if (niveau >= 0) {
        DIR *rep = opendir(pathname); // ouverture du repertoire
        if (rep != NULL) {
            struct dirent *entree = readdir(rep); // lecture d'une entree
            while (entree != NULL) {
                if ((strcmp(entree->d_name, ".") != 0) && (strcmp(entree->d_name, "..") != 0)) { 
                    // creation du chemin
                    strcpy(path, pathname);
                    if (strcmp(path, "/") != 0) {
                        strcat(path, "/");
                    }
                    strcat(path, entree->d_name);
                    // fin de la creation du chemin
                    char str[256] = "";
                    strcpy(str, entree->d_name);
                    int check = regexec(&r, str, 0, NULL, 0); // comparaison entre le nom du fichier et le modele
                    if (check == 0) { // nom du fichier correspend au modele
                        printf("Path: %s\n", path);
                    }
                    struct stat buf;
                    stat(path, &buf);
                    if (S_ISDIR(buf.st_mode) != 0) { // recherche dans les sous-repertoires
                        search_from_rep_with_levels(path, fname, niveau - 1, r);
                    }
                }
                entree = readdir(rep); // passage a la prochaine entree
            }
            closedir(rep); // fermeture du repertoire
        }
    }
}


void search_from_rep_with_options(char pathname[PATH_MAX], char fname[256], _Bool options[6], regex_t r) {
    char path[PATH_MAX]; // utilisee pour afficher le chemin complet des fichiers
    DIR *rep = opendir(pathname); // ouverture du repertoire
    if (rep != NULL) {
        struct dirent *entree = readdir(rep); // lecture d'une entree
        while (entree != NULL) {
            if ((strcmp(entree->d_name, ".") != 0) && (strcmp(entree->d_name, "..") != 0)) {
                // creation du chemin
                strcpy(path, pathname);
                if (strcmp(path, "/") != 0) {
                    strcat(path, "/");
                }
                strcat(path, entree->d_name);
                // fin de la creation du chemin
                struct stat buf;
                stat(path, &buf);
                char str[256] = "";
                strcpy(str, entree->d_name);
                int check = regexec(&r, str, 0, NULL, 0); // comparaison entre le nom du fichier et le modele
                if (check == 0) { // nom du fichier correspend au modele
                    if (options[5]) {
                        for (int i = 0; i < 5; i++) options[i] = 0;
                        char date[36];
                        printf("Created: %s; ", formatdate(date, buf.st_atime));
                        printf("Modified: %s; ", formatdate(date, buf.st_mtime));
                        printf("Size: %d; ", buf.st_size);
                        if (S_ISREG(buf.st_mode) != 0) {
                            printf("Type: -; ");
                        } else if (S_ISFIFO(buf.st_mode) != 0) {
                            printf("Type: p; ");
                        } else if (S_ISCHR(buf.st_mode) != 0) {
                            printf("Type: c; ");
                        } else if (S_ISBLK(buf.st_mode) != 0) {
                            printf("Type: b; ");
                        } else if (S_ISDIR(buf.st_mode) != 0) {
                            printf("Type: d; ");
                        } else if (S_ISLNK(buf.st_mode) != 0) {
                            printf("Type: l; ");
                        } else if (S_ISSOCK(buf.st_mode) != 0) {
                            printf("Type: s; ");
                        }
                        printf("Protection: %d; ", buf.st_mode);
                    }

                    if (options[0]) {
                        char date[36];
                        printf("Created: %s; ", formatdate(date, buf.st_atime));
                    }
                    if (options[1]) {
                        char date[36];
                        printf("Modified: %s; ", formatdate(date, buf.st_mtime));
                    }
                    if (options[2]) {
                        printf("Size: %d; ", buf.st_size);
                    }
                    if (options[3]) {
                        if (S_ISREG(buf.st_mode) != 0) {
                            printf("Type: -; ");
                        } else if (S_ISFIFO(buf.st_mode) != 0) {
                            printf("Type: p; ");
                        } else if (S_ISCHR(buf.st_mode) != 0) {
                            printf("Type: c; ");
                        } else if (S_ISBLK(buf.st_mode) != 0) {
                            printf("Type: b; ");
                        } else if (S_ISDIR(buf.st_mode) != 0) {
                            printf("Type: d; ");
                        } else if (S_ISLNK(buf.st_mode) != 0) {
                            printf("Type: l; ");
                        } else if (S_ISSOCK(buf.st_mode) != 0) {
                            printf("Type: s; ");
                        }
                    }
                    if (options[4]) {
                        printf("Protection: %d; ", buf.st_mode);
                    }
                    printf("Path: %s\n", path);
                }
                if (S_ISDIR(buf.st_mode) != 0) { // recherche dans les sous-repertoires
                    search_from_rep_with_options(path, fname, options, r);
                }
            }
            entree = readdir(rep); // passage a la prochaine entree
        }
        closedir(rep); // fermeture du repertoire
    }
}


void search_from_rep_with_levels_and_options(char pathname[PATH_MAX], char fname[256], int niveau, _Bool options[6], regex_t r) {
    char path[PATH_MAX]; // utilisee pour afficher le chemin complet des fichiers
    if (niveau >= 0) {
        DIR *rep = opendir(pathname); // ouverture du repertoire
        if (rep != NULL) {
            struct dirent *entree = readdir(rep); // lecture d'une entree
            while (entree != NULL) {
                if ((strcmp(entree->d_name, ".") != 0) && (strcmp(entree->d_name, "..") != 0)) {
                    // creation du chemin
                    strcpy(path, pathname);
                    if (strcmp(path, "/") != 0) {
                        strcat(path, "/");
                    }
                    strcat(path, entree->d_name);
                    // fin de la creation du chemin
                    struct stat buf;
                    stat(path, &buf);
                    char str[256] = "";
                    strcpy(str, entree->d_name);
                    int check = regexec(&r, str, 0, NULL, 0); // comparaison entre le nom du fichier et le modele
                    if (check == 0) { // nom du fichier correspend au modele
                        if (options[5]) {
                            for (int i = 0; i < 5; i++) options[i] = 0;
                            char date[36];
                            printf("Created: %s; ", formatdate(date, buf.st_ctime));
                            printf("Modified: %s; ", formatdate(date, buf.st_mtime));
                            printf("Size: %d; ", buf.st_size);
                            if (S_ISREG(buf.st_mode) != 0) {
                                printf("Type: -; ");
                            } else if (S_ISFIFO(buf.st_mode) != 0) {
                                printf("Type: p; ");
                            } else if (S_ISCHR(buf.st_mode) != 0) {
                                printf("Type: c; ");
                            } else if (S_ISBLK(buf.st_mode) != 0) {
                                printf("Type: b; ");
                            } else if (S_ISDIR(buf.st_mode) != 0) {
                                printf("Type: d; ");
                            } else if (S_ISLNK(buf.st_mode) != 0) {
                                printf("Type: l; ");
                            } else if (S_ISSOCK(buf.st_mode) != 0) {
                                printf("Type: s; ");
                            }
                            printf("Protection: %d; ", buf.st_mode);
                        }
                        if (options[0]) {
                            char date[36];
                            printf("Created: %s; ", formatdate(date, buf.st_atime));
                        }
                        if (options[1]) {
                            char date[36];
                            printf("Modified: %s; ", formatdate(date, buf.st_mtime));
                        }
                        if (options[2]) {
                            printf("Size: %d; ", buf.st_size);
                        }
                        if (options[3]) {
                            if (S_ISREG(buf.st_mode) != 0) {
                                printf("Type: -; ");
                            } else if (S_ISFIFO(buf.st_mode) != 0) {
                                printf("Type: p; ");
                            } else if (S_ISCHR(buf.st_mode) != 0) {
                                printf("Type: c; ");
                            } else if (S_ISBLK(buf.st_mode) != 0) {
                                printf("Type: b; ");
                            } else if (S_ISDIR(buf.st_mode) != 0) {
                                printf("Type: d; ");
                            } else if (S_ISLNK(buf.st_mode) != 0) {
                                printf("Type: l; ");
                            } else if (S_ISSOCK(buf.st_mode) != 0) {
                                printf("Type: s; ");
                            }
                        }
                        if (options[4]) {
                            printf("Protection: %d; ", buf.st_mode);
                        }
                        printf("Path: %s\n", path);
                    }
                    if (S_ISDIR(buf.st_mode) != 0) { // recherche dans les sous-repertoires
                        search_from_rep_with_levels_and_options(path, fname, niveau - 1, options, r);
                    }
                }
                entree = readdir(rep); // passage a la prochaine entree
            }
            closedir(rep); // fermeture du repertoire
        }
    }
}

// argc: contient le nombre d'arguments
// argv: contient les arguments
void main(int argc, char * argv[]) {
    char str[256] = "";
    nom_avec_caracteres_generiques(argv[argc - 1], str);
    regex_t regex; // pointeur de l'expression reguliere compilee
    int check = regcomp(&regex, str, 0); // compilation de l'expression reguliere specifiee par le modele 'str'
    if (check == 0) { // compilation avec succes
        if (argc == 1) { // aucun argument
            printf("Error: File name missing.\n");
            printf("Type -h or --help for help.\n");
        } else if (argc == 2) { // -help ou fichier
            char cwd[PATH_MAX];
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                if ((strcmp(argv[1], "-h") != 0) && (strcmp(argv[1], "--help") != 0)) {
                    search_from_rep(cwd, argv[1], regex);
                } else {
                    printf("Usage: search [DIRECTORY] [OPTION] FILE\n");
                    printf("Search files in the systeme\n");
                    printf("Options: \n");
                    printf("-d: show creation date\n");
                    printf("-m: show latest modification date\n");
                    printf("-s: show file size\n");
                    printf("-t: show file type\n");
                    printf("-p: show protection\n");
                    printf("-a: show all caracteristics\n");
                    printf("Made by: TAIBI Mohamed Kamel Eddine\n");
                }
            }
        } else if (argc == 3) { // deux arguments: (option + fichier) ou (rep + fichier)
            if (argv[1][0] == '-') { // premier cas: option + fichier
                _Bool options[6] = {0}; // tableau des options
                char option[5];
                strncpy(option, argv[1] + 1, strlen(argv[1]) - 1);
                char cwd[PATH_MAX];
                if (getcwd(cwd, sizeof(cwd)) != NULL) { // repertoire courant
                    int n = -1;
                    switch (option[0]) {
                    case 'd':
                        options[0] = 1;
                        break;
                    case 'm':
                        options[1] = 1;
                        break;
                    case 's':
                        options[2] = 1;
                        break;
                    case 't':
                        options[3] = 1;
                        break;
                    case 'p':
                        options[4] = 1;
                        break;
                    case 'a':
                        options[5] = 1;
                        break;
                    default:
                        n = atoi(option); // niveau
                    }
                    if (n == -1) {
                        search_from_rep_with_options(cwd, argv[2], options, regex);
                    } else {
                        search_from_rep_with_levels(cwd, argv[2], n, regex);
                    }
                }
            } else { // deuxieme cas: rep + fichier
                search_from_rep(argv[1], argv[2], regex);
            }
        } else if (argc > 3) { // trois arguments: rep + options + fichier
            _Bool options[6] = {0};
            char option[5];
            int j = 0;
            int k;
            if (argv[1][0] == '-') { // pas de repertoire specifie
                k = 1;
            } else { // le premier argument est le nom du repertoire
                k = 2;
            }
            for(int i = k; i <= argc - 2; i++) { // extraction des options
                strncpy(option, argv[i] + 1, strlen(argv[i]) - 1);
                switch (option[0]) {
                    case 'd':
                        options[0] = 1;
                        break;
                    case 'm':
                        options[1] = 1;
                        break;
                    case 's':
                        options[2] = 1;
                        break;
                    case 't':
                        options[3] = 1;
                        break;
                    case 'p':
                        options[4] = 1;
                        break;
                    case 'a':
                        options[5] = 1;
                        break;
                    default:
                        j = i; // option des niveaux de recherche
                        break;
                }
            }
            strncpy(option, argv[j] + 1, strlen(argv[j]) - 1);
            int n;
            if (j == 0) {
                n = 9999;
            } else {
                n = atoi(option); // niveau
            }
            
            if (argv[1][0] == '-') { // pas de repertoire specifie
                search_from_rep_with_levels_and_options(".", argv[argc - 1], n, options, regex);
            } else { // le premier argument est le nom du repertoire
                search_from_rep_with_levels_and_options(argv[1], argv[argc - 1], n, options, regex);
            }
        }
    }
}