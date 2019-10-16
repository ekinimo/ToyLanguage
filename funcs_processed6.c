#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <alloca.h>
#include <time.h>
#include <unistd.h>
#include <malloc.h>

#include "data.h"

#include "utils.h"

// #include <jemalloc/jemalloc.h>

#define typecheck(stack,...) do                                         \
    {                                                                   \
      long int check = 0;                                                    \
      types Types[] = { __VA_ARGS__ };                                  \
      long int L = sizeof(Types) / sizeof Types[0];                          \
      long int stack_len = (*s)->top;                                        \
      if(L>stack_len){                                                  \
        printf("Stack length ( %ld ) is less than requested number of args ( %ld )  in %s,\n",(*s)->top,L,__FUNCTION__); \
        l_val* ret = l_val_err(1);                                      \
        push_stack(stack,ret);                                          \
        return;                                                         \
      }                                                                 \
      for (long int i = 0; i < L; i++){                                      \
        if((*s)->data[stack_len-1-i]->T == Types[i]){                   \
          check=1;break;                                                \
        }                                                               \
      }                                                                 \
      if(check==0){                                                     \
        printf("type error in %s,exiting",__FUNCTION__);                    \
        l_val* ret = l_val_err(1);                                      \
        push_stack(stack,ret);                                          \
        return;                                                         \
      }                                                                 \
    }while(0)                                                           \


void def(stack **   s, word_dict **   d)
{
	typecheck(s, STACK, STACK);
	l_val *name_val = pop_stack(s);
	l_val *body_val = pop_stack(s);

	stack *name_stack = name_val->s_val;
	stack *body_stack = body_val->s_val;

	free(name_val);
	free(body_val);

	l_val *name_key_val = pop_stack(&name_stack);

	word_key *name_key = name_key_val->word_val;

	free(name_key_val);
	free_stack(name_stack);

	long int n = name_key->key1;
	char *name = strdup(name_key->name);
	word_val *val = user_func(name, body_stack);

	free(name_key->name);
	free(name_key);
	add_dict(d, n, val);
}

void int2double(stack **   s, word_dict **   d)
{
	typecheck(s, INT);
	l_val *dummy = (*s)->data[(*s)->top - 1];

	dummy->d_val = (double)dummy->i_val;
	dummy->T = DOUBLE;
}

void double2int(stack **   s, word_dict **   d)
{
	typecheck(s, DOUBLE);
	l_val *dummy = (*s)->data[(*s)->top - 1];

	dummy->i_val = (int)dummy->i_val;
	dummy->T = INT;
}

void apply(stack **   s, word_dict **   d)
{
	typecheck(s, STACK);
	l_val *dummy = pop_stack(s);
	stack *val = dummy->s_val;
	for (long int i = 0; i < val->top; i++) {
		l_val *new_val = val->data[i];
		push_stack(s, new_val);

	}

	free(val->data);
	free(val);
	free(dummy);

}

stack *eval(stack *   s, word_dict **   d);
void apply_v(stack **   s, word_dict **   d)
{
	typecheck(s, VEC);
	l_val *dummy_vec = pop_stack(s);
	vector *val = dummy_vec->vec_val;
	for (long int i = 0; i < val->top; i++) {
		stack *instr = val->s_vec[i];
		stack *copy = copy_stack(*s);
		link_stacks(&copy, instr);

		val->s_vec[i] = eval(copy, d);

	}
	push_stack(s, dummy_vec);
}

void
eval_word(word_key *   k1, stack **   s,
	  word_dict **   dict)
{
	word_val *ret1 = retrieve_dict(*dict, k1);
	if (ret1 == NULL) {
		printf("\n  word not found \n");
		return;
	}
	switch (ret1->flag) {
	case USER_FUN:
		{
			// print_stack(ret1->data->data[0]->s_val, 0);
			// stack* dummy = copy_stack(ret1->data);
			for (long int i = 0; i < ret1->data->top; i++) {
				l_val *new_val = copy_lval(ret1->data->data[i]);
				push_stack(s, new_val);
			}
			break;
		}
	case PRIM_FUN:
		{
			func f = ret1->f;
			f(s, dict);
			break;
		}
	}

	free_word_vals(ret1);

	// free_word_key(k1);
}

stack *eval(stack *   s, word_dict **   d)
{

	stack *new_stack = init_stack((s)->len);
	for (long int i = 0; i < s->top; i++) {

		l_val *val = s->data[i];
		switch (val->T) {
		case ERROR:
			{
				push_stack(&new_stack, val);
				printf("\n error in eval \n");	// exit(0);}
				break;
			}
		case INT ... STACK:
			{
				push_stack(&new_stack, val);
				// (*s)->data[i]=NULL;
				// free(dummy); */
				break;
			}
		case SYMBOL:
			{
				push_stack(&new_stack, val);
				break;
			}
		case VEC:
			{
				push_stack(&new_stack, val);
				break;
			}
		case WORD:
			{
				eval_word(val->word_val, &new_stack, d);
				new_stack = eval(new_stack, d);
				free(val);

				// (*s)->data[i]=NULL;
				// eval(&new_stack, d);
				break;
			}
		case STREAM:
			{
				push_stack(&new_stack, val);
				break;
			}
		}

		// free(val);
	}

	// printf("--------------------\n");
	// print_stack(new_stack,0);

	free(s->data);
	free(s);
	/*
	 *s = new_stack; */
	return new_stack;
}

long int reshape_aux_cnt(stack *   shape, long int mem);

long int reshape_aux_cnt_vec(vector *   shape, long int mem)
{
	switch (shape->T) {
	case INT:{
			for (long int i = 0; i < shape->top; i++) {
				long int dummy = shape->i_vec[i];
				mem = dummy > mem ? dummy : mem;
			}
			break;
		}
	case STACK:{
			for (long int i = 0; i < shape->top; i++) {
				mem = reshape_aux_cnt(shape->s_vec[i], mem);
			}
			break;
		}
	case WORD:{
			break;
		}
	case SYMBOL:{
			break;
		}
	case VEC:{
			for (long int i = 0; i < shape->top; i++) {
				mem =
				    reshape_aux_cnt_vec(shape->vec_vec[i], mem);

			}
			break;
		}
	default:{
			printf
			    ("ERROR in reshape2 unknown datatype encountered in reshape2_aux_mem_cnt exiting");
			// exit(0);
			return -10;
		}
	}
	return mem;
}

long int reshape_aux_cnt(stack *   shape, long int mem)
{

	for (long int i = 0; i < shape->top; i++) {
		switch (shape->data[i]->T) {
		case INT:{
				long int dummy = shape->data[i]->i_val;
				mem = dummy > mem ? dummy : mem;
				break;
			}
		case STACK:{
				mem =
				    reshape_aux_cnt(shape->data[i]->s_val, mem);
				break;
			}
		case WORD:{
				break;
			}
		case SYMBOL:{
				break;
			}
		case VEC:{
				mem =
				    reshape_aux_cnt_vec(shape->data[i]->vec_val,
							mem);
				break;
			}

		default:{
				printf
				    ("ERROR in reshape2 unknown datatype encountered in reshape2_aux_mem_cnt exiting");
				// exit(0);
				return -10;
			}
		}
	}
	return mem;

}

void reshape_aux_mem_cnt(stack *   shape, char **  mem);
void reshape_aux_mem_cnt_vec(vector *   shape, char **  mem)
{
	switch (shape->T) {
	case INT:
		{
			for (long int i = 0; i < shape->top; i++) {
				(*mem)[shape->i_vec[i]] += 1;

			}
			break;
		}
	case STACK:
		{
			for (long int i = 0; i < shape->top; i++) {
				reshape_aux_mem_cnt(shape->s_vec[i], mem);
			}
			break;
		}
	case WORD:{
			break;
		}
	case VEC:{
			for (long int i = 0; i < shape->top; i++) {
				reshape_aux_mem_cnt_vec(shape->vec_vec[i], mem);
			}
			break;
		}

	case SYMBOL:{
			break;
		}
	default:{
			printf
			    ("ERROR in reshape2 unknown datatype encountered in reshape2_aux_mem_cnt exiting");
			// exit(0);
			return;
		}

	}
}

void reshape_aux_mem_cnt(stack *   shape, char **  mem)
{

	for (long int i = 0; i < shape->top; i++) {
		switch (shape->data[i]->T) {
		case INT:
			{
				(*mem)[shape->data[i]->i_val] += 1;
				break;
			}
		case STACK:{
				reshape_aux_mem_cnt(shape->data[i]->s_val, mem);
				break;
			}
		case WORD:{
				break;
			}
		case VEC:{
				reshape_aux_mem_cnt_vec(shape->data[i]->vec_val,
							mem);
				break;
			}
		case SYMBOL:{
				break;
			}
		default:{
				printf
				    ("ERROR in reshape2 unknown datatype encountered in reshape2_aux_mem_cnt exiting");
				// exit(0);
				return;
			}
		}
	}
}

void


reshape_aux_shape(stack **   s, stack *   shape,
		  word_dict **   dict, l_val **   temp_stack,
		  char **  mem);

