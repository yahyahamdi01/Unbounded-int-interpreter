#include "unbounded_int.h"
#include <stdio.h>
#include <string.h>

static void clear_unbounded_int(unbounded_int i){
  if (i.premier == NULL) return;
  chiffre *it1, *it2;
  it1 = i.premier;
  for(int k=0; k < i.len; ++k){
    it2 = it1->suivant;
    free(it1);
    it1 = it2;
  }
}

/* If malloc fails, fill up signe with '*' */

/* When e is not valid, the "signe" element of the result is == '*' */
/* String to unbounded int */
unbounded_int string2unbounded_int(const char *e)
{
  unbounded_int invalid;
  invalid.premier = NULL;
  invalid.dernier = NULL;
  invalid.len = 0;
  invalid.signe = '*';

  unbounded_int res = invalid;
  
  if ( e == NULL) return invalid;
  size_t len = strlen(e);
  if (strlen(e) == 0 ) return invalid;

  const char *it = e;
  size_t i = 0;
  res.signe = '+'; /* + is not mandatory in the string */
  if (*it == '-' || *it == '+'){
    res.signe = *it;
    ++it;
    ++i;
  }
  res.len = len - i;

  chiffre* slider = NULL;
  chiffre* current = NULL;
  for (; i < len; ++i, ++it){
    if (*it >= '0' && *it <= '9') {
      current = (chiffre *) malloc (sizeof (struct chiffre));
      if ( current == NULL){
	return invalid;
      }

      current->c = *it;
      current->precedent = NULL;
      current->suivant = NULL;
      if (res.premier == NULL) {
	res.premier = current;
      } else {
	slider->suivant = current;
	current->precedent = slider;
      }
      slider = current;
    } else {
      /* free memory */
      if (current != NULL){
	while (slider != NULL){
	  slider = current->precedent;
	  free(current);
	  current = slider;
	}
      }
      return invalid;
    }
  }
  res.dernier = slider;
  return res;
}

/* long long to unbounded int */
unbounded_int ll2unbounded_int(long long i)
{
  /* The below algo works by division */
  /* Alternatively we could use sprintf(str, "%lli", i) then return 
     string2unbounded_int(str) by increasing str size when return is negative */
  unbounded_int invalid;
  invalid.premier = NULL;
  invalid.dernier = NULL;
  invalid.len = 0;
  invalid.signe = '*';

  unbounded_int res = invalid;
  res.signe = i >= 0 ? '+' : '-';
  res.len = 0;
  int sign = i >= 0 ? 1 :-1;
  
  /* handling the 0 case */
  if ( i == 0){
    res.len = 1;
    res.premier = (chiffre *)malloc(sizeof(struct chiffre));
    res.premier->suivant = NULL;
    res.premier->precedent = NULL;
    res.premier->c = '0';
    res.dernier = res.premier;
    return res;
  }

  long long j = i;
  chiffre* slider = NULL;
  chiffre* current = NULL;
  int mod = 0;
  while(j != 0)
  {
    current = (chiffre *) malloc(sizeof(struct chiffre));
    if (current == NULL) return invalid;

    current->precedent = NULL;
    current->suivant = NULL;
    if (res.dernier == NULL){
      res.dernier = current;
      /* first iteration we don't switch the sign because |LLONG_MIN|>|LLONG_MAX| */
      mod = (j % (10 * sign))*sign;
      j  /= 10 * sign;
    }else{
      current->suivant = slider;
      slider->precedent = current;
      mod = j % 10;
      j  /= 10;
    }
    current->c = mod + '0';
    slider = current;
    res.len += 1;
  }
  res.premier = slider;
  return res;
}

