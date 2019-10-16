#include "data.h"

vector *init_vec(long int len, types T)
{
	vector *ret = calloc(1, sizeof(vector));
	if (ret == NULL) {
		printf("\n Mem run out (init_vec), exiting");
		exit(1);
	} else {
		switch (T) {
		case STREAM:
			{
				ret->fp_vec = calloc(len, sizeof(FILE *));
				if (ret->fp_vec == NULL) {
					free(ret);
					printf
					    ("\n Mem run out (init_stack), exiting");
					exit(1);
				}
				break;
			}

		case SYMBOL:
			{
				ret->sym_vec = calloc(len, sizeof(l_val *));
				if (ret->word_vec == NULL) {
					free(ret);
					printf
					    ("\n Mem run out (init_stack), exiting");
					exit(1);
				}
				break;
			}
		case VEC:
			{
				ret->vec_vec = calloc(len, sizeof(vector *));
				if (ret->vec_vec == NULL) {
					free(ret);
					printf
					    ("\n Mem run out (init_stack), exiting");
					exit(1);
				}
				break;
			}
		case WORD:
			{
				ret->word_vec = calloc(len, sizeof(word_val *));
				if (ret->word_vec == NULL) {
					free(ret);
					printf
					    ("\n Mem run out (init_stack), exiting");
					exit(1);
				}
				break;
			}
		case STR:
			{
				ret->str_vec = calloc(len, sizeof(char *));
				if (ret->str_vec == NULL) {
					free(ret);
					printf
					    ("\n Mem run out (init_stack), exiting");
					exit(1);
				}
				break;
			}
		case STACK:
			{
				ret->s_vec = calloc(len, sizeof(stack *));
				if (ret->s_vec == NULL) {
					free(ret);
					printf
					    ("\n Mem run out (init_stack), exiting");
					exit(1);
				}
				break;
			}

		case ERROR:
			{
				ret->err_code = calloc(len, sizeof(int));
				if (ret->err_code == NULL) {
					free(ret);
					printf
					    ("\n Mem run out (init_stack), exiting");
					exit(1);
				}
				break;
			}
		case INT:
			{
				ret->i_vec = calloc(len, sizeof(int));
				if (ret->i_vec == NULL) {
					free(ret);
					printf
					    ("\n Mem run out (init_stack), exiting");
					exit(1);
				}
				break;
			}
		case DOUBLE:
			{
				ret->d_vec = calloc(len, sizeof(double));
				if (ret->i_vec == NULL) {
					free(ret);
					printf
					    ("\n Mem run out (init_stack), exiting");
					exit(1);
				}
				break;
			}
		case CHAR:
			{
				ret->c_vec = calloc(len, sizeof(char));
				if (ret->c_vec == NULL) {
					free(ret);
					printf
					    ("\n Mem run out (init_stack), exiting");
					exit(1);
				}
				break;
			}
		case BOOL:
			{
				ret->b_vec = calloc(len, sizeof(bool));
				if (ret->b_vec == NULL) {
					free(ret);
					printf
					    ("\n Mem run out (init_stack), exiting");
					exit(1);
				}
				break;
			}
		}
		ret->T = T;
		ret->len = len;
		ret->top = 0;
		return ret;
	}
}

unsigned long int hash(char *s)
{
	// char* str= strdup(s);
	long int len = strlen(s);
	unsigned long int hash = len;
	long int i = 0;

	for (i = 0; i < len; ++s, ++i) {
		hash = ((hash << 5) ^ (hash >> 27)) ^ (*s);
	}
	return hash;
}

word_key *init_word_key(char *i)
{
	word_key *ret = calloc(1, sizeof(word_key));
	if (ret == NULL) {
		exit(0);
	} else {
		ret->name = i;
		ret->key1 = hash(i);
		// free(i);
		return ret;
	}
}

void free_word_key(word_key * w)
{
	free(w->name);
	free(w);
}

stack *init_stack(long int len)
{
	stack *ret = calloc(1, sizeof(stack));
	if (ret == NULL) {
		printf("\n Mem run out (init_stack), exiting");
		exit(1);
	} else {
		ret->data = calloc(len, sizeof(l_val *));
		if (ret->data == NULL) {
			free(ret);
			printf("\n Mem run out (init_stack), exiting");
			exit(1);

		} else {
			ret->len = len;
			ret->top = 0;
			return ret;
		}

	}
}

