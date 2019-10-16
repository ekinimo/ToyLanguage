
#ifndef DATA_H_INCLUDED		/* Include guard */
#define DATA_H_INCLUDED
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <alloca.h>
#include <time.h>
#include <unistd.h>
#include <malloc.h>
#include <stdbool.h>

typedef enum { ERROR, INT, DOUBLE, CHAR, BOOL, STR, STACK, WORD, STREAM, SYMBOL,
	VEC
} types;
typedef enum { PRIM_FUN, USER_FUN } word_flag;

typedef struct e_ err_val;
typedef struct w_k word_key;
typedef struct w_v word_val;
typedef struct w_d word_dict;
typedef struct l l_val;
typedef struct s stack;
typedef void (*func) (stack **, word_dict **);
typedef struct v vector;

struct w_k {
	char *name;
	unsigned long int key1;
};

struct w_v {
	char *name;
	word_flag flag;
	union {
		stack *data;
		func f;
	};
	word_val *next;
};

struct w_d {
	long int len;
	word_val **data;
};

struct l {
	types T;
	union {
		long int err_code;
		long int i_val;
		char c_val;
		bool b_val;
		char *str_val;
		word_key *word_val;
		double d_val;
		stack *s_val;
		FILE *fp;
		l_val *sym_val;
		vector *vec_val;
	};
};

struct s {

	long int len;
	long int top;
	l_val **data;
};

struct v {
	types T;
	long int len;
	long int top;
	union {
		long int *err_code;
		long int *i_vec;
		double *d_vec;
		char *c_vec;
		char *b_vec;
		char **str_vec;
		word_key **word_vec;
		stack **s_vec;
		FILE **fp_vec;
		l_val **sym_vec;
		vector **vec_vec;
	};
};

stack *init_stack(long int len);
vector *init_vec(long int len, types T);
word_key *init_word_key(char *  i);
word_dict *init_dict(long int len);

void free_stack(stack *   s);
void free_word_vals(word_val *   list);

void free_word_key(word_key *   w);
void free_vec(vector *   l);
void free_l_val(l_val *   l);
void free_dict(word_dict *   d);

void grow_stack(stack **   user_array);
void grow_vec(vector **   user_array);

unsigned long int hash(char *  s) __attribute__ ((pure));

void check_stack_size(stack **   s);
void check_vec_size(vector **   s);

types get_vec_type(vector *   s, long int *  c) /*__attribute__ ((pure))*/;

long int check_vec_type_aux(vector *   s1, vector *   s2)
    /*__attribute__ ((pure))*/;
long int check_vec_type(vector *   s1, l_val *   i)
    /*__attribute__ ((pure))*/;

void push_stack(stack ** s, l_val * i);
l_val *pop_stack(stack **   s);
stack *split_stack(stack *   s, long int n);

void push_vec(vector **   s, l_val *   i);
l_val *pop_vec(vector **   s);
vector *split_vec(vector *   s, long int n);

l_val *l_val_err(long int i);
l_val *l_val_int(long int i);
l_val *l_val_double(double i);
l_val *l_val_char(char i);
l_val *l_val_bool(bool i);
l_val *l_val_file(FILE * fp);
l_val *l_val_string(char *  i);
l_val *l_val_word(char *  i);
l_val *l_val_stack(stack *   i);
l_val *l_val_vec(vector *   i);
l_val *l_val_sym(l_val *   i);

l_val *copy_lval(l_val *   s) /*__attribute__ ((pure))*/;
stack *copy_stack(stack *   s) /*__attribute__ ((pure))*/;
vector *copy_vec(vector *   val) /*__attribute__ ((pure))*/;

vector *stack2vec(stack *   s);

word_val *prim_func(char *  name, func f);
word_val *user_func(char *  name, stack *   f);
void append_val(word_val **   list, word_val *   val);
void add_dict(word_dict **   dict, unsigned long int key,
	      word_val *   val);
word_val *retrieve_dict(word_dict *   dict, word_key *   key);
void link_stacks(stack **   s, stack *   body);

void vector_set(vector *   vec, l_val *   val, long int n);
l_val *vector_get(vector * vec, long int n);
#endif				// DATA_H_