/* unbounded int to string */
char *unbounded_int2string(unbounded_int i)
{
  char *res = NULL;
  if (i.signe == '*' || i.len == 0) return res;
  size_t res_len = i.len;
  if (i.signe == '-' )
    if (i.len!=1 || i.premier->c != '0') /* avoiding -0 */
      ++res_len;

  res = (char *)malloc ((res_len + 1)*sizeof(char));
  
  size_t counter = 0;
  if (i.signe == '-')
    if (i.len!=1 || i.premier->c != '0')
      res[counter++] = '-';

  chiffre *it = i.premier;
  for(; counter < res_len; ++counter){
    res[counter] = it->c;
    it = it->suivant;
  }
  res[res_len] = '\0';

  return res;
}

/* compare a to b, returns -1,0,1 if a<b, a==b, a>b accordingly */
int unbounded_int_cmp_unbounded_int(unbounded_int a, unbounded_int b)
{
  if (a.signe != b.signe && a.len == b.len && a.len == 1)
    if (a.premier->c == '0' && b.premier->c == '0')
      return 0; /* -0 == 0 == +0 */
    
  if (a.signe == '-' && b.signe == '+')
    return -1;
  if (a.signe == '+' && b.signe == '-')
    return 1;
  
  if (a.signe == '*' || b.signe == '*' || a.signe != b.signe)
    printf(" !!! %i : ERROR in inputs cmp", __LINE__);

  unbounded_int left  = a; /* shallow copy */
  unbounded_int right = b; /* shallow copy */
  if (a.signe == '-'){ /* then we return (|b| cmp |a|) */
    left = b;
    left.signe = '+';
    right = a;
    right.signe = '-';
  }
  /* we return (left > right) */
  if (left.len > right.len)
    return 1;
  if (right.len > left.len)
    return -1;
  
  chiffre* left_it = left.premier;
  chiffre* right_it = right.premier;
  int l,r;
  for(int ctr = 0; ctr < left.len; ++ctr){
    l = left_it->c - '0';
    r = right_it->c - '0';
    if ( l > r) return 1;
    if ( l < r) return -1;
    left_it = left_it->suivant;
    right_it = right_it->suivant;
  }

  return 0;
}

/* compare a to ll b */
int unbounded_int_cmp_ll(unbounded_int a, long long b)
{
  unbounded_int ui_b = ll2unbounded_int(b);
  int res = unbounded_int_cmp_unbounded_int(a, ui_b);
  clear_unbounded_int(ui_b);
  return res;
}

/* aux : returns a + b where a and b are non negative */
static unbounded_int unbounded_int_pos_somme(unbounded_int a, unbounded_int b)
{
  unbounded_int res;
  res.signe = '+';
  res.len = 0;
  res.premier = NULL;
  res.dernier = NULL;
  chiffre* res_slider = NULL;
  
  chiffre* ita = a.dernier;
  chiffre* itb = b.dernier;
  int add_one = 0; /* 0 or 1 */
  int sum = 0;
  while(ita != NULL  && itb != NULL)
  {
    sum = (ita->c - '0') + (itb->c - '0') + add_one;
    chiffre* current = (chiffre*) malloc (sizeof(struct chiffre));
    current->suivant = NULL;
    current->precedent = NULL;
    current->c = (sum % 10) + '0';
    add_one = sum / 10;

    if (res.dernier == NULL)
    {
      res.dernier = current;
    }else{
      current->suivant = res_slider;
      res_slider->precedent = current;
    }
    res_slider = current;
    res.len += 1;
    ita = ita->precedent;
    itb = itb->precedent;
  }

  chiffre* itl = NULL;
  if (ita != NULL && itb == NULL)
    itl = ita;
  if (ita == NULL && itb != NULL)
    itl = itb;
  
  while(itl != NULL)
  {
    chiffre* current = (chiffre*) malloc(sizeof(struct chiffre));

    sum = (itl->c - '0') + add_one;
    current->c = (sum % 10) + '0';
    add_one = sum / 10;

    current->precedent = NULL;
    current->suivant = res_slider;
    res_slider->precedent = current;

    res_slider = current;
    res.len += 1;

    itl = itl->precedent;
  }

  if (add_one == 1){
    chiffre* current = (chiffre*) malloc(sizeof(struct chiffre));
    current->c = '1';
    current->precedent= NULL;
    current->suivant = res_slider;
    res_slider->precedent = current;
    res_slider = current;
    res.len += 1;
  }

  res.premier = res_slider;

  return res;
}