void free_vec(vector * l)
{
	switch (l->T) {
	case ERROR:
		{
			free(l->err_code);
			break;
		}
	case INT:
		{
			free(l->i_vec);
			break;
		}
	case DOUBLE:
		{
			free(l->d_vec);
			break;
		}
	case BOOL:
		{
			free(l->b_vec);
			break;
		}
	case CHAR:
		{
			free(l->c_vec);
			break;
		}

	case STR:
		{
			for (long int i = 0; i < l->top; i++) {
				free(l->str_vec[i]);
			}
			free(l->str_vec);
			break;
		}
	case STACK:
		{
			for (long int i = 0; i < l->top; i++) {
				free_stack(l->s_vec[i]);
			}
			free(l->s_vec);
			break;
		}
	case WORD:
		{
			for (long int i = 0; i < l->top; i++) {
				free_word_key(l->word_vec[i]);
			}
			free(l->word_vec);
			break;
		}
	case SYMBOL:
		{

			for (long int i = 0; i < l->top; i++) {
				free_l_val(l->sym_vec[i]);
			}
			free(l->sym_vec);
			break;
		}
	case VEC:
		{
			for (long int i = 0; i < l->top; i++) {
				free_vec(l->vec_vec[i]);
			}
			free(l->vec_vec);
			break;
		}
	case STREAM:
		{
			for (long int i = 0; i < l->top; i++) {
				if (l->fp_vec[i] != NULL) {
					fclose(l->fp_vec[i]);
				}
			}
			free(l->fp_vec);
			break;
		}
	}
	free(l);
}

void free_l_val(l_val * l)
{
	switch (l->T) {
	case ERROR ... BOOL:
		{
			break;
		}
	case STR:
		{
			free(l->str_val);

			break;
		}
	case STACK:
		{
			free_stack(l->s_val);
			break;
		}
	case WORD:
		{
			free_word_key(l->word_val);
			break;
		}
	case SYMBOL:
		{
			free_l_val(l->sym_val);
			break;
		}
	case VEC:
		{
			free_vec(l->vec_val);
			break;
		}
	case STREAM:
		{
			if (l->fp != NULL) {
				fclose(l->fp);
			}
			break;
		}

	}
	free(l);

}

void free_stack(stack * s)
{
	for (long int i = 0; i < s->top; i++) {
		if (s->data[i]->T == STACK) {
			free_stack(s->data[i]->s_val);
			free(s->data[i]);
		} else if (s->data[i]->T == STR) {
			free(s->data[i]->str_val);
			free(s->data[i]);
		}

		else if (s->data[i]->T == WORD) {
			free_word_key(s->data[i]->word_val);
			free(s->data[i]);
		} else if (s->data[i]->T == STREAM) {
			if (s->data[i]->fp != NULL) {
				fclose(s->data[i]->fp);
			}
			// free_word_key(s->data[i]->word_val);
			free(s->data[i]);
		} else if (s->data[i]->T == SYMBOL) {
			free_l_val(s->data[i]->sym_val);
			free(s->data[i]);

		} else if (s->data[i]->T == VEC) {
			free_vec(s->data[i]->vec_val);
			free(s->data[i]);
		} else {
			free(s->data[i]);
		}
	}
	free(s->data);
	free(s);
}

void grow_stack(stack ** user_array)
{
	const long int totalSize = (*user_array)->len;
	l_val **data = (*user_array)->data;
	l_val **new_data = calloc(2 * totalSize, sizeof(l_val));

	if (new_data == NULL) {
		printf("Cannot allocate more memory (grow_stack).\n");
		exit(0);
	} else {
		for (long int i = 0; i < totalSize; i++) {
			new_data[i] = data[i];
		}
		free((*user_array)->data);
		(*user_array)->data = new_data;
		(*user_array)->len *= 2;
	}
}

