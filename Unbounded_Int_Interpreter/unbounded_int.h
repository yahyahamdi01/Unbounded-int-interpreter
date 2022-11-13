#ifndef _UNBOUNDED_INT_H_
#define _UNBOUNDED_INT_H_

#include <stdlib.h>

typedef struct chiffre{
  struct chiffre *suivant;
  char c;
  struct chiffre *precedent;
} chiffre;

typedef struct unbounded_int {
  char signe; /* '+' or '-' */
  size_t len; /* length of the list */
  chiffre *premier; /* pointer to first element in the list */
  chiffre *dernier; /* pointer to last element in the list */
} unbounded_int;


unbounded_int string2unbounded_int(const char *e);
unbounded_int ll2unbounded_int(long long i);
char *unbounded_int2string(unbounded_int i);
int unbounded_int_cmp_unbounded_int(unbounded_int a, unbounded_int b);
int unbounded_int_cmp_ll(unbounded_int a, long long b);
unbounded_int unbounded_int_somme(unbounded_int a, unbounded_int b);
unbounded_int unbounded_int_difference(unbounded_int a, unbounded_int b);
unbounded_int unbounded_int_produit(unbounded_int a, unbounded_int b);

#endif /* _UNBOUNDED_INT_H_ */