/* aux : returns a - b where a >= b >= 0 */
static unbounded_int unbounded_int_pos_ordered_difference(unbounded_int a, unbounded_int b)
{
  unbounded_int res;
  res.signe = '+';
  res.len = 0;
  res.premier = NULL;
  res.dernier = NULL;
  chiffre* res_slider = NULL;

  chiffre* ita = a.dernier;
  chiffre* itb = b.dernier;
  int minus_one = 0; /* 0 or -1 */
  int sub = 0;

  while(ita != NULL && itb != NULL)
  {
    sub = (ita->c - '0') - (itb->c - '0') + minus_one;
    chiffre* current = (chiffre*) malloc (sizeof(struct chiffre));
    current->suivant = NULL;
    current->precedent = NULL;
    if (sub >= 0){
      current->c = sub + '0';
      minus_one = 0;
    }else{
      current->c = (sub + 10) + '0';
      minus_one = -1;
    }

    if (res.dernier == NULL)
    {
      res.dernier = current;
    }else{
      current->suivant = res_slider;
      res_slider->precedent = current;
    }
    res_slider = current;
    res.len += 1;
    ita = ita->precedent;
    itb = itb->precedent;
  }

  while (ita != NULL) /* we check only ita since a>=b */
  {
    chiffre* current = (chiffre*) malloc(sizeof(struct chiffre));

    sub = (ita->c - '0') + minus_one;
    if (sub >= 0){
      current->c = sub + '0';
      minus_one = 0;
    }else{
      current->c = (sub + 10) + '0';
      minus_one = -1;
    }
    current->precedent = NULL;
    current->suivant = res_slider;
    res_slider->precedent = current;
    res_slider = current;
    res.len += 1;

    ita = ita->precedent;
  }
  
  if (minus_one == -1)
  {
    printf("-- ERROR : should not happen, a should be >= b");
  }

  res.premier = res_slider;

  /* cleaning up zeros on the left */
  chiffre* to_delete = NULL;
  while(res.premier->c == '0' && res.len > 1)
  {
    to_delete = res.premier;
    res.premier = res.premier->suivant;
    res.premier->precedent = NULL;
    res.len -= 1;
    free(to_delete);
  }

  return res;
}


/* returns a + b */
unbounded_int unbounded_int_somme(unbounded_int a, unbounded_int b)
{
  unbounded_int res;
  res.signe = '*';
  res.premier = NULL;
  res.dernier = NULL;
  res.len = 0;
  
  if (a.signe == '+' && b.signe == '+')
  {
    return unbounded_int_pos_somme(a, b);
  }
  else if (a.signe == '-' && b.signe == '-')
  {
    unbounded_int minus_a = a;
    minus_a.signe = '+';
    unbounded_int minus_b = b;
    minus_b.signe = '+';
    res = unbounded_int_pos_somme(minus_a, minus_b);
    res.signe = '-';
  }
  else if (a.signe == '+' && b.signe == '-')
  {
    unbounded_int minus_b = b;
    minus_b.signe = '+';
    if (1 == unbounded_int_cmp_unbounded_int(a, minus_b)){
      return unbounded_int_pos_ordered_difference(a, minus_b);
    } else{
      res = unbounded_int_pos_ordered_difference(minus_b, a);
      res.signe = '-';
    }
  }
  else if (a.signe == '-' && b.signe == '+')
  {
    unbounded_int minus_a = a;
    minus_a.signe = '+';
    if (1 == unbounded_int_cmp_unbounded_int(b, minus_a)) {
      return unbounded_int_pos_ordered_difference(b, minus_a);
    }else{
      res = unbounded_int_pos_ordered_difference(minus_a, b);
      res.signe = '-';
    }
  }
  return res;
}

