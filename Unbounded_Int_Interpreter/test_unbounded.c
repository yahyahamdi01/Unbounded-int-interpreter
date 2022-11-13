#include "unbounded_int.h"
#include <stdio.h>
#include <string.h>
#include <limits.h>
/*
static void view(const unbounded_int input)
{
  printf("#################\n");
  printf("signe : %c, len : %zu, premier : %p, dernier : %p\n",input.signe, input.len, (void *)input.premier, (void *)input.dernier);
  if (input.premier == NULL && input.len >0){
    printf("  -- ERROR -- premier pointer is NULL --\n");
    return;
  }
  chiffre *it = input.premier;
  for (int i = 0; i<input.len; ++i)
  {
    printf("------\n");
    if (it->suivant != NULL)
      printf("%p\n", (void *)it->suivant);
    else
      printf("NULL\n");
    printf("%c at %p\n", it->c, (void *)it);
    if (it->precedent != NULL)
      printf("%p\n", (void *)it->precedent);
    else
      printf("NULL\n");
    it = it->suivant;
  }
  printf("#################\n");
}*/

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


int main()
{
  /*/// Tests ////*/

  /* 1 - string -> unbounded_int */
  printf("\n          >>>>>>>  TEST 1 : string -> unbounded_int  <<<<<<<\n\n");
  unbounded_int ex0 = string2unbounded_int("0");
  /* view(ex0); */
  unbounded_int ex1 = string2unbounded_int("23432432423");
  /* view(ex1); */
  unbounded_int ex2 = string2unbounded_int("-4543676543298");
  /* view(ex2); */
  unbounded_int ex3 = string2unbounded_int("NOT VALID");
  /* view(ex3); */
  unbounded_int ex4 = string2unbounded_int("123BAD321");
  /* view(ex4); */

  /* 2 - ll -> unbounded_int */
  printf("\n          >>>>>>>  TEST 2 : ll -> unbounded_int  <<<<<<<\n\n");
  long long int ll0 = 0;
  unbounded_int ex_ll0 = ll2unbounded_int(ll0);
  char * str_ll0 = unbounded_int2string(ex_ll0);
  printf("tostring(fromll2(%lli)) -> %s\n", ll0, str_ll0);

  long long int ll1 = 123;
  unbounded_int ex_ll1 = ll2unbounded_int(ll1);
  char * str_ll1 = unbounded_int2string(ex_ll1);
  printf("tostring(fromll2(%lli)) -> %s\n", ll1, str_ll1);

  long long int ll2 = LLONG_MIN;
  unbounded_int ex_ll2 = ll2unbounded_int(ll2);
  char * str_ll2 = unbounded_int2string(ex_ll2);
  printf("tostring(fromll2(%lli)) -> %s\n", ll2, str_ll2);

  long long int ll3 = LLONG_MAX;
  unbounded_int ex_ll3 = ll2unbounded_int(ll3);
  char * str_ll3 = unbounded_int2string(ex_ll3);
  printf("tostring(fromll2(%lli)) -> %s\n", ll3, str_ll3);
  

  /* 3 - unbounded_int -> string */
  printf("\n          >>>>>>>  TEST 3 : unbounded_int -> string  <<<<<<<\n\n");

  char * str0 = unbounded_int2string(ex0);
  printf("tostring of fromstring of \"0\" is : \"%s\"\n", str0);

  char * str1 = unbounded_int2string(ex1);
  printf("tostring of fromstring of \"23432432423\" is : \"%s\"\n", str1);

  char * str2 = unbounded_int2string(ex2);
  printf("tostring of fromstring of \"-4543676543298\" is : \"%s\"\n", str2);

  char * str3 = unbounded_int2string(ex3);
  printf("tostring of fromstring of \"NOT VALID\" is : \"%s\"\n", str3);

  char * str4 = unbounded_int2string(ex4);
  printf("tostring of fromstring of \"123BAD321\" is : \"%s\"\n", str4);

  /* 4 - unbounded_int CMP unbounded_int */
  printf("\n          >>>>>>>  TEST 4 : unbounded_int CMP unbounded_int  <<<<<<<\n\n");
  
  printf("%s cmp %s => %i\n", str0, str1, unbounded_int_cmp_unbounded_int(ex0, ex1));
  printf("%s cmp %s => %i\n", str0, str2, unbounded_int_cmp_unbounded_int(ex0, ex2));
  printf("%s cmp %s => %i\n", str1, str2, unbounded_int_cmp_unbounded_int(ex1, ex2));
  printf("%s cmp %s => %i\n", str1, str1, unbounded_int_cmp_unbounded_int(ex1, ex1));
  printf("%s cmp %s => %i\n", str2, str2, unbounded_int_cmp_unbounded_int(ex2, ex2));
  printf("%s cmp %s => %i\n", str0, str0, unbounded_int_cmp_unbounded_int(ex0, ex0));
  
  unbounded_int ex5 = string2unbounded_int("23432432424");
  char * str5 = unbounded_int2string(ex5);
  printf("%s cmp %s => %i\n", str1, str5, unbounded_int_cmp_unbounded_int(ex1, ex5));
  printf("%s cmp %s => %i\n", str5, str1, unbounded_int_cmp_unbounded_int(ex5, ex1));

  unbounded_int ex6 = string2unbounded_int("-4543676543297");
  char * str6 = unbounded_int2string(ex6);
  printf("%s cmp %s => %i\n", str2, str6, unbounded_int_cmp_unbounded_int(ex2, ex6));
  printf("%s cmp %s => %i\n", str6, str2, unbounded_int_cmp_unbounded_int(ex6, ex2));

  unbounded_int ex7 = string2unbounded_int("4");
  unbounded_int ex8 = string2unbounded_int("4");
  printf("4 cmp 4 => %i\n", unbounded_int_cmp_unbounded_int(ex7, ex8));

  unbounded_int ex9 = string2unbounded_int("-5");
  unbounded_int ex10 = string2unbounded_int("-5");
  printf("-5 cmp -5 => %i\n", unbounded_int_cmp_unbounded_int(ex9, ex10));

  unbounded_int ex11 = string2unbounded_int("-0");
  printf("-0 cmp +0 => %i\n", unbounded_int_cmp_unbounded_int(ex11, ex0));

  /* 5 - unbounded_int CMP ll */
  printf("\n          >>>>>>>  TEST 5 : unbounded_int CMP ll  <<<<<<<\n\n");
  
  printf("%s cmp %lli => %i\n", str0, 23432432423LL,
	 unbounded_int_cmp_ll(ex0, 23432432423LL));
  printf("%s cmp %lli => %i\n", str0, -4543676543298LL,
	 unbounded_int_cmp_ll(ex0, -4543676543298LL));
  printf("%s cmp %lli => %i\n", str1, -4543676543298LL,
	 unbounded_int_cmp_ll(ex1, -4543676543298LL));
  printf("%s cmp %lli => %i\n", str1, 23432432423LL,
	 unbounded_int_cmp_ll(ex1,23432432423LL));
  printf("%s cmp %lli => %i\n", str2, -4543676543298LL,
	 unbounded_int_cmp_ll(ex2, -4543676543298LL));
  printf("%s cmp %lli => %i\n", str0, 0LL,
	 unbounded_int_cmp_ll(ex0, 0LL));
  printf("%s cmp %lli => %i\n", str1, 23432432424LL,
	 unbounded_int_cmp_ll(ex1, 23432432424LL));
  printf("%s cmp %lli => %i\n", str5, 23432432423LL,
	 unbounded_int_cmp_ll(ex5, 23432432423LL));
  printf("%s cmp %lli => %i\n", str2, -4543676543297LL,
	 unbounded_int_cmp_ll(ex2, -4543676543297LL));
  printf("%s cmp %lli => %i\n", str6, -4543676543298LL,
	 unbounded_int_cmp_ll(ex6, -4543676543298LL));
  printf("4 cmp 4 => %i\n", unbounded_int_cmp_ll(ex7, 4LL));
  printf("-5 cmp -5 => %i\n", unbounded_int_cmp_ll(ex9, -5LL));
  printf("-0 cmp +0 => %i\n", unbounded_int_cmp_ll(ex11, 0LL));
  

  /* 6 - unbounded_int + unbounded_int */
  printf("\n          >>>>>>>  TEST 6 : unbounded_int + unbounded_int  <<<<<<<\n\n");
  unbounded_int ex12 = unbounded_int_somme(ex1, ex0);
  char *str12 = unbounded_int2string(ex12);
  printf("%s + 0 => %s\n", str1, str12);

  unbounded_int ex13 = unbounded_int_somme(ex1, ex1);
  char *str13 = unbounded_int2string(ex13);
  printf("%s + %s => %s\n", str1, str1, str13);

  unbounded_int ex14 = unbounded_int_somme(ex0, ex1);
  char *str14 = unbounded_int2string(ex14);
  printf("0 + %s => %s\n", str1, str14);

  unbounded_int ex15 = unbounded_int_somme(ex0, ex0);
  char *str15 = unbounded_int2string(ex15);
  printf("0 + 0 => %s\n", str15);

  unbounded_int ex16 = unbounded_int_somme(ex_ll3, ex_ll3);
  char *str16 = unbounded_int2string(ex16);
  printf("%lli + %lli => %s\n", LLONG_MAX, LLONG_MAX, str16);

  unbounded_int ex31 = ll2unbounded_int(-8973498534);
  char *str31 = unbounded_int2string(ex31);
  unbounded_int ex32 = unbounded_int_somme(ex6, ex31);
  char *str32 = unbounded_int2string(ex32);
  printf("%s + %s => %s\n", str31, str6, str32);

  unbounded_int ex33 = unbounded_int_somme(ex5, ex6);
  char *str33 = unbounded_int2string(ex33);
  printf("%s + %s => %s\n", str5, str6, str33);

  unbounded_int ex34 = unbounded_int_somme(ex6, ex5);
  char *str34 = unbounded_int2string(ex34);
  printf("%s + %s => %s\n", str6, str5, str34);

  unbounded_int ex35 = unbounded_int_somme(ex31, ex5);
  char *str35 = unbounded_int2string(ex35);
  printf("%s + %s => %s\n", str31, str5, str35);

  unbounded_int ex36 = unbounded_int_somme(ex5, ex31);
  char *str36 = unbounded_int2string(ex36);
  printf("%s + %s => %s\n", str5, str31, str36);

  /* 7 - unbounded_int - unbounded_int */
  printf("\n          >>>>>>>  TEST 7 : unbounded_int - unbounded_int  <<<<<<<\n\n");

  unbounded_int ex17 = unbounded_int_difference(ex5, ex1);
  char *str17 = unbounded_int2string(ex17);
  printf("%s - %s => %s\n", str5, str1, str17);

  unbounded_int ex18 = unbounded_int_difference(ex13, ex1);
  char *str18 = unbounded_int2string(ex18);
  printf("%s - %s => %s\n", str13, str1, str18);

  unbounded_int ex19 = unbounded_int_difference(ex1, ex1);
  char *str19 = unbounded_int2string(ex19);
  printf("%s - %s => %s\n", str1, str1, str19);

  unbounded_int ex20 = unbounded_int_difference(ex1, ex0);
  char *str20 = unbounded_int2string(ex20);
  printf("%s - 0 => %s\n", str1, str20);

  unbounded_int ex21 = unbounded_int_difference(ex16, ex_ll3);
  char *str21 = unbounded_int2string(ex21);
  printf("18446744073709551614 - %lli => %s\n", LLONG_MAX, str21);

  unbounded_int ex41 = unbounded_int_difference(ex_ll3, ex16);
  char *str41 = unbounded_int2string(ex41);
  printf("%lli - 18446744073709551614 => %s\n", LLONG_MAX, str41);

  unbounded_int ex42 = unbounded_int_difference(ex31, ex6);
  char *str42 = unbounded_int2string(ex42);
  printf("%s - %s => %s\n", str31, str6, str42);  

  unbounded_int ex43 = unbounded_int_difference(ex6, ex31);
  char *str43 = unbounded_int2string(ex43);
  printf("%s - %s => %s\n", str6, str31, str43);

  unbounded_int ex44 = unbounded_int_difference(ex1, ex31);
  char *str44 = unbounded_int2string(ex44);
  printf("%s - %s => %s\n", str1, str31, str44);

  unbounded_int ex45 = unbounded_int_difference(ex31, ex1);
  char *str45 = unbounded_int2string(ex45);
  printf("%s - %s => %s\n", str31, str1, str45);

  /* 8 - unbounded_int * unbounded_int */
  printf("\n          >>>>>>>  TEST 8 : unbounded_int * unbounded_int  <<<<<<<\n\n");

  unbounded_int ex51 = unbounded_int_produit(ex7, ex9);
  char *str51 = unbounded_int2string(ex51);
  printf("4 * -5 => %s\n", str51);

  unbounded_int ex52 = unbounded_int_produit(ex1, ex9);
  char *str52 = unbounded_int2string(ex52);
  printf("%s * -5 => %s\n", str1, str52);

  unbounded_int ex53 = unbounded_int_produit(ex7, ex_ll2);
  char *str53 = unbounded_int2string(ex53);
  printf("4 * %s => %s\n", str_ll2, str53);

  unbounded_int ex54 = unbounded_int_produit(ex_ll2, ex7);
  char *str54 = unbounded_int2string(ex54);
  printf("%s * 4 => %s\n", str_ll2, str54);

  unbounded_int ex55 = unbounded_int_produit(ex_ll3, ex_ll0);
  char *str55 = unbounded_int2string(ex55);
  printf("%s * 0 => %s\n", str_ll3, str55);

  unbounded_int ex56 = unbounded_int_produit(ex_ll0, ex_ll3);
  char *str56 = unbounded_int2string(ex56);
  printf("0 * %s => %s\n", str_ll3, str56);

  /* Deallocating memory */
  /* Test1 */
  clear_unbounded_int(ex0);
  clear_unbounded_int(ex1);
  clear_unbounded_int(ex2);
  clear_unbounded_int(ex3);
  clear_unbounded_int(ex4);
  /* Test2 */
  clear_unbounded_int(ex_ll0);
  free(str_ll0);
  clear_unbounded_int(ex_ll1);
  free(str_ll1);
  clear_unbounded_int(ex_ll2);
  free(str_ll2);
  clear_unbounded_int(ex_ll3);
  free(str_ll3);
  /* Test3 */
  free(str0);
  free(str1);
  free(str2);
  free(str3);
  free(str4);
  /* Test4 */
  clear_unbounded_int(ex5);
  clear_unbounded_int(ex6);
  clear_unbounded_int(ex7);
  clear_unbounded_int(ex8);
  clear_unbounded_int(ex9);
  clear_unbounded_int(ex10);
  clear_unbounded_int(ex11);
  free(str5);
  free(str6);
  /* Test5 */  
  /* Test6 */
  clear_unbounded_int(ex12);
  clear_unbounded_int(ex13);
  clear_unbounded_int(ex14);
  clear_unbounded_int(ex15);
  clear_unbounded_int(ex16);
  free(str12);
  free(str13);
  free(str14);
  free(str15);
  free(str16);
  clear_unbounded_int(ex31);
  clear_unbounded_int(ex32);
  clear_unbounded_int(ex33);
  clear_unbounded_int(ex34);
  clear_unbounded_int(ex35);
  clear_unbounded_int(ex36);
  free(str31);
  free(str32);
  free(str33);
  free(str34);
  free(str35);
  free(str36);
  /* Test7 */
  clear_unbounded_int(ex17);
  clear_unbounded_int(ex18);
  clear_unbounded_int(ex19);
  clear_unbounded_int(ex20);
  clear_unbounded_int(ex21);
  free(str17);
  free(str18);
  free(str19);
  free(str20);
  free(str21);
  clear_unbounded_int(ex41);
  clear_unbounded_int(ex42);
  clear_unbounded_int(ex43);
  clear_unbounded_int(ex44);
  clear_unbounded_int(ex45);
  free(str41);
  free(str42);
  free(str43);
  free(str44);
  free(str45);
  /* Test8 */
  clear_unbounded_int(ex51);
  clear_unbounded_int(ex52);
  clear_unbounded_int(ex53);
  clear_unbounded_int(ex54);
  clear_unbounded_int(ex55);
  clear_unbounded_int(ex56);
  free(str51);
  free(str52);
  free(str53);
  free(str54);
  free(str55);
  free(str56);
  return 0;
}