void
reshape_aux_shape_vec(stack **   s, vector *   shape,
		      word_dict **   dict,
		      l_val **   temp_stack, char **  mem)
{

	switch (shape->T) {
	case INT:{
			for (long int i = 0; i < shape->top; i++) {
				long int var_cnt = shape->i_vec[i];
				if ((*mem)[var_cnt] > 1) {
					push_stack(s,
						   copy_lval(temp_stack
							     [var_cnt]));
					(*mem)[var_cnt] -= 1;
				} else {
					push_stack(s, temp_stack[var_cnt]);
				}
			}
			break;
		}
	case STACK:{
			for (long int i = 0; i < shape->top; i++) {
				stack *new_stack =
				    init_stack(shape->s_vec[i]->top);

				reshape_aux_shape(&new_stack, shape->s_vec[i],
						  dict, temp_stack, mem);

				push_stack(s, l_val_stack(new_stack));
			}
			break;
		}
	case VEC:{
			for (long int i = 0; i < shape->top; i++) {

				stack *new_stack =
				    init_stack(shape->vec_vec[i]->top);

				reshape_aux_shape_vec(&new_stack,
						      shape->vec_vec[i], dict,
						      temp_stack, mem);

				vector *v = stack2vec(new_stack);
				push_stack(s, l_val_vec(v));

			}
			break;
		}
	case WORD:{
			for (long int i = 0; i < shape->top; i++) {
				char *dum = strdup(shape->word_vec[i]->name);
				l_val *w_val = l_val_word(dum);
				push_stack(s, w_val);
			}
			break;
		}
	case SYMBOL:{
			for (long int i = 0; i < shape->top; i++) {
				l_val *val = copy_lval(shape->sym_vec[i]);
				push_stack(s, val);

			}
			break;
		}
	default:{
			printf
			    ("ERROR in reshape2 unknown datatype encountered in reshape2_aux_shapeexiting");
			return;
		}
	}
}

void
reshape_aux_shape(stack **   s, stack *   shape,
		  word_dict **   dict, l_val **   temp_stack,
		  char **  mem)
{
	for (long int i = 0; i < shape->top; i++) {
		switch (shape->data[i]->T) {
		case INT:{
				long int var_cnt = shape->data[i]->i_val;
				if ((*mem)[var_cnt] > 1) {
					push_stack(s,
						   copy_lval(temp_stack
							     [var_cnt]));
					(*mem)[var_cnt] -= 1;
				} else {
					push_stack(s, temp_stack[var_cnt]);
				}
				break;
			}
		case STACK:
			{
				stack *new_stack =
				    init_stack(shape->data[i]->s_val->top);

				reshape_aux_shape(&new_stack,
						  shape->data[i]->s_val, dict,
						  temp_stack, mem);

				push_stack(s, l_val_stack(new_stack));

				break;
			}
		case VEC:{
				stack *new_stack =
				    init_stack(shape->data[i]->vec_val->top);

				reshape_aux_shape_vec(&new_stack,
						      shape->data[i]->vec_val,
						      dict, temp_stack, mem);

				vector *v = stack2vec(new_stack);
				push_stack(s, l_val_vec(v));
				break;
			}

		case WORD:
			{

				char *dum =
				    strdup(shape->data[i]->word_val->name);
				l_val *w_val = l_val_word(dum);
				push_stack(s, w_val);
				break;
			}
		case SYMBOL:
			{
				l_val *val = copy_lval(shape->data[i]->sym_val);
				push_stack(s, val);
				break;
			}
		default:{
				printf
				    ("ERROR in reshape2 unknown datatype encountered in reshape2_aux_shapeexiting");
				return;
			}
		}
	}
}

void
reshape_aux(stack **   s, word_dict **   dict,
	    stack *   shape)
{
	long int len;
	len = 0;
	len = reshape_aux_cnt(shape, len) + 1;
	if (len < 0) {
		return;
	}
	//printf("%s len : %ld ",len,__FUNCTION__);
	long int s_len = (*s)->top;
	if (len - 1 > s_len) {
		printf
		    ("Error in reshape,\n  stack length %ld is less than requeired len %ld, exiting",
		     s_len, len);
		//exit(0);
		return;
	}
	// temp_stack to hold vars
	// l_val** temp_stack = alloca(len*sizeof(l_val*));
	l_val **temp_stack = calloc(len, sizeof(l_val *));

	// dictionary to count occurences
	char *mem = calloc((len), sizeof(char));
	// char* mem = alloca((len)*sizeof(char));
	// memset(mem, 0, len);

	// copy vars to temp_stack
	for (long int i = 0; i < len; i++) {
		temp_stack[i] = pop_stack(s);
	}

	// fill the dict /count occurences
	reshape_aux_mem_cnt(shape, &mem);

	// printf("mem : %s",mem);
	reshape_aux_shape(s, shape, dict, temp_stack, &mem);

	// reclaim the memory for unused vars
	for (long int i = 0; i < len; i++) {
		if (mem[i] == 0) {
			if (temp_stack[i]->T == STACK) {
				free_stack(temp_stack[i]->s_val);
			}
			if (temp_stack[i]->T == WORD) {
				free_word_key(temp_stack[i]->word_val);
			}
			if (temp_stack[i]->T == SYMBOL) {
				free_l_val(temp_stack[i]->sym_val);
			}
			if (temp_stack[i]->T == VEC) {
				free_vec(temp_stack[i]->vec_val);
			}
			free(temp_stack[i]);

		}
	}
	free(mem);
	free(temp_stack);
	free_stack(shape);
}

void isError(stack **   s, word_dict **   d)
{
	l_val *v1 = (*s)->data[(*s)->top - 1];
	types T = v1->T;
	push_stack(s, l_val_bool(T == ERROR));
}

void isInt(stack **   s, word_dict **   d)
{
	l_val *v1 = (*s)->data[(*s)->top - 1];
	types T = v1->T;
	push_stack(s, l_val_bool(T == INT));
}

void isDouble(stack **   s, word_dict **   d)
{
	l_val *v1 = (*s)->data[(*s)->top - 1];
	types T = v1->T;
	push_stack(s, l_val_bool(T == DOUBLE));
}

void isBool(stack **   s, word_dict **   d)
{
	l_val *v1 = (*s)->data[(*s)->top - 1];
	types T = v1->T;
	push_stack(s, l_val_bool(T == BOOL));
}

void isChar(stack **   s, word_dict **   d)
{
	l_val *v1 = (*s)->data[(*s)->top - 1];
	types T = v1->T;
	push_stack(s, l_val_bool(T == CHAR));
}

void isString(stack **   s, word_dict **   d)
{
	l_val *v1 = (*s)->data[(*s)->top - 1];
	types T = v1->T;
	push_stack(s, l_val_bool(T == STR));
}

void isStack(stack **   s, word_dict **   d)
{
	l_val *v1 = (*s)->data[(*s)->top - 1];
	types T = v1->T;
	push_stack(s, l_val_bool(T == STACK));
}

void isFile(stack **   s, word_dict **   d)
{
	l_val *v1 = (*s)->data[(*s)->top - 1];
	types T = v1->T;
	push_stack(s, l_val_bool(T == STREAM));
}

void isWord(stack **   s, word_dict **   d)
{
	l_val *v1 = (*s)->data[(*s)->top - 1];
	types T = v1->T;
	push_stack(s, l_val_bool(T == WORD));
}

void isVec(stack **   s, word_dict **   d)
{
	l_val *v1 = (*s)->data[(*s)->top - 1];
	types T = v1->T;
	push_stack(s, l_val_bool(T == VEC));
}

void isSymbol(stack **   s, word_dict **   d)
{
	l_val *v1 = (*s)->data[(*s)->top - 1];
	types T = v1->T;
	push_stack(s, l_val_bool(T == SYMBOL));
}

void isErrorVec(stack **   s, word_dict **   d)
{
	l_val *v1 = (*s)->data[(*s)->top - 1];
	types T1 = v1->T;
	types T2 = v1->vec_val->T;
	push_stack(s, l_val_bool(T1 == VEC && T2 == ERROR));
}

void isIntVec(stack **   s, word_dict **   d)
{
	l_val *v1 = (*s)->data[(*s)->top - 1];
	types T1 = v1->T;
	types T2 = v1->vec_val->T;
	push_stack(s, l_val_bool(T1 == VEC && T2 == INT));
}

void isDoubleVec(stack **   s, word_dict **   d)
{
	l_val *v1 = (*s)->data[(*s)->top - 1];
	types T1 = v1->T;
	types T2 = v1->vec_val->T;
	push_stack(s, l_val_bool(T1 == VEC && T2 == DOUBLE));
}

void isCharVec(stack **   s, word_dict **   d)
{
	l_val *v1 = (*s)->data[(*s)->top - 1];
	types T1 = v1->T;
	types T2 = v1->vec_val->T;
	push_stack(s, l_val_bool(T1 == VEC && T2 == CHAR));
}

void isBoolVec(stack **   s, word_dict **   d)
{
	l_val *v1 = (*s)->data[(*s)->top - 1];
	types T1 = v1->T;
	types T2 = v1->vec_val->T;
	push_stack(s, l_val_bool(T1 == VEC && T2 == BOOL));
}

void isStringVec(stack **   s, word_dict **   d)
{
	l_val *v1 = (*s)->data[(*s)->top - 1];
	types T1 = v1->T;
	types T2 = v1->vec_val->T;
	push_stack(s, l_val_bool(T1 == VEC && T2 == STR));
}

void isStackVec(stack **   s, word_dict **   d)
{
	l_val *v1 = (*s)->data[(*s)->top - 1];
	types T1 = v1->T;
	types T2 = v1->vec_val->T;
	push_stack(s, l_val_bool(T1 == VEC && T2 == STACK));
}

void isWordVec(stack **   s, word_dict **   d)
{
	l_val *v1 = (*s)->data[(*s)->top - 1];
	types T1 = v1->T;
	types T2 = v1->vec_val->T;
	push_stack(s, l_val_bool(T1 == VEC && T2 == WORD));
}

void isVecVec(stack **   s, word_dict **   d)
{
	l_val *v1 = (*s)->data[(*s)->top - 1];
	types T1 = v1->T;
	types T2 = v1->vec_val->T;
	push_stack(s, l_val_bool(T1 == VEC && T2 == VEC));
}

void isFileVec(stack **   s, word_dict **   d)
{
	l_val *v1 = (*s)->data[(*s)->top - 1];
	types T1 = v1->T;
	types T2 = v1->vec_val->T;
	push_stack(s, l_val_bool(T1 == VEC && T2 == VEC));
}