void grow_vec(vector ** user_array)
{
	const long int totalSize = (*user_array)->len;
	switch ((*user_array)->T) {

	case VEC:
		{
			vector **data = (*user_array)->vec_vec;
			vector **new_data =
			    calloc(2 * totalSize, sizeof(vector *));
			if (new_data == NULL) {
				goto err;
			}
			for (long int i = 0; i < totalSize; i++) {
				new_data[i] = data[i];
			}
			free((*user_array)->vec_vec);
			(*user_array)->vec_vec = new_data;
			return;
		}

	case SYMBOL:
		{
			l_val **data = (*user_array)->sym_vec;
			l_val **new_data =
			    calloc(2 * totalSize, sizeof(l_val *));
			if (new_data == NULL) {
				goto err;
			}
			for (long int i = 0; i < totalSize; i++) {
				new_data[i] = data[i];
			}
			free((*user_array)->sym_vec);
			(*user_array)->sym_vec = new_data;
			return;
		}
	case STREAM:
		{
			FILE **data = (*user_array)->fp_vec;
			FILE **new_data = calloc(2 * totalSize, sizeof(FILE *));
			if (new_data == NULL) {
				goto err;
			}
			for (long int i = 0; i < totalSize; i++) {
				new_data[i] = data[i];
			}
			free((*user_array)->vec_vec);
			(*user_array)->fp_vec = new_data;
			return;
		}

	case WORD:
		{
			word_key **data = (*user_array)->word_vec;
			word_key **new_data =
			    calloc(2 * totalSize, sizeof(word_key *));
			if (new_data == NULL) {
				goto err;
			}
			for (long int i = 0; i < totalSize; i++) {
				new_data[i] = data[i];
			}
			free((*user_array)->word_vec);
			(*user_array)->word_vec = new_data;
			return;
		}
	case STACK:
		{
			stack **data = (*user_array)->s_vec;
			stack **new_data =
			    calloc(2 * totalSize, sizeof(stack *));
			if (new_data == NULL) {
				goto err;
			}
			for (long int i = 0; i < totalSize; i++) {
				new_data[i] = data[i];
			}
			free((*user_array)->s_vec);
			(*user_array)->s_vec = new_data;
			return;
		}
	case STR:
		{
			char **data = (*user_array)->str_vec;
			char **new_data = calloc(2 * totalSize, sizeof(char *));
			if (new_data == NULL) {
				goto err;
			}
			for (long int i = 0; i < totalSize; i++) {
				new_data[i] = data[i];
			}
			free((*user_array)->str_vec);
			(*user_array)->str_vec = new_data;
			return;
		}

	case INT:
		{
			long int *data = (*user_array)->i_vec;
			long int *new_data = calloc(2 * totalSize, sizeof(int));
			if (new_data == NULL) {
				goto err;
			}
			for (long int i = 0; i < totalSize; i++) {
				new_data[i] = data[i];
			}
			free((*user_array)->i_vec);
			(*user_array)->i_vec = new_data;
			return;
		}
	case ERROR:
		{
			long int *data = (*user_array)->err_code;
			long int *new_data = calloc(2 * totalSize, sizeof(int));
			if (new_data == NULL) {
				goto err;
			}
			for (long int i = 0; i < totalSize; i++) {
				new_data[i] = data[i];
			}
			free((*user_array)->err_code);
			(*user_array)->err_code = new_data;
			return;
		}
	case DOUBLE:
		{
			double *data = (*user_array)->d_vec;
			double *new_data =
			    calloc(2 * totalSize, sizeof(double));
			if (new_data == NULL) {
				goto err;
			}
			for (long int i = 0; i < totalSize; i++) {
				new_data[i] = data[i];
			}
			free((*user_array)->d_vec);
			(*user_array)->d_vec = new_data;
			return;
		}
	case CHAR:
		{
			char *data = (*user_array)->c_vec;
			char *new_data = calloc(2 * totalSize, sizeof(char));
			if (new_data == NULL) {
				goto err;
			}
			for (long int i = 0; i < totalSize; i++) {
				new_data[i] = data[i];
			}
			free((*user_array)->c_vec);
			(*user_array)->c_vec = new_data;
			return;
		}
	case BOOL:
		{
			char *data = (*user_array)->b_vec;
			char *new_data = calloc(2 * totalSize, sizeof(char));
			if (new_data == NULL) {
				goto err;
			}
			for (long int i = 0; i < totalSize; i++) {
				new_data[i] = data[i];
			}
			free((*user_array)->b_vec);
			(*user_array)->b_vec = new_data;
			return;
		}

	}

 err:
	printf("error in grow_vec, exiting");
	exit(1);
}

stack *split_stack(stack * s, long int n)
{
	long int split_pt = s->top - n;
	stack *ret = init_stack(n + 5);
	memcpy(ret->data, s->data + split_pt, (n + 1) * sizeof(l_val *));
	memset(s->data + split_pt, 0, (n + 1) * sizeof(l_val *));
	s->top = split_pt;
	ret->top = n;
	return ret;
}

