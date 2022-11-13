#include "unbounded_int.h"
#include <stdio.h>
#include <string.h>

#define CONTAINER_CHUNK 128
#define LINE_CHUNK 1024

static void clear_unbounded_int(unbounded_int *i){
  if (i == NULL || i->premier == NULL) return;
  chiffre *it1, *it2;
  it1 = i->premier;
  for(int k=0; k < i->len; ++k){
    it2 = it1->suivant;
    free(it1);
    it1 = it2;
  }
}

/***************    Container    *****************/

/* Container to store variables names and their values
   Has ownership of the memory */
typedef struct container
{
  int capacity;
  int length;
  char ** names;
  unbounded_int ** values;
} container;

static void init_container(container *variables)
{
  variables->capacity = CONTAINER_CHUNK;
  variables->length = 0;
  variables->names = (char **) calloc(variables->capacity, sizeof(char *));
  variables->values = (unbounded_int **) calloc(variables->capacity, sizeof(unbounded_int *));
}

static void resize_container(container *variables)
{
  variables->capacity += CONTAINER_CHUNK;
  char **new_names = (char **)calloc(variables->capacity, sizeof(char *));
  unbounded_int **new_values = (unbounded_int **) calloc(variables->capacity, sizeof(unbounded_int *));
  for(int i = 0; i < variables->length; ++i){
    new_names[i] = variables->names[i];
    new_values[i] = variables->values[i];
  }
  variables->names = new_names;
  variables->values = new_values;
  free(new_names);
  free(new_values);
}

/* inserts or overwrites existing value
   passes ownership of value to container */
static void insert(container *variables, char* name, unbounded_int *value)
{
  int index = -1;
  for (int i=0; i < variables->length; ++i)
  {
    if (!strcmp(variables->names[i], name))
      index = i;
  }

  if ( index != -1 ){
    /* free previous name and variable */
    clear_unbounded_int(variables->values[index]);
    if(NULL != variables->values[index]) free(variables->values[index]);
    if(NULL != name && name != variables->names[index]) free(variables->names[index]); 
    /* update */
    variables->names[index] = name;
    variables->values[index] = value;
  } else {
    if (variables->capacity == variables->length){
      resize_container(variables);
    }
    variables->names[variables->length] = name;
    variables->values[variables->length] = value;
    variables->length += 1;
  }
}

static unbounded_int * get(container *variables, char* name)
{
  unbounded_int *res = NULL;
  /* we can use a hashmap for better performance */
  for (int i=0; i < variables->length; ++i)
  {
    if (!strcmp(variables->names[i], name))
      res = variables->values[i];
  }
  return res;
}

static void clear_container(container *variables)
{
  if (variables != NULL) {
    for (int i=0; i < variables->length; ++i){
      free(variables->names[i]);
      clear_unbounded_int(variables->values[i]);
      if(variables->values[i]!=NULL) free(variables->values[i]);
    }
    free(variables->values);
    free(variables->names);
  }
}
/*************************************************/

static void skip_spaces(char *line, int line_len, int *cursor)
{
  while(*cursor < line_len){
    char c = line[*cursor];
    if (c==' ' || c=='\t'){
      *cursor += 1;
      continue;
    } else {
      return;
    }
  }
}

/* chars allowed in a variable name A-Za-z_ */
static int is_alpha(char c){
  if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_')
    return 1;
  return 0;
}

static int is_num(char c){
  if (c >= '0' && c <= '9')
    return 1;
  return 0;
}

static int is_op(char c){
  if (c == '-' || c == '+' || c == '*')
    return 1;
  return 0;
}

static int is_variable_name(char * name, int len){
  if (name == NULL || len <=0) return 0;
  if (is_num(name[0])) return 0;

  for(int i=1; i<len; ++i)
  {
    if (!is_num(name[i]) && !is_alpha(name[i])){
      return 0;
    }
  }
  return 1;
}

static int is_valid_integer(char *name, int len){
  if( len <= 0) return 0;
  int i = 0;
  int res = 0;
  if (name[0] == '-')
    i++;
  for(; i<len; ++i){
    if (!is_num(name[i]))
	return 0;
    res = 1; 
  }
  return res;
}