void isSymbolVec(stack **   s, word_dict **   d)
{
	l_val *v1 = (*s)->data[(*s)->top - 1];
	types T1 = v1->T;
	types T2 = v1->vec_val->T;
	push_stack(s, l_val_bool(T1 == VEC && T2 == SYMBOL));
}

void get_char(stack **   s, word_dict **   d)
{
	char c = fgetc(stdin);
	l_val *ret = l_val_char(c);
	push_stack(s, ret);
}

void put_char(stack **   s, word_dict **   d)
{
	typecheck(s, CHAR);
	l_val *ret = pop_stack(s);

	char c = ret->c_val;
	fputc(c, stdout);
}

void lang_if(stack **   s, word_dict **   d)
{
	typecheck(s, STACK, STACK, BOOL);
	l_val *fls = pop_stack(s);
	l_val *tre = pop_stack(s);
	l_val *cnd = pop_stack(s);
	if (cnd->b_val == 0) {
		link_stacks(s, fls->s_val);
		free(fls);
		free_stack(tre->s_val);
		free(tre);
	} else {
		link_stacks(s, tre->s_val);
		free(tre);
		free_stack(fls->s_val);
		free(fls);
	}
	free(cnd);
	// *s =eval(*s, d);
}

void reshape(stack **   s, word_dict **   d)
{
	typecheck(s, STACK);
	l_val *shape = pop_stack(s);

	reshape_aux(s, d, shape->s_val);
	// eval(*s,d);
	free(shape);
	// eval(*s,d);
}

void reshape_s(stack **   s, word_dict **   d)
{
	typecheck(s, STACK, STACK);
	l_val *shape = pop_stack(s);
	l_val *stk = pop_stack(s);
	stack *temp = stk->s_val;
	reshape_aux(&temp, d, shape->s_val);
	push_stack(s, stk);
	// eval(*s,d);
	free(shape);
	// eval(*s,d);
}

void reshape_vs(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	bool cond1 = (*s)->data[(*s)->top - 1]->vec_val->T == STACK;
	bool cond2 = (*s)->data[(*s)->top - 2]->vec_val->T == STACK;
	if (cond1 && cond2) {
		l_val *l_s1 = pop_stack(s);
		l_val *l_s2 = pop_stack(s);
		const long int n = l_s1->vec_val->top;
		for (long int i = 0; i < n; i++) {
			stack *st = l_s2->vec_val->s_vec[i];
			reshape_aux(&st, d, l_s1->vec_val->s_vec[i]);
		}
		free(l_s1->vec_val->s_vec);
		free(l_s1->vec_val);
		free(l_s1);
		push_stack(s, l_s2);
	} else {
		push_stack(s, l_val_err(1));
	}

}

void reshape_v_whole_stack(stack **   s, word_dict **   d)
{
	typecheck(s, VEC);
	l_val *dummy_vec = pop_stack(s);
	vector *val = dummy_vec->vec_val;
	for (long int i = 0; i < val->top; i++) {
		stack *shape = val->s_vec[i];
		stack *copy = copy_stack(*s);
		reshape_aux(&copy, d, shape);
		val->s_vec[i] = copy;

	}
	push_stack(s, dummy_vec);
}

void lang_eval(stack **   s, word_dict **   d)
{
	typecheck(s, STACK);
	l_val *l_s = pop_stack(s);

	l_s->s_val = eval(l_s->s_val, d);
	push_stack(s, l_s);
}

void lang_eval_v(stack **   s, word_dict **   d)
{
	typecheck(s, VEC);
	if ((*s)->data[(*s)->top - 1]->vec_val->T == STACK) {
		l_val *l_s = pop_stack(s);
		for (long int i = 0; i < l_s->vec_val->top; i++) {
			l_s->vec_val->s_vec[i] =
			    eval(l_s->vec_val->s_vec[i], d);
		}
		push_stack(s, l_s);
	} else {
		push_stack(s, l_val_err(1));
	}

}

void lang_s_split(stack **   s, word_dict **   d)
{
	typecheck(s, STACK, INT);
	long int n = (*s)->data[(*s)->top - 2]->i_val;
	long int s_len = (*s)->data[(*s)->top - 1]->s_val->top;
	printf("%ld , %ld ", n, s_len);
	if (s_len > n) {
		l_val *stk = pop_stack(s);
		l_val *num = pop_stack(s);
		stack *st = stk->s_val;
		stack *ret_stk = split_stack(st, num->i_val);
		push_stack(s, stk);
		push_stack(s, l_val_stack(ret_stk));
		free(num);
	} else {
		push_stack(s, l_val_err(1));
	}
}

void lang_v_split(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, INT);
	long int n = (*s)->data[(*s)->top - 2]->i_val;
	long int s_len = (*s)->data[(*s)->top - 1]->s_val->top;
	printf("%ld , %ld ", n, s_len);
	if (s_len > n) {
		l_val *stk = pop_stack(s);
		l_val *num = pop_stack(s);
		vector *st = stk->vec_val;
		vector *ret_stk = split_vec(st, num->i_val);
		push_stack(s, stk);
		push_stack(s, l_val_vec(ret_stk));
		free(num);
	} else {
		push_stack(s, l_val_err(1));
	}
}

void lang_v_split_v(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	vector *v1 = (*s)->data[(*s)->top - 1]->vec_val;
	vector *split_num = (*s)->data[(*s)->top - 2]->vec_val;
	if (v1->T == VEC && split_num->T == INT && split_num->top == v1->top) {
		vector *ret = init_vec(v1->top + 1, VEC);
		for (long int i = 0; i < v1->top; i++) {
			ret->vec_vec[i] =
			    split_vec(v1->vec_vec[i], split_num->i_vec[i]);
		}
		ret->top = v1->top;
		l_val *s_vec = pop_stack(s);
		free_vec(split_num);
		free(pop_stack(s));

		push_stack(s, s_vec);
		push_stack(s, l_val_vec(ret));

	} else {
		push_stack(s, l_val_err(1));
	}
}

void lang_s_split_v(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	vector *v1 = (*s)->data[(*s)->top - 1]->vec_val;
	vector *split_num = (*s)->data[(*s)->top - 2]->vec_val;
	if (v1->T == STACK && split_num->T == INT && split_num->top == v1->top) {
		vector *ret = init_vec(v1->top + 1, STACK);
		for (long int i = 0; i < v1->top; i++) {
			ret->s_vec[i] =
			    split_stack(v1->s_vec[i], split_num->i_vec[i]);
		}
		ret->top = v1->top;
		l_val *s_vec = pop_stack(s);
		free_vec(split_num);
		free(pop_stack(s));

		push_stack(s, s_vec);
		push_stack(s, l_val_vec(ret));

	} else {
		push_stack(s, l_val_err(1));
	}
}

void lang_link(stack **   s, word_dict **   d)
{
	typecheck(s, STACK, STACK);
	l_val *l_s1 = pop_stack(s);
	l_val *l_s2 = pop_stack(s);
	stack *st = l_s2->s_val;
	link_stacks(&st, l_s1->s_val);
	push_stack(s, l_s2);
	free(l_s1);

}

void lang_link_v(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	if ((*s)->data[(*s)->top - 1]->vec_val->T == STACK) {
		l_val *l_s1 = pop_stack(s);
		l_val *l_s2 = pop_stack(s);

		for (long int i = 0; i < l_s1->vec_val->top; i++) {
			stack *st = l_s2->vec_val->s_vec[i];
			link_stacks(&st, l_s1->vec_val->s_vec[i]);

		}
		free(l_s1->vec_val->s_vec);
		free(l_s1->vec_val);
		free(l_s1);
		push_stack(s, l_s2);
	} else {
		push_stack(s, l_val_err(1));
	}

}

void lang_get_def(stack **   s, word_dict **   d)
{
	typecheck(s, STACK);
	l_val *val = pop_stack(s);
	// typecheck(s,"get_def", val, STACK);

	l_val *dum = pop_stack(&val->s_val);
	// typecheck(s,"get_def", dum, STACK);

	word_key *key = dum->word_val;
	word_val *w_val = retrieve_dict(*d, key);

	l_val *ret = l_val_stack(w_val->data);

	push_stack(s, ret);

	free(w_val);
	free(dum);
	free(val->s_val);
	free(val);

}

void lang_pop(stack **   s, word_dict **   d)
{
	typecheck(s, STACK);
	l_val *l = pop_stack(s);
	stack *st = l->s_val;
	l_val *new_val = pop_stack(&st);
	push_stack(s, l);
	push_stack(s, new_val);
}

void lang_pop_v(stack **   s, word_dict **   d)
{
	typecheck(s, VEC);
	l_val *l = pop_stack(s);
	vector *st = l->vec_val;
	l_val *new_val = pop_vec(&st);
	push_stack(s, l);
	push_stack(s, new_val);
}

void lang_push_v(stack **   s, word_dict **   d)
{

	if ((*s)->data[(*s)->top - 2]->T != VEC
	    || (*s)->data[(*s)->top - 1]->T !=
	    (*s)->data[(*s)->top - 2]->vec_val->T) {
		push_stack(s, l_val_err(1));
		return;
	}

	l_val *new_val = pop_stack(s);
	l_val *l = (*s)->data[(*s)->top - 1];

	// typecheck(s,"push", l, STACK);
	vector *st = l->vec_val;
	push_vec(&st, new_val);
}

void lang_push(stack **   s, word_dict **   d)
{

	if ((*s)->data[(*s)->top - 2]->T != STACK) {
		push_stack(s, l_val_err(1));
		return;
	}
	l_val *new_val = pop_stack(s);
	l_val *l = (*s)->data[(*s)->top - 1];

	// typecheck(s,"push", l, STACK);
	stack *st = l->s_val;
	push_stack(&st, new_val);
}