vector *split_vec(vector * s, long int n)
{
	long int split_pt = s->top - n;
	vector *ret = init_vec(n + 5, s->T);
	switch (s->T) {
	case SYMBOL:
		{
			memcpy(ret->sym_vec, s->sym_vec + split_pt,
			       (n + 1) * sizeof(ret->sym_vec));
			memset(s->sym_vec + split_pt, 0,
			       (n + 1) * sizeof(ret->sym_vec));
			break;
		}
	case VEC:
		{
			memcpy(ret->vec_vec, s->vec_vec + split_pt,
			       (n + 1) * sizeof(ret->vec_vec));
			memset(s->vec_vec + split_pt, 0,
			       (n + 1) * sizeof(ret->vec_vec));
			break;
		}

	case WORD:
		{
			memcpy(ret->word_vec, s->word_vec + split_pt,
			       (n + 1) * sizeof(ret->word_vec));
			memset(s->word_vec + split_pt, 0,
			       (n + 1) * sizeof(ret->word_vec));
			break;
		}

	case STACK:
		{
			memcpy(ret->s_vec, s->s_vec + split_pt,
			       (n + 1) * sizeof(ret->s_vec));
			memset(s->s_vec + split_pt, 0,
			       (n + 1) * sizeof(ret->s_vec));
			break;
		}

	case STREAM:
		{
			memcpy(ret->fp_vec, s->fp_vec + split_pt,
			       (n + 1) * sizeof(ret->fp_vec));
			memset(s->fp_vec + split_pt, 0,
			       (n + 1) * sizeof(ret->fp_vec));
			break;
		}

	case STR:
		{
			memcpy(ret->str_vec, s->str_vec + split_pt,
			       (n + 1) * sizeof(ret->str_vec));
			memset(s->str_vec + split_pt, 0,
			       (n + 1) * sizeof(ret->str_vec));
			break;
		}

	case ERROR:
		{
			memcpy(ret->err_code, s->err_code + split_pt,
			       (n + 1) * sizeof(ret->err_code));
			memset(s->err_code + split_pt, 0,
			       (n + 1) * sizeof(ret->err_code));
			break;
		}
	case BOOL:
		{
			memcpy(ret->b_vec, s->b_vec + split_pt,
			       (n + 1) * sizeof(ret->b_vec));
			memset(s->b_vec + split_pt, 0,
			       (n + 1) * sizeof(ret->b_vec));
			break;
		}
	case INT:
		{
			memcpy(ret->i_vec, s->i_vec + split_pt,
			       (n + 1) * sizeof(ret->i_vec));
			memset(s->i_vec + split_pt, 0,
			       (n + 1) * sizeof(ret->i_vec));
			break;
		}
	case DOUBLE:
		{
			memcpy(ret->d_vec, s->d_vec + split_pt,
			       (n + 1) * sizeof(ret->d_vec));
			memset(s->d_vec + split_pt, 0,
			       (n + 1) * sizeof(ret->d_vec));
			break;
		}
	case CHAR:
		{
			memcpy(ret->c_vec, s->c_vec + split_pt,
			       (n + 1) * sizeof(ret->c_vec));
			memset(s->c_vec + split_pt, 0,
			       (n + 1) * sizeof(ret->c_vec));
			break;
		}

	}
	s->top = split_pt;
	ret->top = n;

	return ret;
}

void check_stack_size(stack ** s)
{

	if ((*s)->len - 2 == (*s)->top) {
		grow_stack(s);
	}
}

void check_vec_size(vector ** s)
{

	if ((*s)->len - 2 == (*s)->top) {
		grow_vec(s);
	}
}

types get_vec_type(vector * s, long int *c)
{
	if (s->T == VEC) {
		*c = *c + 1;
		return get_vec_type(s->vec_vec[0], c);
	} else {
		return s->T;
	}
}

long int check_vec_type_aux(vector * s1, vector * s2)
{
	long int rep1 = 0;
	long int rep2 = 0;
	types T1 = get_vec_type(s1, &rep1);
	types T2 = get_vec_type(s2, &rep2);
	//printf("rep1 %ld T1 %ld , rep2 %ld T2 %ld\n", rep1, rep2, T1, T2);
	if (rep1 == rep2 && T1 == T2) {
		return 1;
	} else {
		return 0;
	}
}

long int check_vec_type(vector * s1, l_val * i)
{
	if (i->T == VEC) {
		return check_vec_type_aux(s1, i->vec_val);
	} else {
		return s1->T == i->T;
	}
}

void push_stack(stack ** s, l_val * i)
{
	check_stack_size(s);
	(*s)->data[(*s)->top] = i;
	// free(i);
	(*s)->top += 1;
}

void push_vec(vector ** s, l_val * i)
{
	check_vec_size(s);

	// printf("DEBUG %ld %ld",i->T,(*s)->T);
	if (check_vec_type(*s, i) == 1) {
		switch (i->T) {
		case VEC:
			{
				(*s)->vec_vec[(*s)->top] = i->vec_val;
				(*s)->top += 1;
				free(i);
				break;
			}
		case SYMBOL:
			{
				(*s)->sym_vec[(*s)->top] = i->sym_val;
				(*s)->top += 1;
				free(i);
				break;
			}
		case WORD:
			{
				(*s)->word_vec[(*s)->top] = i->word_val;
				(*s)->top += 1;
				free(i);
				break;
			}
		case STREAM:
			{
				(*s)->fp_vec[(*s)->top] = i->fp;
				(*s)->top += 1;
				free(i);
				break;
			}
		case STR:
			{
				(*s)->str_vec[(*s)->top] = i->str_val;
				(*s)->top += 1;
				free(i);
				break;
			}
		case STACK:
			{
				(*s)->s_vec[(*s)->top] = i->s_val;
				(*s)->top += 1;
				free(i);
				break;
			}
		case ERROR:
			{
				(*s)->err_code[(*s)->top] = i->err_code;
				(*s)->top += 1;
				free(i);
				break;
			}
		case INT:
			{
				(*s)->i_vec[(*s)->top] = i->i_val;
				(*s)->top += 1;
				free(i);
				break;
			}
		case DOUBLE:
			{
				(*s)->d_vec[(*s)->top] = i->d_val;
				(*s)->top += 1;
				free(i);
				break;
			}
		case BOOL:
			{
				(*s)->b_vec[(*s)->top] = i->b_val;
				(*s)->top += 1;
				free(i);
				break;
			}
		case CHAR:
			{
				(*s)->c_vec[(*s)->top] = i->c_val;
				(*s)->top += 1;
				free(i);
				break;
			}

		}
	} else {
		printf
		    ("error in push_vec,vector and elem are not of the same type \n");
		exit(1);
	}
}