/* returns a-b */
unbounded_int unbounded_int_difference(unbounded_int a, unbounded_int b)
{
  unbounded_int res;
  res.signe = '*';
  res.premier = NULL;
  res.dernier = NULL;
  res.len = 0;
  
  if (a.signe == '+' && b.signe == '+')
  {
    if (1 == unbounded_int_cmp_unbounded_int(a, b)) {
      return unbounded_int_pos_ordered_difference(a, b);
    } else {
      res = unbounded_int_pos_ordered_difference(b, a);
      res.signe = '-';
    }
  }
  else if (a.signe == '-' && b.signe == '-')
  {
    unbounded_int minus_a = a;
    minus_a.signe = '+';
    unbounded_int minus_b = b;
    minus_b.signe = '+';
    if (1 == unbounded_int_cmp_unbounded_int(minus_b, minus_a)){
      return unbounded_int_pos_ordered_difference(minus_b, minus_a);
    }else{
      res = unbounded_int_pos_ordered_difference(minus_a, minus_b);
      res.signe = '-';
    }
  }
  else if (a.signe == '+' && b.signe == '-')
  {
    unbounded_int minus_b = b;
    minus_b.signe = '+';
    return unbounded_int_pos_somme(a, minus_b);
  }
  else if (a.signe == '-' && b.signe == '+')
  {
    unbounded_int minus_a = a;
    minus_a.signe = '+';
    res = unbounded_int_pos_somme(minus_a, b);
    res.signe = '-';
  }

  return res;
}

/* returns a*b */
unbounded_int unbounded_int_produit(unbounded_int a, unbounded_int b)
{
  unbounded_int res;
  res.premier = NULL;
  res.dernier = NULL;
  int n = a.len;
  int m = b.len;
  res.len = m+n;
  res.signe = (a.signe == b.signe) ? '+' : '-';

  /* allocate result */
  chiffre* slider = NULL;
  for(int k=0; k < m+n; ++k){
    chiffre* current = (chiffre*) malloc(sizeof(struct chiffre));
    current->c = '0';
    current->suivant = NULL;
    current->precedent = NULL;
    if (NULL == res.premier){
      res.premier = current;
    } else {
      slider->suivant = current;
      current->precedent = slider;
    }
    slider = current;
  }
  res.dernier = slider;

  int i,j,r;
  chiffre *itb = b.dernier;
  chiffre *itres_at_j = res.dernier;

  chiffre *ita = NULL;
  chiffre *itres_at_ij = NULL;

  chiffre *itres_at_jn = res.dernier;
  for (i=0; i<n;++i)
    itres_at_jn = itres_at_jn->precedent;

  for (j=0;  j < m; ++j)
  {
    r = 0;
    if (itb->c == '0'){
      itb = itb->precedent;
      itres_at_j = itres_at_j->precedent;
      itres_at_jn = itres_at_jn->precedent;
      continue;
    }
    i = 0;
    ita = a.dernier;
    itres_at_ij = itres_at_j;
    for(; i < n; ++i)
    {
      int v = (itres_at_ij->c - '0') + (ita->c - '0')*(itb->c - '0') + r;
      itres_at_ij->c = (v%10) + '0';
      r = v / 10;

      ita = ita->precedent;
      itres_at_ij = itres_at_ij->precedent;
    } 
    itres_at_jn->c = r + '0';

    itb = itb->precedent;
    itres_at_j = itres_at_j->precedent;
    itres_at_jn = itres_at_jn->precedent;
  } 

  /* cleaning up zeros on the left */
  chiffre* to_delete = NULL;
  while(res.premier->c == '0' && res.len > 1)
  {
    to_delete = res.premier;
    res.premier = res.premier->suivant;
    res.premier->precedent = NULL;
    res.len -= 1;
    free(to_delete);
  }

  return res;
}