void lang_fopen(stack **   s, word_dict **   d)
{
	typecheck(s, STACK);
	l_val *new_val = pop_stack(s);

	stack *st = new_val->s_val;
	// typecheck(s,"fopen", st->data[0], WORD);
	// typecheck(s,"fopen", st->data[0], WORD);
	char *filename = st->data[0]->word_val->name;
	char *filemode = st->data[1]->word_val->name;
	printf("name %s , mode %s", filename, filemode);
	FILE *fp = fopen(filename, filemode);
	l_val *val = l_val_file(fp);
	push_stack(s, val);
	free(new_val);
	free_stack(st);
}

void lang_fclose(stack **   s, word_dict **   d)
{
	typecheck(s, STREAM);
	l_val *new_val = pop_stack(s);
	// typecheck(s,"fclose", new_val, STREAM);
	fclose(new_val->fp);
	free(new_val);

}

void lang_using(stack **   s, word_dict **   d)
{
	typecheck(s, STACK);
	l_val *new_val = pop_stack(s);

	stack *st = new_val->s_val;

	char *filename = st->data[0]->word_val->name;
	FILE *fp = fopen(filename, "r");
	stack *dummy = parse_file(fp);
	link_stacks(s, dummy);
	fclose(fp);
	free(new_val);
	free_stack(st);
}

void lang_stack_len_s(stack **   s, word_dict **   d)
{
	typecheck(s, STACK);
	l_val *new_val = l_val_int((*s)->data[(*s)->top - 1]->s_val->top);
	push_stack(s, new_val);
}

void lang_stack_len_v(stack **   s, word_dict **   d)
{
	typecheck(s, VEC);
	l_val *new_val = l_val_int((*s)->data[(*s)->top - 1]->vec_val->top);
	push_stack(s, new_val);
}

void lang_stack_len(stack **   s, word_dict **   d)
{
	l_val *new_val = l_val_int((*s)->top);
	push_stack(s, new_val);
}

#define func_adder( fun_name, funadress) do{                            \
    char s_val[] = (fun_name);                                          \
    unsigned long int n = hash(s_val);                                                \
    printf("added %s to dict hash is %ld adjusted to len %ld\n",s_val,n,n%(*d)->len ); \
    word_val* val = prim_func(strdup(s_val), &(funadress));             \
    add_dict(d, n, val);                                                \
  }while(0);                                                            \


// generated funcs

inline bool i_less(long int arg1, long int arg2)
{
	return arg1 < arg2;
}

