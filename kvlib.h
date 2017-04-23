/**
 * Header de la librairie : contient uniquement l'interface presentee
 * aux processus utilisateur
 */

#ifndef KVLIB_H
#define KVLIB_H

/* ecriture d'un couple cle valeur */
int kvlib_set(const char *key, const char *value);

/* lecture d'une valeur a partir d'une cle */
int kvlib_get(const char *key, char *value);

/* fomatage */
int kvlib_format();

int kvlib_del(const char *key);

#endif /* KVLIB_H */