/* returns the next word. for "   12 -3 " return "12" and sets cursor to "-3 " */
static char * read_word(char *line, int line_len, int *cursor, int *word_len)
{
  *word_len = 0;
  skip_spaces(line, line_len, cursor);
  int word_start = *cursor;
  while(*cursor < line_len){
    char c = line[*cursor];
    if (c== ' ' || c == '\t' || c == '=' || c == '+' || c == '*'){
      break;
    }else if (c == '-'){
      if (*cursor >= line_len -1) break;
      if (!is_num(line[1+*cursor])) break;
    }
    *cursor += 1;
    *word_len += 1;
  }
  char *res = (char *)calloc(*word_len+1, sizeof(char));
  for (int k=0; k < *word_len; ++k)
    res[k] = line[word_start+k];
  res[*word_len] = '\0';
  skip_spaces(line, line_len, cursor);
  return res;
}

/* Three types of instructions are handled
[s]print [s]var[s]
[s]var[s]=[s]var[s]OP[s]var[s]
[s]var[s]=[s]UI[s]

where : [s] is eventual separators; OP is -, + or *; UI is unbounded int

*/
static void handle_instruction(char * line, int line_len,
			       container* variables, FILE *output)
{
  int cursor = 0;
  int word_len = 0;
  char* word = NULL;
  char* variable = NULL;
  const char* var_rule = "Variables need to start with alphabetic char or _";

  skip_spaces(line, line_len, &cursor);
  if (cursor == line_len){
    return;
  }

  word = read_word(line, line_len, &cursor, &word_len);

  if (!strcmp(word, "print"))
  {
    if(cursor >= line_len) {
      fprintf(stderr, "Error : \"print\" called without variable!");
      return;
    }else{
      variable = read_word(line, line_len, &cursor, &word_len);
      if (is_alpha(variable[0]) == 0) {
	fprintf(stderr, "Error: Variable name \"%s\", argument of \"print\" is not valid. %s\n", variable, var_rule);
	free(variable);
	return;
      }
      unbounded_int *res = get(variables, variable);
      if (res == NULL){
	fprintf(stderr, "Error : variable \"%s\", argument of \"print\", has not been assigned yet !\n", variable);
	free(variable);
	return;	
      }
      char *res_str = unbounded_int2string(*res);
      fprintf(output, "%s = %s\n", variable, res_str);
      free(variable);
      free(res_str);
      free(word);
      return;
    }
  } else if (0 == is_alpha(word[0]) || !is_variable_name(word, word_len)) {
    fprintf(stderr, "Error: Variable name \"%s\" is not valid. %s\n", word, var_rule);
    free(word);
    return;
  } else {
    if (line[cursor] != '='){
      fprintf(stderr, "Error : bad instruction, missing '='. \"%s\"\n", line);
      free(word);
      return;
    }
    cursor++;
    char * left = NULL;
    char * right = NULL;
    word_len = 0;
    left = read_word(line, line_len, &cursor, &word_len);

    if (left != NULL && word_len > 0)
    {
      if(is_valid_integer(left, word_len) && cursor == line_len) /* var = UI */
      {
	unbounded_int *res = (unbounded_int *) malloc(sizeof (struct unbounded_int));
	*res = string2unbounded_int(left);
	if (res->signe != '*'){
	  insert(variables, word, res);
        } else {
	  fprintf(stderr, "Error : could not parse int ! \"%s\"\n", left);
	}
	if(left != NULL) free(left);
	return;
      } else { /* var = entier_var OP entier_var */
	if (!is_op(line[cursor])){
	  fprintf(stderr, "Error : instruction operator missing! \"%s\"\n", line);
	  if(left != NULL) free(left);
	  return;
	}
	char op = line[cursor++];
	
	unbounded_int *left_ui = NULL;
	int local_left = 0;
	unbounded_int *right_ui = NULL;
	int local_right = 0;
	if(is_valid_integer(left, word_len)){
	  left_ui = (unbounded_int *) malloc(sizeof (struct unbounded_int));
	  *left_ui = string2unbounded_int(left);
	  if (left_ui->signe == '*'){
	    fprintf(stderr, "Error : could not parse left int ! \"%s\"\n", left);
	    return;
	  }
	  local_left = 1;
	} else if(is_variable_name(left, word_len)){
	  left_ui = get(variables, left);
	  if (left_ui == NULL){
	    fprintf(stderr, "Error : variable \"%s\" has not been assigned yet !\n", left);
	    free(left);
	    return;
	  }
	}
	word_len = 0;
	right = read_word(line, line_len, &cursor, &word_len);	
	if(is_valid_integer(right, word_len)){
	  right_ui = (unbounded_int *) malloc(sizeof (struct unbounded_int));
	  *right_ui = string2unbounded_int(right);
	  if (right_ui->signe == '*'){
	    fprintf(stderr, "Error : could not parse right int ! \"%s\"\n", right);
	    return;
	  }
	  local_right = 1;
	} else if(is_variable_name(right, word_len)){
	  right_ui = get(variables, right);
	  if (right_ui == NULL){
	    fprintf(stderr, "Error : variable \"%s\" has not been assigned yet !\n", right);
	    free(right);
	    return;
	  }
	}
	
	unbounded_int *res = (unbounded_int *) malloc(sizeof (struct unbounded_int));
	if ('+' == op){
	  *res = unbounded_int_somme(*left_ui, *right_ui);
	} else if ('-' == op) {
	  *res = unbounded_int_difference(*left_ui, *right_ui);
	} else { /* op == * */
	  *res = unbounded_int_produit(*left_ui, *right_ui);
	}

	insert(variables, word, res);

	if (local_left){
	  clear_unbounded_int(left_ui);
	  if(left_ui != NULL)
	    free(left_ui);
	}
	if (left != NULL) free(left);

	if (local_right){
	  clear_unbounded_int(right_ui);
	  if(right_ui != NULL)
	    free(right_ui);
	}
	if (right != NULL) free(right);
      }
    } else {
      fprintf(stderr, "Error : bad instruction! \"%s\"\n", line);
      return;
    }
  }

  if(cursor >= line_len) return;
  
}