l_val *l_val_err(long int i)
{
	l_val *ret = calloc(1, sizeof(l_val));
	if (ret == NULL) {
		printf("\n Mem run out (l_val_err), exiting");
	} else {
		ret->T = ERROR;
		ret->err_code = i;
	}
	return ret;
}

l_val *l_val_int(long int i)
{
	l_val *ret = calloc(1, sizeof(l_val));
	if (ret == NULL) {
		printf("\n Mem run out (l_val_int), exiting");
	} else {
		ret->T = INT;
		ret->i_val = i;
	}
	return ret;
}

l_val *pop_stack(stack ** s)
{
	if ((*s)->top == 0) {
		l_val *ret = l_val_err(1);
		return ret;
	}
	l_val *ret = (*s)->data[(*s)->top - 1];
	(*s)->data[(*s)->top - 1] = NULL;
	(*s)->top -= 1;
	return ret;
}

l_val *pop_vec(vector ** s)
{
	if ((*s)->top == 0) {
		l_val *ret = l_val_err(1);
		return ret;
	}
	l_val *ret;
	switch ((*s)->T) {
	case ERROR:{
			(*s)->top -= 1;
			ret = l_val_err((*s)->err_code[(*s)->top]);
			break;
		}
	case INT:{
			(*s)->top -= 1;
			ret = l_val_int((*s)->i_vec[(*s)->top]);
			break;
		}
	case DOUBLE:{
			(*s)->top -= 1;
			ret = l_val_double((*s)->d_vec[(*s)->top]);
			break;
		}
	case CHAR:{
			(*s)->top -= 1;
			ret = l_val_char((*s)->c_vec[(*s)->top]);
			break;
		}
	case BOOL:{
			(*s)->top -= 1;
			ret = l_val_bool((*s)->b_vec[(*s)->top]);
			break;
		}
	case STACK:{
			(*s)->top -= 1;
			ret = l_val_stack((*s)->s_vec[(*s)->top]);
			break;
		}
	case VEC:{
			(*s)->top -= 1;
			ret = l_val_vec((*s)->vec_vec[(*s)->top]);
			break;
		}
	case SYMBOL:{
			(*s)->top -= 1;
			ret = l_val_sym((*s)->sym_vec[(*s)->top]);
			break;
		}
	case STR:{
			(*s)->top -= 1;
			ret = l_val_string((*s)->str_vec[(*s)->top]);
			break;
		}
	case STREAM:{
			(*s)->top -= 1;
			ret = l_val_file((*s)->fp_vec[(*s)->top]);
			break;
		}
	case WORD:{
			(*s)->top -= 1;
			ret = l_val_word((*s)->word_vec[(*s)->top]->name);
			break;
		}
	default:
		{
			printf("this shouldnt happen in %s", __FUNCTION__);
			ret = l_val_err(1);
		}
	}
	return ret;

	/* l_val *ret = (*s)->data[(*s)->top - 1]; */
	/* (*s)->data[(*s)->top - 1] = NULL; */
	/* (*s)->top -= 1; */
	/* return ret; */
}

l_val *l_val_double(double i)
{
	l_val *ret = calloc(1, sizeof(l_val));
	if (ret == NULL) {
		printf("\n Mem run out (l_val_double), exiting");
	} else {
		ret->T = DOUBLE;
		ret->d_val = i;
	}
	return ret;
}

l_val *l_val_char(char i)
{
	l_val *ret = calloc(1, sizeof(l_val));
	if (ret == NULL) {
		printf("\n Mem run out (l_val_char), exiting");
	} else {
		ret->T = CHAR;
		ret->c_val = i;
	}
	return ret;
}

l_val *l_val_bool(bool i)
{
	l_val *ret = calloc(1, sizeof(l_val));
	if (ret == NULL) {
		printf("\n Mem run out (l_val_bool), exiting");
	} else {
		ret->T = BOOL;
		ret->b_val = i;
	}
	return ret;
}