void lang_i_less(stack **   s, word_dict **   d)
{
	typecheck(s, INT, INT);
	l_val *var0 = pop_stack(s);
	long int ret2 = var0->i_val;
	l_val *var1 = pop_stack(s);
	long int ret1 = var1->i_val;
	bool val = i_less(ret1, ret2);
	push_stack(s, l_val_bool(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vi_less(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, BOOL);
	for (long int i = 0; i < arg1->top; i++) {
		ret->i_vec[i] = arg1->i_vec[i] < arg2->i_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vi_less(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vi_less(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}
void lang_v_get(stack **   s, word_dict **   d)
{
  typecheck(s, INT, VEC);
  l_val *var0 = pop_stack(s);
  int n = var0->i_val;
  l_val *var1 = pop_stack(s);
  vector* vec = var1->vec_val;
  l_val* new_val = vector_get(vec,n);
  push_stack(s, var1);
  push_stack(s, new_val);
  free(var0);
}
void lang_v_set(stack **   s, word_dict **   d)
{
  typecheck(s,VEC,INT);
  types val_T = (*s)->data[(*s)->top-3]->T;
  types vec_T = (*s)->data[(*s)->top-1]->vec_val->T;
  long int idx = (*s)->data[(*s)->top-2]->i_val;
  
  long int n = (*s)->data[(*s)->top-3]->vec_val->top;
  if(n>=idx && val_T==vec_T){
  l_val *var0 = pop_stack(s);
  
  l_val *var1 = pop_stack(s);
  vector* vec = var1->vec_val;
  l_val* new_val = vector_get(vec,n);
  push_stack(s, var1);
  push_stack(s, new_val);
  free(var0);}
  else{
    
  }
}

inline bool d_less(double arg1, double arg2)
{
	return arg1 < arg2;
}

void lang_d_less(stack **   s, word_dict **   d)
{
	typecheck(s, DOUBLE, DOUBLE);
	l_val *var0 = pop_stack(s);
	double ret2 = var0->d_val;
	l_val *var1 = pop_stack(s);
	double ret1 = var1->d_val;
	bool val = d_less(ret1, ret2);
	push_stack(s, l_val_bool(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vd_less(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, BOOL);
	for (long int i = 0; i < arg1->top; i++) {
		ret->d_vec[i] = arg1->d_vec[i] < arg2->d_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vd_less(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vd_less(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

inline bool c_less(char arg1, char arg2)
{
	return arg1 < arg2;
}

void lang_c_less(stack **   s, word_dict **   d)
{
	typecheck(s, CHAR, CHAR);
	l_val *var0 = pop_stack(s);
	char ret2 = var0->c_val;
	l_val *var1 = pop_stack(s);
	char ret1 = var1->c_val;
	bool val = c_less(ret1, ret2);
	push_stack(s, l_val_bool(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vc_less(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, BOOL);
	for (long int i = 0; i < arg1->top; i++) {
		ret->c_vec[i] = arg1->c_vec[i] < arg2->c_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vc_less(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vc_less(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

inline bool b_less(bool arg1, bool arg2)
{
	return arg1 < arg2;
}

void lang_b_less(stack **   s, word_dict **   d)
{
	typecheck(s, BOOL, BOOL);
	l_val *var0 = pop_stack(s);
	bool ret2 = var0->b_val;
	l_val *var1 = pop_stack(s);
	bool ret1 = var1->b_val;
	bool val = b_less(ret1, ret2);
	push_stack(s, l_val_bool(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vb_less(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, BOOL);
	for (long int i = 0; i < arg1->top; i++) {
		ret->b_vec[i] = arg1->b_vec[i] < arg2->b_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vb_less(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vb_less(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

// @@ less < INT BOOL CHAR DOUBLE
inline bool i_great(long int arg1, long int arg2)
{
	return arg1 > arg2;
}

void lang_i_great(stack **   s, word_dict **   d)
{
	typecheck(s, INT, INT);
	l_val *var0 = pop_stack(s);
	long int ret2 = var0->i_val;
	l_val *var1 = pop_stack(s);
	long int ret1 = var1->i_val;
	bool val = i_great(ret1, ret2);
	push_stack(s, l_val_bool(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vi_great(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, BOOL);
	for (long int i = 0; i < arg1->top; i++) {
		ret->i_vec[i] = arg1->i_vec[i] > arg2->i_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vi_great(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vi_great(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

inline bool d_great(double arg1, double arg2)
{
	return arg1 > arg2;
}

void lang_d_great(stack **   s, word_dict **   d)
{
	typecheck(s, DOUBLE, DOUBLE);
	l_val *var0 = pop_stack(s);
	double ret2 = var0->d_val;
	l_val *var1 = pop_stack(s);
	double ret1 = var1->d_val;
	bool val = d_great(ret1, ret2);
	push_stack(s, l_val_bool(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vd_great(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, BOOL);
	for (long int i = 0; i < arg1->top; i++) {
		ret->d_vec[i] = arg1->d_vec[i] > arg2->d_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vd_great(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vd_great(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

inline bool c_great(char arg1, char arg2)
{
	return arg1 > arg2;
}

void lang_c_great(stack **   s, word_dict **   d)
{
	typecheck(s, CHAR, CHAR);
	l_val *var0 = pop_stack(s);
	char ret2 = var0->c_val;
	l_val *var1 = pop_stack(s);
	char ret1 = var1->c_val;
	bool val = c_great(ret1, ret2);
	push_stack(s, l_val_bool(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vc_great(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, BOOL);
	for (long int i = 0; i < arg1->top; i++) {
		ret->c_vec[i] = arg1->c_vec[i] > arg2->c_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vc_great(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vc_great(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

inline bool b_great(bool arg1, bool arg2)
{
	return arg1 > arg2;
}

void lang_b_great(stack **   s, word_dict **   d)
{
	typecheck(s, BOOL, BOOL);
	l_val *var0 = pop_stack(s);
	bool ret2 = var0->b_val;
	l_val *var1 = pop_stack(s);
	bool ret1 = var1->b_val;
	bool val = b_great(ret1, ret2);
	push_stack(s, l_val_bool(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vb_great(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, BOOL);
	for (long int i = 0; i < arg1->top; i++) {
		ret->b_vec[i] = arg1->b_vec[i] > arg2->b_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vb_great(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vb_great(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

// @@ great > INT BOOL CHAR DOUBLE
inline bool i_leq(long int arg1, long int arg2)
{
	return arg1 <= arg2;
}

void lang_i_leq(stack **   s, word_dict **   d)
{
	typecheck(s, INT, INT);
	l_val *var0 = pop_stack(s);
	long int ret2 = var0->i_val;
	l_val *var1 = pop_stack(s);
	long int ret1 = var1->i_val;
	bool val = i_leq(ret1, ret2);
	push_stack(s, l_val_bool(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vi_leq(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, BOOL);
	for (long int i = 0; i < arg1->top; i++) {
		ret->i_vec[i] = arg1->i_vec[i] <= arg2->i_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vi_leq(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vi_leq(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

inline bool d_leq(double arg1, double arg2)
{
	return arg1 <= arg2;
}

void lang_d_leq(stack **   s, word_dict **   d)
{
	typecheck(s, DOUBLE, DOUBLE);
	l_val *var0 = pop_stack(s);
	double ret2 = var0->d_val;
	l_val *var1 = pop_stack(s);
	double ret1 = var1->d_val;
	bool val = d_leq(ret1, ret2);
	push_stack(s, l_val_bool(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vd_leq(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, BOOL);
	for (long int i = 0; i < arg1->top; i++) {
		ret->d_vec[i] = arg1->d_vec[i] <= arg2->d_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vd_leq(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vd_leq(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

inline bool c_leq(char arg1, char arg2)
{
	return arg1 <= arg2;
}

void lang_c_leq(stack **   s, word_dict **   d)
{
	typecheck(s, CHAR, CHAR);
	l_val *var0 = pop_stack(s);
	char ret2 = var0->c_val;
	l_val *var1 = pop_stack(s);
	char ret1 = var1->c_val;
	bool val = c_leq(ret1, ret2);
	push_stack(s, l_val_bool(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vc_leq(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, BOOL);
	for (long int i = 0; i < arg1->top; i++) {
		ret->c_vec[i] = arg1->c_vec[i] <= arg2->c_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vc_leq(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vc_leq(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

inline bool b_leq(bool arg1, bool arg2)
{
	return arg1 <= arg2;
}

void lang_b_leq(stack **   s, word_dict **   d)
{
	typecheck(s, BOOL, BOOL);
	l_val *var0 = pop_stack(s);
	bool ret2 = var0->b_val;
	l_val *var1 = pop_stack(s);
	bool ret1 = var1->b_val;
	bool val = b_leq(ret1, ret2);
	push_stack(s, l_val_bool(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vb_leq(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, BOOL);
	for (long int i = 0; i < arg1->top; i++) {
		ret->b_vec[i] = arg1->b_vec[i] <= arg2->b_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vb_leq(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vb_leq(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

// @@ leq <= INT BOOL CHAR DOUBLE
inline bool i_geq(long int arg1, long int arg2)
{
	return arg1 >= arg2;
}

void lang_i_geq(stack **   s, word_dict **   d)
{
	typecheck(s, INT, INT);
	l_val *var0 = pop_stack(s);
	long int ret2 = var0->i_val;
	l_val *var1 = pop_stack(s);
	long int ret1 = var1->i_val;
	bool val = i_geq(ret1, ret2);
	push_stack(s, l_val_bool(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vi_geq(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, BOOL);
	for (long int i = 0; i < arg1->top; i++) {
		ret->i_vec[i] = arg1->i_vec[i] >= arg2->i_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vi_geq(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vi_geq(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

inline bool d_geq(double arg1, double arg2)
{
	return arg1 >= arg2;
}

void lang_d_geq(stack **   s, word_dict **   d)
{
	typecheck(s, DOUBLE, DOUBLE);
	l_val *var0 = pop_stack(s);
	double ret2 = var0->d_val;
	l_val *var1 = pop_stack(s);
	double ret1 = var1->d_val;
	bool val = d_geq(ret1, ret2);
	push_stack(s, l_val_bool(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vd_geq(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, BOOL);
	for (long int i = 0; i < arg1->top; i++) {
		ret->d_vec[i] = arg1->d_vec[i] >= arg2->d_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vd_geq(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vd_geq(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

inline bool c_geq(char arg1, char arg2)
{
	return arg1 >= arg2;
}

void lang_c_geq(stack **   s, word_dict **   d)
{
	typecheck(s, CHAR, CHAR);
	l_val *var0 = pop_stack(s);
	char ret2 = var0->c_val;
	l_val *var1 = pop_stack(s);
	char ret1 = var1->c_val;
	bool val = c_geq(ret1, ret2);
	push_stack(s, l_val_bool(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vc_geq(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, BOOL);
	for (long int i = 0; i < arg1->top; i++) {
		ret->c_vec[i] = arg1->c_vec[i] >= arg2->c_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vc_geq(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vc_geq(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

inline bool b_geq(bool arg1, bool arg2)
{
	return arg1 >= arg2;
}

void lang_b_geq(stack **   s, word_dict **   d)
{
	typecheck(s, BOOL, BOOL);
	l_val *var0 = pop_stack(s);
	bool ret2 = var0->b_val;
	l_val *var1 = pop_stack(s);
	bool ret1 = var1->b_val;
	bool val = b_geq(ret1, ret2);
	push_stack(s, l_val_bool(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vb_geq(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, BOOL);
	for (long int i = 0; i < arg1->top; i++) {
		ret->b_vec[i] = arg1->b_vec[i] >= arg2->b_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vb_geq(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vb_geq(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

// @@ geq >= INT BOOL CHAR DOUBLE
inline bool i_eq(long int arg1, long int arg2)
{
	return arg1 == arg2;
}

void lang_i_eq(stack **   s, word_dict **   d)
{
	typecheck(s, INT, INT);
	l_val *var0 = pop_stack(s);
	long int ret2 = var0->i_val;
	l_val *var1 = pop_stack(s);
	long int ret1 = var1->i_val;
	bool val = i_eq(ret1, ret2);
	push_stack(s, l_val_bool(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vi_eq(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, BOOL);
	for (long int i = 0; i < arg1->top; i++) {
		ret->i_vec[i] = arg1->i_vec[i] == arg2->i_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vi_eq(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vi_eq(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

inline bool d_eq(double arg1, double arg2)
{
	return arg1 == arg2;
}

void lang_d_eq(stack **   s, word_dict **   d)
{
	typecheck(s, DOUBLE, DOUBLE);
	l_val *var0 = pop_stack(s);
	double ret2 = var0->d_val;
	l_val *var1 = pop_stack(s);
	double ret1 = var1->d_val;
	bool val = d_eq(ret1, ret2);
	push_stack(s, l_val_bool(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vd_eq(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, BOOL);
	for (long int i = 0; i < arg1->top; i++) {
		ret->d_vec[i] = arg1->d_vec[i] == arg2->d_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vd_eq(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vd_eq(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

inline bool c_eq(char arg1, char arg2)
{
	return arg1 == arg2;
}

void lang_c_eq(stack **   s, word_dict **   d)
{
	typecheck(s, CHAR, CHAR);
	l_val *var0 = pop_stack(s);
	char ret2 = var0->c_val;
	l_val *var1 = pop_stack(s);
	char ret1 = var1->c_val;
	bool val = c_eq(ret1, ret2);
	push_stack(s, l_val_bool(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vc_eq(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, BOOL);
	for (long int i = 0; i < arg1->top; i++) {
		ret->c_vec[i] = arg1->c_vec[i] == arg2->c_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vc_eq(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vc_eq(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

inline bool b_eq(bool arg1, bool arg2)
{
	return arg1 == arg2;
}

void lang_b_eq(stack **   s, word_dict **   d)
{
	typecheck(s, BOOL, BOOL);
	l_val *var0 = pop_stack(s);
	bool ret2 = var0->b_val;
	l_val *var1 = pop_stack(s);
	bool ret1 = var1->b_val;
	bool val = b_eq(ret1, ret2);
	push_stack(s, l_val_bool(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vb_eq(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, BOOL);
	for (long int i = 0; i < arg1->top; i++) {
		ret->b_vec[i] = arg1->b_vec[i] == arg2->b_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vb_eq(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vb_eq(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

// @@ eq == INT BOOL CHAR DOUBLE

inline long int i_add(long int arg1, long int arg2)
{
	return arg1 + arg2;
}

void lang_i_add(stack **   s, word_dict **   d)
{
	typecheck(s, INT, INT);
	l_val *var0 = pop_stack(s);
	long int ret2 = var0->i_val;
	l_val *var1 = pop_stack(s);
	long int ret1 = var1->i_val;
	long int val = i_add(ret1, ret2);
	push_stack(s, l_val_int(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vi_add(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, INT);
	for (long int i = 0; i < arg1->top; i++) {
		ret->i_vec[i] = arg1->i_vec[i] + arg2->i_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vi_add(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vi_add(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

inline double d_add(double arg1, double arg2)
{
	return arg1 + arg2;
}

void lang_d_add(stack **   s, word_dict **   d)
{
	typecheck(s, DOUBLE, DOUBLE);
	l_val *var0 = pop_stack(s);
	double ret2 = var0->d_val;
	l_val *var1 = pop_stack(s);
	double ret1 = var1->d_val;
	double val = d_add(ret1, ret2);
	push_stack(s, l_val_double(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vd_add(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, DOUBLE);
	for (long int i = 0; i < arg1->top; i++) {
		ret->d_vec[i] = arg1->d_vec[i] + arg2->d_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vd_add(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vd_add(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

inline char c_add(char arg1, char arg2)
{
	return arg1 + arg2;
}

void lang_c_add(stack **   s, word_dict **   d)
{
	typecheck(s, CHAR, CHAR);
	l_val *var0 = pop_stack(s);
	char ret2 = var0->c_val;
	l_val *var1 = pop_stack(s);
	char ret1 = var1->c_val;
	char val = c_add(ret1, ret2);
	push_stack(s, l_val_char(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vc_add(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, CHAR);
	for (long int i = 0; i < arg1->top; i++) {
		ret->c_vec[i] = arg1->c_vec[i] + arg2->c_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vc_add(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vc_add(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

inline bool b_add(bool arg1, bool arg2)
{
	return arg1 + arg2;
}

void lang_b_add(stack **   s, word_dict **   d)
{
	typecheck(s, BOOL, BOOL);
	l_val *var0 = pop_stack(s);
	bool ret2 = var0->b_val;
	l_val *var1 = pop_stack(s);
	bool ret1 = var1->b_val;
	bool val = b_add(ret1, ret2);
	push_stack(s, l_val_bool(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vb_add(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, BOOL);
	for (long int i = 0; i < arg1->top; i++) {
		ret->b_vec[i] = arg1->b_vec[i] + arg2->b_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vb_add(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vb_add(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

// @@ add + INT BOOL CHAR DOUBLE
inline long int i_sub(long int arg1, long int arg2)
{
	return arg1 - arg2;
}

void lang_i_sub(stack **   s, word_dict **   d)
{
	typecheck(s, INT, INT);
	l_val *var0 = pop_stack(s);
	long int ret2 = var0->i_val;
	l_val *var1 = pop_stack(s);
	long int ret1 = var1->i_val;
	long int val = i_sub(ret1, ret2);
	push_stack(s, l_val_int(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vi_sub(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, INT);
	for (long int i = 0; i < arg1->top; i++) {
		ret->i_vec[i] = arg1->i_vec[i] - arg2->i_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vi_sub(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vi_sub(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

inline double d_sub(double arg1, double arg2)
{
	return arg1 - arg2;
}

void lang_d_sub(stack **   s, word_dict **   d)
{
	typecheck(s, DOUBLE, DOUBLE);
	l_val *var0 = pop_stack(s);
	double ret2 = var0->d_val;
	l_val *var1 = pop_stack(s);
	double ret1 = var1->d_val;
	double val = d_sub(ret1, ret2);
	push_stack(s, l_val_double(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vd_sub(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, DOUBLE);
	for (long int i = 0; i < arg1->top; i++) {
		ret->d_vec[i] = arg1->d_vec[i] - arg2->d_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vd_sub(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vd_sub(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

inline char c_sub(char arg1, char arg2)
{
	return arg1 - arg2;
}

void lang_c_sub(stack **   s, word_dict **   d)
{
	typecheck(s, CHAR, CHAR);
	l_val *var0 = pop_stack(s);
	char ret2 = var0->c_val;
	l_val *var1 = pop_stack(s);
	char ret1 = var1->c_val;
	char val = c_sub(ret1, ret2);
	push_stack(s, l_val_char(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vc_sub(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, CHAR);
	for (long int i = 0; i < arg1->top; i++) {
		ret->c_vec[i] = arg1->c_vec[i] - arg2->c_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vc_sub(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vc_sub(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

inline bool b_sub(bool arg1, bool arg2)
{
	return arg1 - arg2;
}

void lang_b_sub(stack **   s, word_dict **   d)
{
	typecheck(s, BOOL, BOOL);
	l_val *var0 = pop_stack(s);
	bool ret2 = var0->b_val;
	l_val *var1 = pop_stack(s);
	bool ret1 = var1->b_val;
	bool val = b_sub(ret1, ret2);
	push_stack(s, l_val_bool(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vb_sub(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, BOOL);
	for (long int i = 0; i < arg1->top; i++) {
		ret->b_vec[i] = arg1->b_vec[i] - arg2->b_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vb_sub(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vb_sub(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

// @@ sub - INT BOOL CHAR DOUBLE
inline long int i_mul(long int arg1, long int arg2)
{
	return arg1 * arg2;
}

void lang_i_mul(stack **   s, word_dict **   d)
{
	typecheck(s, INT, INT);
	l_val *var0 = pop_stack(s);
	long int ret2 = var0->i_val;
	l_val *var1 = pop_stack(s);
	long int ret1 = var1->i_val;
	long int val = i_mul(ret1, ret2);
	push_stack(s, l_val_int(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vi_mul(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, INT);
	for (long int i = 0; i < arg1->top; i++) {
		ret->i_vec[i] = arg1->i_vec[i] * arg2->i_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vi_mul(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vi_mul(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

inline double d_mul(double arg1, double arg2)
{
	return arg1 * arg2;
}

void lang_d_mul(stack **   s, word_dict **   d)
{
	typecheck(s, DOUBLE, DOUBLE);
	l_val *var0 = pop_stack(s);
	double ret2 = var0->d_val;
	l_val *var1 = pop_stack(s);
	double ret1 = var1->d_val;
	double val = d_mul(ret1, ret2);
	push_stack(s, l_val_double(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vd_mul(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, DOUBLE);
	for (long int i = 0; i < arg1->top; i++) {
		ret->d_vec[i] = arg1->d_vec[i] * arg2->d_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vd_mul(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vd_mul(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

inline char c_mul(char arg1, char arg2)
{
	return arg1 * arg2;
}

void lang_c_mul(stack **   s, word_dict **   d)
{
	typecheck(s, CHAR, CHAR);
	l_val *var0 = pop_stack(s);
	char ret2 = var0->c_val;
	l_val *var1 = pop_stack(s);
	char ret1 = var1->c_val;
	char val = c_mul(ret1, ret2);
	push_stack(s, l_val_char(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vc_mul(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, CHAR);
	for (long int i = 0; i < arg1->top; i++) {
		ret->c_vec[i] = arg1->c_vec[i] * arg2->c_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vc_mul(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vc_mul(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

inline bool b_mul(bool arg1, bool arg2)
{
	return arg1 * arg2;
}

void lang_b_mul(stack **   s, word_dict **   d)
{
	typecheck(s, BOOL, BOOL);
	l_val *var0 = pop_stack(s);
	bool ret2 = var0->b_val;
	l_val *var1 = pop_stack(s);
	bool ret1 = var1->b_val;
	bool val = b_mul(ret1, ret2);
	push_stack(s, l_val_bool(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vb_mul(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, BOOL);
	for (long int i = 0; i < arg1->top; i++) {
		ret->b_vec[i] = arg1->b_vec[i] * arg2->b_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vb_mul(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vb_mul(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

// @@ mul * INT BOOL CHAR DOUBLE
inline long int i_div(long int arg1, long int arg2)
{
	return arg1 / arg2;
}

void lang_i_div(stack **   s, word_dict **   d)
{
	typecheck(s, INT, INT);
	l_val *var0 = pop_stack(s);
	long int ret2 = var0->i_val;
	l_val *var1 = pop_stack(s);
	long int ret1 = var1->i_val;
	long int val = i_div(ret1, ret2);
	push_stack(s, l_val_int(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vi_div(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, INT);
	for (long int i = 0; i < arg1->top; i++) {
		ret->i_vec[i] = arg1->i_vec[i] / arg2->i_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vi_div(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vi_div(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

inline double d_div(double arg1, double arg2)
{
	return arg1 / arg2;
}

void lang_d_div(stack **   s, word_dict **   d)
{
	typecheck(s, DOUBLE, DOUBLE);
	l_val *var0 = pop_stack(s);
	double ret2 = var0->d_val;
	l_val *var1 = pop_stack(s);
	double ret1 = var1->d_val;
	double val = d_div(ret1, ret2);
	push_stack(s, l_val_double(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vd_div(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, DOUBLE);
	for (long int i = 0; i < arg1->top; i++) {
		ret->d_vec[i] = arg1->d_vec[i] / arg2->d_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vd_div(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vd_div(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

// @@ div / INT DOUBLE
inline long int i_mod(long int arg1, long int arg2)
{
	return arg1 % arg2;
}

void lang_i_mod(stack **   s, word_dict **   d)
{
	typecheck(s, INT, INT);
	l_val *var0 = pop_stack(s);
	long int ret2 = var0->i_val;
	l_val *var1 = pop_stack(s);
	long int ret1 = var1->i_val;
	long int val = i_mod(ret1, ret2);
	push_stack(s, l_val_int(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vi_mod(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, INT);
	for (long int i = 0; i < arg1->top; i++) {
		ret->i_vec[i] = arg1->i_vec[i] % arg2->i_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vi_mod(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vi_mod(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

// @@ mod % INT
inline long int i_and(long int arg1, long int arg2)
{
	return arg1 && arg2;
}

void lang_i_and(stack **   s, word_dict **   d)
{
	typecheck(s, INT, INT);
	l_val *var0 = pop_stack(s);
	long int ret2 = var0->i_val;
	l_val *var1 = pop_stack(s);
	long int ret1 = var1->i_val;
	long int val = i_and(ret1, ret2);
	push_stack(s, l_val_int(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vi_and(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, INT);
	for (long int i = 0; i < arg1->top; i++) {
		ret->i_vec[i] = arg1->i_vec[i] && arg2->i_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vi_and(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vi_and(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

inline char c_and(char arg1, char arg2)
{
	return arg1 && arg2;
}

void lang_c_and(stack **   s, word_dict **   d)
{
	typecheck(s, CHAR, CHAR);
	l_val *var0 = pop_stack(s);
	char ret2 = var0->c_val;
	l_val *var1 = pop_stack(s);
	char ret1 = var1->c_val;
	char val = c_and(ret1, ret2);
	push_stack(s, l_val_char(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vc_and(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, CHAR);
	for (long int i = 0; i < arg1->top; i++) {
		ret->c_vec[i] = arg1->c_vec[i] && arg2->c_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vc_and(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vc_and(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

inline bool b_and(bool arg1, bool arg2)
{
	return arg1 && arg2;
}

void lang_b_and(stack **   s, word_dict **   d)
{
	typecheck(s, BOOL, BOOL);
	l_val *var0 = pop_stack(s);
	bool ret2 = var0->b_val;
	l_val *var1 = pop_stack(s);
	bool ret1 = var1->b_val;
	bool val = b_and(ret1, ret2);
	push_stack(s, l_val_bool(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vb_and(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, BOOL);
	for (long int i = 0; i < arg1->top; i++) {
		ret->b_vec[i] = arg1->b_vec[i] && arg2->b_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vb_and(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vb_and(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

// @@ and && BOOL INT CHAR
inline long int i_or(long int arg1, long int arg2)
{
	return arg1 || arg2;
}

void lang_i_or(stack **   s, word_dict **   d)
{
	typecheck(s, INT, INT);
	l_val *var0 = pop_stack(s);
	long int ret2 = var0->i_val;
	l_val *var1 = pop_stack(s);
	long int ret1 = var1->i_val;
	long int val = i_or(ret1, ret2);
	push_stack(s, l_val_int(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vi_or(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, INT);
	for (long int i = 0; i < arg1->top; i++) {
		ret->i_vec[i] = arg1->i_vec[i] || arg2->i_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vi_or(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vi_or(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

inline char c_or(char arg1, char arg2)
{
	return arg1 || arg2;
}

void lang_c_or(stack **   s, word_dict **   d)
{
	typecheck(s, CHAR, CHAR);
	l_val *var0 = pop_stack(s);
	char ret2 = var0->c_val;
	l_val *var1 = pop_stack(s);
	char ret1 = var1->c_val;
	char val = c_or(ret1, ret2);
	push_stack(s, l_val_char(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vc_or(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, CHAR);
	for (long int i = 0; i < arg1->top; i++) {
		ret->c_vec[i] = arg1->c_vec[i] || arg2->c_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vc_or(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vc_or(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

inline bool b_or(bool arg1, bool arg2)
{
	return arg1 || arg2;
}

void lang_b_or(stack **   s, word_dict **   d)
{
	typecheck(s, BOOL, BOOL);
	l_val *var0 = pop_stack(s);
	bool ret2 = var0->b_val;
	l_val *var1 = pop_stack(s);
	bool ret1 = var1->b_val;
	bool val = b_or(ret1, ret2);
	push_stack(s, l_val_bool(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vb_or(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, BOOL);
	for (long int i = 0; i < arg1->top; i++) {
		ret->b_vec[i] = arg1->b_vec[i] || arg2->b_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vb_or(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vb_or(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

// @@ or || INT BOOL CHAR
inline long int i_bin_shift_l(long int arg1, long int arg2)
{
	return arg1 << arg2;
}

void lang_i_bin_shift_l(stack **   s, word_dict **   d)
{
	typecheck(s, INT, INT);
	l_val *var0 = pop_stack(s);
	long int ret2 = var0->i_val;
	l_val *var1 = pop_stack(s);
	long int ret1 = var1->i_val;
	long int val = i_bin_shift_l(ret1, ret2);
	push_stack(s, l_val_int(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vi_bin_shift_l(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, INT);
	for (long int i = 0; i < arg1->top; i++) {
		ret->i_vec[i] = arg1->i_vec[i] << arg2->i_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vi_bin_shift_l(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vi_bin_shift_l(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

inline char c_bin_shift_l(char arg1, char arg2)
{
	return arg1 << arg2;
}

void lang_c_bin_shift_l(stack **   s, word_dict **   d)
{
	typecheck(s, CHAR, CHAR);
	l_val *var0 = pop_stack(s);
	char ret2 = var0->c_val;
	l_val *var1 = pop_stack(s);
	char ret1 = var1->c_val;
	char val = c_bin_shift_l(ret1, ret2);
	push_stack(s, l_val_char(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vc_bin_shift_l(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, CHAR);
	for (long int i = 0; i < arg1->top; i++) {
		ret->c_vec[i] = arg1->c_vec[i] << arg2->c_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vc_bin_shift_l(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vc_bin_shift_l(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

// @@ bin_shift_l << CHAR INT
inline long int i_bin_shift_r(long int arg1, long int arg2)
{
	return arg1 >> arg2;
}

void lang_i_bin_shift_r(stack **   s, word_dict **   d)
{
	typecheck(s, INT, INT);
	l_val *var0 = pop_stack(s);
	long int ret2 = var0->i_val;
	l_val *var1 = pop_stack(s);
	long int ret1 = var1->i_val;
	long int val = i_bin_shift_r(ret1, ret2);
	push_stack(s, l_val_int(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vi_bin_shift_r(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, INT);
	for (long int i = 0; i < arg1->top; i++) {
		ret->i_vec[i] = arg1->i_vec[i] >> arg2->i_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vi_bin_shift_r(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vi_bin_shift_r(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

inline char c_bin_shift_r(char arg1, char arg2)
{
	return arg1 >> arg2;
}

void lang_c_bin_shift_r(stack **   s, word_dict **   d)
{
	typecheck(s, CHAR, CHAR);
	l_val *var0 = pop_stack(s);
	char ret2 = var0->c_val;
	l_val *var1 = pop_stack(s);
	char ret1 = var1->c_val;
	char val = c_bin_shift_r(ret1, ret2);
	push_stack(s, l_val_char(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vc_bin_shift_r(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, CHAR);
	for (long int i = 0; i < arg1->top; i++) {
		ret->c_vec[i] = arg1->c_vec[i] >> arg2->c_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vc_bin_shift_r(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vc_bin_shift_r(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

// @@ bin_shift_r >> CHAR INT
inline long int i_bin_or(long int arg1, long int arg2)
{
	return arg1 | arg2;
}

void lang_i_bin_or(stack **   s, word_dict **   d)
{
	typecheck(s, INT, INT);
	l_val *var0 = pop_stack(s);
	long int ret2 = var0->i_val;
	l_val *var1 = pop_stack(s);
	long int ret1 = var1->i_val;
	long int val = i_bin_or(ret1, ret2);
	push_stack(s, l_val_int(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vi_bin_or(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, INT);
	for (long int i = 0; i < arg1->top; i++) {
		ret->i_vec[i] = arg1->i_vec[i] | arg2->i_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vi_bin_or(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vi_bin_or(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

inline char c_bin_or(char arg1, char arg2)
{
	return arg1 | arg2;
}

void lang_c_bin_or(stack **   s, word_dict **   d)
{
	typecheck(s, CHAR, CHAR);
	l_val *var0 = pop_stack(s);
	char ret2 = var0->c_val;
	l_val *var1 = pop_stack(s);
	char ret1 = var1->c_val;
	char val = c_bin_or(ret1, ret2);
	push_stack(s, l_val_char(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vc_bin_or(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, CHAR);
	for (long int i = 0; i < arg1->top; i++) {
		ret->c_vec[i] = arg1->c_vec[i] | arg2->c_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vc_bin_or(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vc_bin_or(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

// @@ bin_or | CHAR INT
inline long int i_bin_and(long int arg1, long int arg2)
{
	return arg1 & arg2;
}

void lang_i_bin_and(stack **   s, word_dict **   d)
{
	typecheck(s, INT, INT);
	l_val *var0 = pop_stack(s);
	long int ret2 = var0->i_val;
	l_val *var1 = pop_stack(s);
	long int ret1 = var1->i_val;
	long int val = i_bin_and(ret1, ret2);
	push_stack(s, l_val_int(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vi_bin_and(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, INT);
	for (long int i = 0; i < arg1->top; i++) {
		ret->i_vec[i] = arg1->i_vec[i] & arg2->i_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vi_bin_and(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vi_bin_and(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

inline char c_bin_and(char arg1, char arg2)
{
	return arg1 & arg2;
}

void lang_c_bin_and(stack **   s, word_dict **   d)
{
	typecheck(s, CHAR, CHAR);
	l_val *var0 = pop_stack(s);
	char ret2 = var0->c_val;
	l_val *var1 = pop_stack(s);
	char ret1 = var1->c_val;
	char val = c_bin_and(ret1, ret2);
	push_stack(s, l_val_char(val));
	free_l_val(var0);
	free_l_val(var1);
}

vector *vc_bin_and(vector *   arg1, vector *   arg2)
{
	// if(check_vec_type( arg1 , arg2 )==0){
	// return init_vec(arg1->top,ERROR);} 
	vector *ret = init_vec(arg1->top, CHAR);
	for (long int i = 0; i < arg1->top; i++) {
		ret->c_vec[i] = arg1->c_vec[i] & arg2->c_vec[i];
	}
	ret->top = arg1->top;
	return ret;
}

void lang_vc_bin_and(stack **   s, word_dict **   d)
{
	typecheck(s, VEC, VEC);
	l_val *var0 = pop_stack(s);
	vector *ret2 = var0->vec_val;
	l_val *var1 = pop_stack(s);
	vector *ret1 = var1->vec_val;
	vector *val = vc_bin_and(ret1, ret2);
	push_stack(s, l_val_vec(val));
	free_vec(ret1);
	free_vec(ret2);
	free(var0);
	free(var1);
}

// @@ bin_and & CHAR INT

void fillDict(word_dict **   d)
{

	func_adder("isInt", isInt);
	func_adder("isBool", isBool);
	func_adder("isChar", isChar);
	func_adder("isDouble", isDouble);
	func_adder("isStack", isStack);
	func_adder("isString", isString);
	func_adder("isWord", isWord);
	func_adder("isSymbol", isSymbol);
	func_adder("isVec", isVec);
	func_adder("isFile", isInt);
        
        func_adder("isIntVec", isIntVec);
	func_adder("isBoolVec", isBoolVec);
	func_adder("isCharVec", isCharVec);
	func_adder("isDoubleVec", isDoubleVec);
	func_adder("isStackVec", isStackVec);
	func_adder("isStringVec", isStringVec);
	func_adder("isWordVec", isWordVec);
	func_adder("isSymbolVec", isSymbolVec);
	
	func_adder("i2d", int2double);
	func_adder("d2i", double2int);

	func_adder("def", def);

	func_adder("reshape", reshape);
	func_adder("v_reshape", reshape_v_whole_stack);

	func_adder("s_reshape", reshape_s);
	func_adder("vs_reshape", reshape_vs);

	func_adder("eval", lang_eval);
	func_adder("vs_eval", lang_eval_v);

	func_adder("apply", apply);
	func_adder("vs_apply", apply_v);

	func_adder("link", lang_link);
	func_adder("vs_link", lang_link_v);

	func_adder("s_split", lang_s_split);
	func_adder("vs_split", lang_s_split_v);
	func_adder("v_split", lang_s_split);
	func_adder("vv_split", lang_s_split_v);

	func_adder("s_push", lang_push);

	func_adder("s_pop", lang_pop);
	func_adder("v_push", lang_push_v);
	func_adder("v_pop", lang_pop_v);
        func_adder("v_get", lang_v_get);
	func_adder("v_set", lang_v_set);

	func_adder("s_len", lang_stack_len_s);
	func_adder("v_len", lang_stack_len_v);
	func_adder("stackLen", lang_stack_len);

	func_adder("if", lang_if);
	func_adder("putc", put_char);
	func_adder("getc", get_char);

	func_adder("fclose", lang_fclose);
	func_adder("fopen", lang_fopen);
	func_adder("using", lang_using);
        func_adder("get_def", lang_get_def);


        // generated funcs;

	func_adder("i_less", lang_i_less);
	func_adder("vi_less", lang_vi_less);
	func_adder("d_less", lang_d_less);
	func_adder("vd_less", lang_vd_less);
	func_adder("c_less", lang_c_less);
	func_adder("vc_less", lang_vc_less);
	func_adder("b_less", lang_b_less);
	func_adder("vb_less", lang_vb_less);
	func_adder("i_great", lang_i_great);
	func_adder("vi_great", lang_vi_great);
	func_adder("d_great", lang_d_great);
	func_adder("vd_great", lang_vd_great);
	func_adder("c_great", lang_c_great);
	func_adder("vc_great", lang_vc_great);
	func_adder("b_great", lang_b_great);
	func_adder("vb_great", lang_vb_great);
	func_adder("i_leq", lang_i_leq);
	func_adder("vi_leq", lang_vi_leq);
	func_adder("d_leq", lang_d_leq);
	func_adder("vd_leq", lang_vd_leq);
	func_adder("c_leq", lang_c_leq);
	func_adder("vc_leq", lang_vc_leq);
	func_adder("b_leq", lang_b_leq);
	func_adder("vb_leq", lang_vb_leq);
	func_adder("i_geq", lang_i_geq);
	func_adder("vi_geq", lang_vi_geq);
	func_adder("d_geq", lang_d_geq);
	func_adder("vd_geq", lang_vd_geq);
	func_adder("c_geq", lang_c_geq);
	func_adder("vc_geq", lang_vc_geq);
	func_adder("b_geq", lang_b_geq);
	func_adder("vb_geq", lang_vb_geq);
	func_adder("i_eq", lang_i_eq);
	func_adder("vi_eq", lang_vi_eq);
	func_adder("d_eq", lang_d_eq);
	func_adder("vd_eq", lang_vd_eq);
	func_adder("c_eq", lang_c_eq);
	func_adder("vc_eq", lang_vc_eq);
	func_adder("b_eq", lang_b_eq);
	func_adder("vb_eq", lang_vb_eq);
	func_adder("i_add", lang_i_add);
	func_adder("vi_add", lang_vi_add);
	func_adder("d_add", lang_d_add);
	func_adder("vd_add", lang_vd_add);
	func_adder("c_add", lang_c_add);
	func_adder("vc_add", lang_vc_add);
	func_adder("b_add", lang_b_add);
	func_adder("vb_add", lang_vb_add);
	func_adder("i_sub", lang_i_sub);
	func_adder("vi_sub", lang_vi_sub);
	func_adder("d_sub", lang_d_sub);
	func_adder("vd_sub", lang_vd_sub);
	func_adder("c_sub", lang_c_sub);
	func_adder("vc_sub", lang_vc_sub);
	func_adder("b_sub", lang_b_sub);
	func_adder("vb_sub", lang_vb_sub);
	func_adder("i_mul", lang_i_mul);
	func_adder("vi_mul", lang_vi_mul);
	func_adder("d_mul", lang_d_mul);
	func_adder("vd_mul", lang_vd_mul);
	func_adder("c_mul", lang_c_mul);
	func_adder("vc_mul", lang_vc_mul);
	func_adder("b_mul", lang_b_mul);
	func_adder("vb_mul", lang_vb_mul);
	func_adder("i_div", lang_i_div);
	func_adder("vi_div", lang_vi_div);
	func_adder("d_div", lang_d_div);
	func_adder("vd_div", lang_vd_div);
	func_adder("i_mod", lang_i_mod);
	func_adder("vi_mod", lang_vi_mod);
	func_adder("i_and", lang_i_and);
	func_adder("vi_and", lang_vi_and);
	func_adder("c_and", lang_c_and);
	func_adder("vc_and", lang_vc_and);
	func_adder("b_and", lang_b_and);
	func_adder("vb_and", lang_vb_and);
	func_adder("i_or", lang_i_or);
	func_adder("vi_or", lang_vi_or);
	func_adder("c_or", lang_c_or);
	func_adder("vc_or", lang_vc_or);
	func_adder("b_or", lang_b_or);
	func_adder("vb_or", lang_vb_or);
	func_adder("i_bin_shift_l", lang_i_bin_shift_l);
	func_adder("vi_bin_shift_l", lang_vi_bin_shift_l);
	func_adder("c_bin_shift_l", lang_c_bin_shift_l);
	func_adder("vc_bin_shift_l", lang_vc_bin_shift_l);
	func_adder("i_bin_shift_r", lang_i_bin_shift_r);
	func_adder("vi_bin_shift_r", lang_vi_bin_shift_r);
	func_adder("c_bin_shift_r", lang_c_bin_shift_r);
	func_adder("vc_bin_shift_r", lang_vc_bin_shift_r);
	func_adder("i_bin_or", lang_i_bin_or);
	func_adder("vi_bin_or", lang_vi_bin_or);
	func_adder("c_bin_or", lang_c_bin_or);
	func_adder("vc_bin_or", lang_vc_bin_or);
	func_adder("i_bin_and", lang_i_bin_and);
	func_adder("vi_bin_and", lang_vi_bin_and);
	func_adder("c_bin_and", lang_c_bin_and);
	func_adder("vc_bin_and", lang_vc_bin_and);	// @@@@//
}

long int test()
{
	long int c = 0;
	word_dict *dict = init_dict(1024);
	stack *s0 =
	    parse_str(" [ 1 ] [ father ] [ father 2 father 3 father ] [ baba ]",
		      &c);

	def(&s0, &dict);
	def(&s0, &dict);
	free_stack(s0);
	c = 0;
	s0 = parse_str
	    (" [ [ 1 2 [ 5 4 addInt ] 0 0 addInt ] reshape ] [ deneme ] ", &c);
	def(&s0, &dict);

	print_stack(s0, 0);

	fillDict(&dict);

	print_dict(dict);

	free_stack(s0);
	c = 0;
	stack *s1 = parse_str(" 1 2 3 4 5 6 7 deneme ", &c);
	print_stack(s1, 0);

	stack *s2 = eval(s1, &dict);
	stack *s3 = eval(s2, &dict);
	print_stack(s3, 0);
	free_stack(s3);
	free_dict(dict);
	return 1;
}

#define MILLION  1e6;
int main()
{
	word_dict *dict = init_dict(1024);
	fillDict(&dict);
	char *prog = alloca(sizeof(char) * 10000);
	long int c;
	stack *s1;
	stack *s2 = init_stack(100);
	while (1) {

		// struct mallinfo m;
		c = 0;
		printf("__lyng> ");
		fgets(prog, 10000, stdin);
		// printf("%s",prog);

		if (prog[0] == ':' && prog[1] == 'q') {
			// printf("heyooo");
			break;
		}
		clock_t t_parse_start;
		t_parse_start = clock();

		s1 = parse_str(prog, &c);
		clock_t t_parse_end = clock() - t_parse_start;

		/*
		 * m = mallinfo(); 
		 */
		/*
		 * long int mem_old_arena = m.arena; 
		 */
		/*
		 * long int mem_old_bloks_all = m.ordblks; 
		 */
		/*
		 * long int mem_old_bloks_use = m.uordblks; 
		 */
		/*
		 * long int mem_old_bloks_free = m.fordblks; 
		 */
		// print_stack(s1, 0);
		clock_t t_eval_start;
		t_eval_start = clock();

		link_stacks(&s2, s1);
		s2 = eval(s2, &dict);

		clock_t t_eval_end = clock() - t_eval_start;

		/*
		 * m = mallinfo(); 
		 */
		/*
		 * long int mem_new_arena = m.arena; 
		 */
		/*
		 * long int mem_new_bloks_all = m.ordblks; 
		 */
		/*
		 * long int mem_new_bloks_use = m.uordblks; 
		 */
		/*
		 * long int mem_new_bloks_free = m.fordblks; 
		 */

		// time_spent = end - start
		double time_taken_eval = ((double)t_eval_end) / CLOCKS_PER_SEC;
		double time_taken_parse =
		    ((double)t_parse_end) / CLOCKS_PER_SEC;

		print_stack(s2, 0);

		printf("\n           Time :\n");
		printf("                  eval  : %f s, %ld cycles\n",
		       time_taken_eval, t_eval_end);
		printf("                  parse : %f s, %ld cycles\n",
		       time_taken_parse, t_parse_end);

		/*
		 * printf 
		 */
		/*
		 * ("Arena was %ld , it is now %d . difference is %d\n", 
		 */
		/*
		 * mem_old_arena, mem_new_arena, 
		 */
		/*
		 * mem_old_arena - mem_new_arena); 
		 */

		/*
		 * printf 
		 */
		/*
		 * ("Bloks was %ld , it is now %d . difference is %d\n", 
		 */
		/*
		 * mem_old_bloks_all, mem_new_bloks_all, 
		 */
		/*
		 * mem_old_bloks_all - mem_new_bloks_all); 
		 */
		/*
		 * printf 
		 */
		/*
		 * ("New Bloks was %ld , it is now %d . difference is %d\n", 
		 */
		/*
		 * mem_old_bloks_use, mem_new_bloks_use, 
		 */
		/*
		 * mem_old_bloks_use - mem_new_bloks_use); 
		 */
		/*
		 * printf 
		 */
		/*
		 * ("Freed Bloks was %ld , it is now %d . difference is %dx\n\n", 
		 */
		/*
		 * mem_old_bloks_free, mem_new_bloks_free, 
		 */
		/*
		 * mem_old_bloks_free - mem_new_bloks_free); 
		 */

	}

	free_stack(s2);
	free_dict(dict);
	return 1;
}

long int test_3()
{
	long int c = 0;
	word_dict *dict = init_dict(1024);
	fillDict(&dict);
	stack *s0 = parse_str(" { 1 2 } [ 0 0 ] reshape ", &c);
	print_stack(s0, 0);
	stack *s1 = eval(s0, &dict);
	print_stack(s1, 0);
	free_stack(s1);
	free_dict(dict);
	return 0;
}
