
#ifndef UTILS_H_INCLUDED	/* Include guard */
#define UTILS_H_INCLUDED
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <alloca.h>
#include <time.h>
#include <unistd.h>
#include <malloc.h>
#include "data.h"

void print_dict(word_dict *   d);
void print_word_vals(word_val *   list);
stack *parse_file(FILE * fp);
stack *parse_str(char *  fp, long int *  ctr);
long int validInt(char *  s);
long int validDouble(char *  s);
void print_stack(stack *   s, long int level);
void print_stack_aux(stack *   s, long int level, char *  p1,
		     char *  p2);
void print_vec_aux(vector *   s, long int level, char *  p1,
		   char * p2);
void print_vec_aux(vector *   s, long int level, char *  p1,
		   char *  p2);
void print_l_val(l_val *   val);
void print_level(long int level, char *  p);

#endif				// UTILS_H_