l_val *l_val_file(FILE * fp)
{
	l_val *ret = calloc(1, sizeof(l_val));
	if (ret == NULL) {
		printf("\n Mem run out (l_val_file), exiting");
	} else {
		ret->T = STREAM;
		ret->fp = fp;
	}
	return ret;
}

l_val *l_val_string(char *i)
{
	l_val *ret = calloc(1, sizeof(l_val));
	if (ret == NULL) {
		printf("\n Mem run out (l_val_stack), exiting");
	} else {
		ret->T = STR;
		ret->str_val = i;
	}
	return ret;
}

l_val *l_val_word(char *i)
{
	l_val *ret = calloc(1, sizeof(l_val));
	if (ret == NULL) {
		printf("\n Mem run out (l_val_stack), exiting");
	} else {
		ret->T = WORD;
		ret->word_val = init_word_key(i);
	}
	return ret;
}

l_val *l_val_stack(stack * i)
{
	l_val *ret = calloc(1, sizeof(l_val));
	if (ret == NULL) {
		printf("\n Mem run out (l_val_stack), exiting");

	} else {
		ret->T = STACK;
		ret->s_val = i;
	}
	return ret;
}

l_val *l_val_vec(vector * i)
{
	l_val *ret = calloc(1, sizeof(l_val));
	if (ret == NULL) {
		printf("\n Mem run out (l_val_stack), exiting");

	} else {
		ret->T = VEC;
		ret->vec_val = i;
	}
	return ret;
}

l_val *l_val_sym(l_val * i)
{
	l_val *ret = calloc(1, sizeof(l_val));
	if (ret == NULL) {
		printf("\n Mem run out (l_val_sym), exiting");

	} else {
		ret->T = SYMBOL;
		ret->sym_val = i;
	}
	return ret;
}

stack *copy_stack(stack * s)
{
	stack *ret = init_stack(s->top + 1);
	for (long int i = 0; i < s->top; i++) {
		l_val *dummy = copy_lval(s->data[i]);
		push_stack(&ret, dummy);
	}
	return ret;
}

vector *copy_vec(vector * val)
{
	vector *v = init_vec(val->top + 1, val->T);
	switch (val->T) {
	case ERROR:
		{
			// memcpy(v->err_code, val->err_code, sizeof(int)*val->top);

			break;
		}
	case INT:
		{
			// memcpy(v->i_vec, val->i_vec, sizeof(int)*val->top);
			for (long int i = 0; i < val->top; i++) {
				v->i_vec[i] = val->i_vec[i];
			}
			break;
		}
	case DOUBLE:
		{
			for (long int i = 0; i < val->top; i++) {
				v->d_vec[i] = val->d_vec[i];
			}
			break;
		}
	case CHAR:
		{
			for (long int i = 0; i < val->top; i++) {
				v->c_vec[i] = val->c_vec[i];
			}
			break;
		}
	case BOOL:
		{
			for (long int i = 0; i < val->top; i++) {
				v->b_vec[i] = val->b_vec[i];
			}
			break;
		}
	case STR:
		{
			for (long int i = 0; i < val->top; i++) {
				v->str_vec[i] = strdup(val->str_vec[i]);
			}
			break;
		}
	case STACK:
		{

			for (long int i = 0; i < val->top; i++) {
				v->s_vec[i] = copy_stack(val->s_vec[i]);
			}
			break;
		}
	case WORD:
		{

			for (long int i = 0; i < val->top; i++) {
				v->word_vec[i] =
				    init_word_key(strdup
						  (val->word_vec[i]->name));
			}
			break;
		}
	case STREAM:
		{
			printf("stream copying is not implemented yet");
			exit(0);
			break;
		}
	case SYMBOL:
		{

			for (long int i = 0; i < val->top; i++) {
				v->sym_vec[i] = copy_lval(val->sym_vec[i]);
			}
			break;
		}
	case VEC:
		{

			for (long int i = 0; i < val->top; i++) {
				v->vec_vec[i] = copy_vec(val->vec_vec[i]);
			}
			break;
		}
	}
	v->top = val->top;
	return v;
}