static void interpreter(FILE *input, FILE *output)
{
  container *variables = (container *)malloc(sizeof(struct container));
  init_container(variables);

  int chunk_size = LINE_CHUNK;
  int current_size = chunk_size;
  char *line = (char *) calloc(current_size, sizeof(char));
  int c;
  int k = 0;
  while(!feof(input)){
    c = fgetc(input);
    if(c == '\n'){
      handle_instruction(line, k, variables, output);
      free(line);
      line = (char *) calloc(chunk_size, sizeof(char));
      current_size = chunk_size;
      k = 0;
      continue;
    }

    line[k++] = c;
    /* handling lines longer than 1024 */
    if(k==current_size){
      char* new_line = (char *) calloc(current_size+chunk_size, sizeof(char));
      for(int i=0; i<current_size; ++i) new_line[i] = line[i];
      current_size += chunk_size;
      free(line);
      line = new_line;
    }
  }
  if (NULL != line)
    free(line);
  clear_container(variables);
  free(variables);
}

int error_message(char *msg)
{
  printf("BAD INPUT : %s\n", msg);
  printf("\nusage : ./calc_unbounded_int [-i input.txt] [-o output.txt]\n");
  return -1;
}

int main(int argc, char *argv[])
{
  FILE *input = stdin;
  FILE *output = stdout;

  char* input_filename = NULL;
  char* output_filename = NULL;

  if (argc > 1)
  {
    for(int k=1; k < argc -1; k+=2){
      if (!strcmp(argv[k], "-i")){
	input_filename = argv[k+1];
      } else if (!strcmp(argv[k], "-o")){
	output_filename = argv[k+1];
      } else {
	return error_message("Arguments can only be -i or -o followed by filename");
      }
    }
    if(NULL != input_filename) {
      input = fopen(input_filename, "r");
      if (NULL == input) {
	return error_message("Could not open input file");
      }
    }
    if(NULL != output_filename) {
      output = fopen(output_filename, "w");
      if (NULL == output) {
	return error_message("Could not open output file");
      }
    }
  }

  interpreter(input, output);

  if (input_filename != NULL) fclose(input);
  if (output_filename != NULL) fclose(output);

  return 0;
}