l_val *copy_lval(l_val * val)
{
	l_val *ret;
	switch (val->T) {
	case ERROR:
		{
			printf("error in copy_lval");
			ret = l_val_err(val->err_code);
			break;
		}
	case INT:
		{
			ret = l_val_int(val->i_val);
			break;
		}
	case DOUBLE:
		{
			ret = l_val_double(val->d_val);
			break;
		}
	case CHAR:
		{
			ret = l_val_char(val->c_val);
			break;
		}
	case BOOL:
		{
			ret = l_val_bool(val->b_val);
			break;
		}
	case STR:
		{
			char *dummy = strdup(val->str_val);
			ret = l_val_string(dummy);
			break;
		}
	case STACK:
		{
			stack *ret_val = init_stack(val->s_val->top);
			for (long int i = 0; i < val->s_val->top; i++) {
				l_val *dummy = copy_lval(val->s_val->data[i]);
				push_stack(&ret_val, dummy);
			}
			ret = l_val_stack(ret_val);
			break;
		}
	case WORD:
		{
			char *dum = strdup(val->word_val->name);
			ret = l_val_word(dum);
			break;
		}
	case STREAM:
		{
			printf("stream copying is not implemented yet");
			ret = l_val_err(1);
			break;
		}
	case SYMBOL:
		{
			l_val *new = copy_lval(val->sym_val);
			ret = l_val_sym(new);
			break;
		}
	case VEC:
		{
			// print_vec_aux(val->vec_val,4," " , " ");
			vector *new = copy_vec(val->vec_val);
			ret = l_val_vec(new);
			break;
		}
	default:
		{
			printf("this shouldnt happen in %s", __FUNCTION__);
			ret = l_val_err(1);
		}

	}
	return ret;
}

vector *stack2vec(stack * s)
{
	long int ret = 1;
	types T = s->data[0]->T;

	long int rep1 = 0;
	types T2 = 0;

	if (T == VEC) {
		rep1 = 0;
		T2 = get_vec_type(s->data[0]->vec_val, &rep1);
	}

	for (long int i = 1; i < s->top; i++) {
		if (T != s->data[i]->T) {
			ret = 0;
			break;
		}
		if (T == VEC) {
			long int rep2 = 0;
			types T3 = get_vec_type(s->data[i]->vec_val, &rep2);
			if (rep2 != rep1 || T3 != T2) {
				ret = 0;
				break;
			}
		}
	}
	if (ret == 1) {
		vector *v = init_vec(s->len, T);
		for (long int i = 0; i < s->top; i++) {
			switch (T) {
			case ERROR:{
					break;
				}
			case INT:{
					v->i_vec[i] = s->data[i]->i_val;
					break;
				}
			case DOUBLE:{
					v->d_vec[i] = s->data[i]->d_val;
					break;
				}
			case CHAR:{
					v->c_vec[i] = s->data[i]->c_val;
					break;
				}
			case BOOL:{
					v->b_vec[i] = s->data[i]->b_val;
					break;
				}
			case STR:{
					v->str_vec[i] = s->data[i]->str_val;
					break;
				}
			case STACK:{
					v->s_vec[i] = s->data[i]->s_val;
					break;
				}
			case SYMBOL:{
					v->sym_vec[i] = s->data[i]->sym_val;
					break;
				}
			case WORD:{
					v->word_vec[i] = s->data[i]->word_val;
					break;
				}
			case VEC:{
					v->vec_vec[i] = s->data[i]->vec_val;
					break;
				}
			case STREAM:{
					v->fp_vec[i] = s->data[i]->fp;
					break;
				}

			}
			free(s->data[i]);

		}
		v->top = s->top;
		free(s->data);
		free(s);
		return (v);
	} else {
		printf("error in stack2vec exiting");
		exit(0);
	}
}

word_val *prim_func(char *name, func f)
{
	word_val *ret = calloc(1, sizeof(word_val));
	if (ret == NULL) {
		printf("\n Mem run out (prim_func), exiting");
		exit(1);
	} else {
		ret->f = f;
		ret->flag = PRIM_FUN;
		ret->name = name;
		ret->next = NULL;
	}
	return ret;
}

word_val *user_func(char *name, stack * f)
{
	word_val *ret = calloc(1, sizeof(word_val));
	if (ret == NULL) {
		printf("\n Mem run out (user_func), exiting");
		exit(1);
	} else {
		ret->data = f;
		ret->flag = USER_FUN;
		ret->name = name;
		ret->next = NULL;
	}
	return ret;
}

void append_val(word_val ** list, word_val * val)
{

	word_val *p = *list;

	if (p == NULL) {
		p = val;
	} else {
		for (; p->next != NULL;) {
			p = p->next;
		}
		p->next = val;
	}
	// return *list;
}

void free_word_vals(word_val * list)
{
	word_val *p = list;
	for (; p != NULL;) {
		word_val *r = p->next;
		switch (p->flag) {
		case USER_FUN:
			{
				free(p->name);
				free_stack(p->data);
				free(p);
				break;
			}
		case PRIM_FUN:
			{
				// printf("heyo");
				free(p->name);
				free(p);
				break;
			}
		}
		p = r;
	}
}

word_dict *init_dict(long int len)
{
	word_dict *ret = calloc(1, sizeof(word_dict));
	if (ret == NULL) {
		printf("\n Mem run out (init_dict), exiting");
		exit(1);
	} else {
		ret->data = calloc(len, sizeof(word_val **));
		if (ret == NULL) {
			printf("\n Mem run out (init_dict), exiting");
			exit(1);
		} else {
			ret->len = len;
			return ret;
		}
	}
}

void free_dict(word_dict * d)
{
	for (long int i = 0; i < d->len; i++) {

		if (d->data[i] != NULL) {
			free_word_vals(d->data[i]);
		} else {
			free(d->data[i]);
		}
	}
	free(d->data);
	free(d);
}

void add_dict(word_dict ** dict, unsigned long int key, word_val * val)
{
	word_dict *d = *dict;
	unsigned long int n = key % d->len;
	word_val *data = d->data[n];
	if (data == NULL) {
		d->data[n] = val;
		// data=*val;
	} else {
		append_val(&data, val);
	}
}

word_val *retrieve_dict(word_dict * dict, word_key * key)
{
	word_val *ret = dict->data[key->key1 % dict->len];
	if (ret == NULL) {
		printf("error in retrieve_dict,No Such Key  : %s", key->name);
		return NULL;
	}

	char *name = strdup(ret->name);
	for (; ret != NULL; ret = ret->next) {
		long int cmp = strcmp(key->name, ret->name);
		if (cmp == 0) {
			switch (ret->flag) {
			case PRIM_FUN:
				{
					free_word_key(key);
					func f = ret->f;
					return prim_func(name, f);
					break;
				}
			case USER_FUN:
				{
					free_word_key(key);
					stack *body = copy_stack(ret->data);
					return user_func(name, body);
					break;
				}
			}
			break;
		}
	}
	printf("error in retrieve_dict");
	return NULL;
	exit(1);		// / may change later thats why under
	// return

}

void link_stacks(stack ** s, stack * body)
{
	l_val **data = body->data;
	for (long int i = 0; i < body->top; i++) {
		push_stack(s, data[i]);
	}
	free(body->data);
	free(body);
}


void vector_set(vector * vec, l_val * val, long int n)
{
	switch (vec->T) {
	case ERROR:
		{
			vec->err_code[n] = val->err_code;
			free(val);
			return;
		}
	case INT:
		{
			vec->i_vec[n] = val->i_val;
			free(val);
			return;
		}
	case DOUBLE:
		{
			vec->d_vec[n] = val->d_val;
			free(val);
			return;
		}
	case CHAR:
		{
			vec->c_vec[n] = val->c_val;
			free(val);
			return;
		}
	case BOOL:
		{
			vec->c_vec[n] = val->c_val;
			free(val);
			return;
		}
	case STR:
		{
			free(vec->str_vec[n]);
			vec->str_vec[n] = val->str_val;
			free(val);
			return;
		}
	case SYMBOL:
		{
			free_l_val(vec->sym_vec[n]);
			vec->sym_vec[n] = val->sym_val;
			free(val);
			return;
		}
	case STREAM:
		{
			fclose(vec->fp_vec[n]);
			free(vec->fp_vec[n]);
			vec->fp_vec[n] = val->fp;
			free(val);
			return;
		}
	case WORD:
		{
			free_word_key(vec->word_vec[n]);
			vec->word_vec[n] = val->word_val;
			free(val);
			return;
		}

	case STACK:
		{
			free_stack(vec->s_vec[n]);
			vec->s_vec[n] = val->s_val;
			free(val);
			return;
		}
	case VEC:
		{
			free_vec(vec->vec_vec[n]);
			vec->vec_vec[n] = val->vec_val;
			free(val);
			return;
		}

	}
}

l_val *vector_get(vector * vec, long int n)
{
	switch (vec->T) {
	case ERROR:
		{
			return l_val_err(vec->err_code[n]);
		}
	case INT:
		{
			return l_val_int(vec->i_vec[n]);

		}
	case DOUBLE:
		{
			return l_val_double(vec->d_vec[n]);
		}
	case CHAR:
		{
			return l_val_char(vec->c_vec[n]);
		}
	case BOOL:
		{
			return l_val_bool(vec->c_vec[n]);

		}
	case STR:
		{
			return l_val_string(strdup(vec->str_vec[n]));

		}
	case SYMBOL:
		{
			return (copy_lval(vec->sym_vec[n]));

		}
	case STREAM:
		{
			printf("vec stream get not implemented yet in %s %d \n",
			       __FUNCTION__, __LINE__);
			return l_val_err(0);
		}
	case WORD:
		{
			char *dum = strdup(vec->word_vec[n]->name);
			l_val* ret = l_val_word(dum);
			return ret;
		}

	case STACK:
		{
                  return (l_val_stack(copy_stack(vec->s_vec[n])));
		}
	case VEC:
		{
                  return (l_val_vec(copy_vec(vec->vec_vec[n])));
		}

	}
}
