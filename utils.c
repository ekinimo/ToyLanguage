
#include "data.h"
#include "utils.h"

void print_level(long int level, char *  p)
{
	// printf("|");
	for (long int i = 0; i < level; i++) {
		printf("%4s", p);
	}
	// printf("|");
}

void print_l_val(l_val *   val)
{
	switch (val->T) {
	case STREAM:{
			printf("STREAM");
			break;
		}
	case ERROR:{
			printf("error");
			break;
		}
	case INT:{
			printf("%ld", val->i_val);
			break;
		}
	case DOUBLE:{
			printf("%lf", val->d_val);
			break;
		}
	case CHAR:{
			printf("%c", val->c_val);
			break;
		}
	case BOOL:{
			val->b_val == false ? printf(" False") : printf("True");
			break;
		}
	case STR:{
			printf("\"%s\"", val->str_val);
			break;
		}
	case WORD:{
			printf("%s", val->word_val->name);
			break;
		}
	case SYMBOL:{
			printf("_");
			print_l_val(val->sym_val);
			break;
		}
	case STACK:{
			printf(" [ ");
			for (long int i = 0; i < val->s_val->top; i++) {
				printf(" ");
				print_l_val(val->s_val->data[i]);
				printf(" ");
			}
			printf(" ] ");
			break;
		}
	case VEC:{
			/*
			 * printf(" { "); 
			 */
			/*
			 * for(long int i = 0 ; i < val->vec_val->top ; i++){ 
			 */
			/*
			 * printf(" "); 
			 */
			/*
			 * switch(val->vec_val->T){ 
			 */
			/*
			 * case ERROR :{ 
			 */
			/*
			 * print_l_val(val->vec_val->err_code[i]); 
			 */
			/*
			 * } 
			 */

			/*
			 * } 
			 */
			/*
			 * printf(" "); 
			 */
			/*
			 * printf(" } "); 
			 */
			break;
		}

	}
}

void print_stack_aux(stack * s, long int level, char *  p1, char *  p2)
{
	// printf("|");
	for (long int i = 0; i < s->top; i++) {
		types T = s->data[i]->T;

		print_level(level, p2);	// printf("|");

		switch (T) {
		case ERROR:
			{
				print_level(1, p1);
				printf(" ");
				printf("error_code= %ld", s->data[i]->err_code);
				printf(" : Error\n");

				break;
			}
		case SYMBOL:
			{
				// print_level(level,p2);
				print_level(1, p1);
				printf(" ");
				print_l_val(s->data[i]->sym_val);
				printf(" : Symbol\n");
				break;
			}

		case INT:
			{
				// print_level(level,p2);
				print_level(1, p1);
				printf(" %ld : Int\n", s->data[i]->i_val);
				break;
			}
		case DOUBLE:
			{
				// print_level(level,p2);
				print_level(1, p1);
				printf(" %.2lf : Double\n", s->data[i]->d_val);
				break;
			}
		case CHAR:{
				// print_level(level,p2);
				print_level(1, p1);
				printf(" %c : Char\n", s->data[i]->c_val);
				break;
			}
		case BOOL:
			{
				// print_level(level,p2);
				print_level(1, p1);
				s->data[i]->b_val ==
				    0 ? printf(" False : Bool\n") :
				    printf(" True : Bool\n");
				break;
			}
		case STR:
			{
				// print_level(level,p2);
				print_level(1, p1);
				printf(" %s : String \n", s->data[i]->str_val);
				break;
			}
		case WORD:
			{
				// print_level(level,p2);
				print_level(1, p1);
				printf(" %s : Word (hash : %lu) \n",
				       s->data[i]->word_val->name,
				       s->data[i]->word_val->key1 % 10);
				break;
			}

		case STACK:
			{
				// print_level(level,p2);
				print_level(1, p1);
				printf(" Stack\n");
				print_stack_aux(s->data[i]->s_val, level + 1,
						p1, p2);
				break;
			}
		case VEC:
			{
				// print_level(level,p2);
				print_level(1, p1);
				printf(" Vector of len %ld\n",
				       s->data[i]->vec_val->top);
				print_vec_aux(s->data[i]->vec_val, level + 1,
					      p1, p2);
				break;
			}

		case STREAM:
			{
				print_level(1, p1);
				printf(" %p : File \n", s->data[i]->fp);
			}

		}

		// printf("\n");
	}

}

void print_vec_aux(vector * s, long int level, char *p1, char *p2)
{
	// printf("|");
	types T = s->T;
	for (long int i = 0; i < s->top; i++) {
		print_level(level, p2);	// printf("|");

		switch (T) {
		case ERROR:
			{
				printf(" error\n");
				break;
			}
		case SYMBOL:
			{
				// print_level(level,p2);
				print_level(1, p1);
				printf(" ");
				print_l_val(s->sym_vec[i]);
				printf(" : Symbol\n");
				break;
			}

		case INT:
			{
				// print_level(level,p2);
				print_level(1, p1);
				printf(" %ld : Int\n", s->i_vec[i]);
				break;
			}
		case DOUBLE:
			{
				// print_level(level,p2);
				print_level(1, p1);
				printf(" %.2lf : Double\n", s->d_vec[i]);
				break;
			}
		case CHAR:{
				// print_level(level,p2);
				print_level(1, p1);
				printf(" %c : Char\n", s->c_vec[i]);
				break;
			}
		case BOOL:
			{
				// print_level(level,p2);
				print_level(1, p1);
				s->b_vec[i] ==
				    0 ? printf(" False : Bool\n") :
				    printf(" True : Bool\n");
				break;
			}
		case STR:
			{
				// print_level(level,p2);
				print_level(1, p1);
				printf(" %s : String \n", s->str_vec[i]);
				break;
			}
		case WORD:
			{
				// print_level(level,p2);
				print_level(1, p1);
				printf(" %s : Word (hash : %lu) \n",
				       s->word_vec[i]->name,
				       s->word_vec[i]->key1 % 10);
				break;
			}

		case STACK:
			{
				// print_level(level,p2);
				print_level(1, p1);
				printf(" Stack\n");
				print_stack_aux(s->s_vec[i], level + 1, p1, p2);
				break;
			}
		case VEC:
			{
				// print_level(level,p2);
				print_level(1, p1);
				printf(" Vector\n");
				print_vec_aux(s->vec_vec[i], level + 1, p1, p2);
				break;
			}

		case STREAM:
			{
				print_level(1, p1);
				printf(" %p : File \n", s->fp_vec[i]);
				break;
			}

		}

		// printf("\n");
	}

}

void print_stack(stack * s, long int level)
{
	printf("\n");
	print_stack_aux(s, level, "_", "  |");
	printf("\n");
}

long int validDouble(char *s)
{
	char *end;
	strtod(s, &end);
	return s != end;
}

long int validInt(char *s)
{
	char *end;
	long int number = strtol(s, &end, 10);

	if (*end != '\0' || end == s) {
		return 0;
	} else {
		return 1;
	}

}

stack *parse_str(char *  fp, long int *  ctr)
{
	char *aux = fp + *ctr;
	char ptr[1000];
	long int loop;

	char sym_flag = 0;

	stack *ret = init_stack(10);
	loop = sscanf(aux, "%s", ptr);

	while (loop != EOF) {

		long int i;
		double f;
                if (ptr[0] == '_') {
                  sym_flag += 1;
                  *ctr = *ctr + 1;
		}
		else if (ptr[0] == '[') {
			stack *Object;
			*ctr = *ctr + 2;
			Object = parse_str(fp, ctr);
			l_val *val = l_val_stack(Object);
			while (sym_flag != 0) {
				val = l_val_sym(val);
				sym_flag -= 1;
			}
			push_stack(&ret, val);

		} else if (ptr[0] == '{') {
			stack *Object;
			*ctr = *ctr + 2;
			Object = parse_str(fp, ctr);
			// print_stack(Object, 2);
			vector *vect = stack2vec(Object);
			l_val *val = l_val_vec(vect);
			while (sym_flag != 0) {
				val = l_val_sym(val);
				sym_flag -= 1;
			}
			push_stack(&ret, val);

		}

		else if (ptr[0] == ']') {
			*ctr = *ctr + 2;
			return ret;
		} else if (ptr[0] == '}') {
			*ctr = *ctr + 2;
			return ret;
		}

                else if (validDouble(ptr) && !validInt(ptr)) {
			sscanf(ptr, "%lf", &f);
			long int l = strlen(ptr);
			*ctr = *ctr + l + 1;
			l_val *val = l_val_double(f);
			while (sym_flag != 0) {
				val = l_val_sym(val);
				sym_flag -= 1;
			}
			push_stack(&ret, val);
		} else if (validInt(ptr)) {
			sscanf(ptr, "%ld", &i);
			long int l = strlen(ptr);
			*ctr = *ctr + l + 1;
			l_val *val = l_val_int(i);
			while (sym_flag != 0) {
				val = l_val_sym(val);
				sym_flag -= 1;
			}
			push_stack(&ret, val);
		}

		else if (strncmp(ptr, "True", 4) == 0) {
			long int l = strlen(ptr);
			*ctr = *ctr + l + 1;
			l_val *val = l_val_bool(1);
			while (sym_flag != 0) {
				val = l_val_sym(val);
				sym_flag -= 1;
			}
			push_stack(&ret, val);
		} else if (strncmp(ptr, "False", 4) == 0) {
			long int l = strlen(ptr);
			*ctr = *ctr + l + 1;
			l_val *val = l_val_bool(0);
			while (sym_flag != 0) {
				val = l_val_sym(val);
				sym_flag -= 1;
			}
			push_stack(&ret, val);
		} else if (ptr[0] == '\'') {	// burada sikiniti var
			long int l = strlen(ptr);
			*ctr = *ctr + l + 1;
			char b;
			sscanf(ptr + 1, "%c", &b);
			l_val *val = l_val_char(b);
			while (sym_flag != 0) {
				val = l_val_sym(val);
				sym_flag -= 1;
			}
			push_stack(&ret, val);
		} else if (ptr[0] == '\"') {
			// long int l = strlen(ptr);
			char str_val[1000];
			memset(str_val, '\0', 1000);
			str_val[0] = '\"';
			char *anan = memccpy(str_val + 1, fp + (*ctr) + 1, '\"',
					     1000 * sizeof(char));
			if (anan == NULL) {
				printf("string is not closed");
				break;
			}
			long int l = strlen(str_val);
			*ctr = *ctr + l + 1;
			printf("%s", str_val);
			l_val *val = l_val_string(strdup(str_val));
			while (sym_flag != 0) {
				val = l_val_sym(val);
				sym_flag -= 1;
			}
			push_stack(&ret, val);
		}

		else {
			long int l = strlen(ptr);
			char *dummy = strdup(ptr);
			l_val *val = l_val_word(dummy);
			while (sym_flag != 0) {
				val = l_val_sym(val);
				sym_flag -= 1;
			}
			push_stack(&ret, val);
			*ctr = *ctr + l + 1;
		}
		aux = fp + *ctr;
		loop = sscanf(aux, "%s", ptr);
	}
	return ret;
}

stack *parse_file(FILE * fp)
{

	char ptr[1000];
	char helper[1000];
	long int loop;

	char sym_flag = 0;

	stack *ret = init_stack(10);
	loop = fscanf(fp, "%s", ptr);

	while (loop != EOF) {

		long int i;
		double f;

               if (ptr[0] == '_') {
                  sym_flag += 1;
                  printf("sym_flag\n");
                  loop = fscanf(fp, "%c", ptr);

               }
	        if (ptr[0] == '[') {
			stack *Object;
			Object = parse_file(fp);
			l_val *val = l_val_stack(Object);
			while (sym_flag != 0) {
				val = l_val_sym(val);
				sym_flag -= 1;
			}
			push_stack(&ret, val);

		} else if (ptr[0] == '{') {
			stack *Object;
			Object = parse_file(fp);
			// print_stack(Object, 2);
			vector *vect = stack2vec(Object);
			l_val *val = l_val_vec(vect);
			while (sym_flag != 0) {
				val = l_val_sym(val);
				sym_flag -= 1;
			}
			push_stack(&ret, val);

		}

		else if (ptr[0] == ']') {
			return ret;
		} else if (ptr[0] == '}') {
			return ret;
		}  else if (validDouble(ptr) && !validInt(ptr)) {
			sscanf(ptr, "%lf", &f);
			l_val *val = l_val_double(f);
			while (sym_flag != 0) {
				val = l_val_sym(val);
				sym_flag -= 1;
			}
			push_stack(&ret, val);
		} else if (validInt(ptr)) {
			sscanf(ptr, "%ld", &i);
			l_val *val = l_val_int(i);
			while (sym_flag != 0) {
				val = l_val_sym(val);
				sym_flag -= 1;
			}
			push_stack(&ret, val);
		}

		else if (strncmp(ptr, "True", 4) == 0) {
			l_val *val = l_val_bool(1);
			while (sym_flag != 0) {
				val = l_val_sym(val);
				sym_flag -= 1;
			}
			push_stack(&ret, val);
		} else if (strncmp(ptr, "False", 4) == 0) {
			l_val *val = l_val_bool(0);
			while (sym_flag != 0) {
				val = l_val_sym(val);
				sym_flag -= 1;
			}
			push_stack(&ret, val);
		} else if (ptr[0] == '\'') {	// burada sikiniti var
			char b;
			sscanf(ptr + 1, "%c", &b);
			l_val *val = l_val_char(b);
			while (sym_flag != 0) {
				val = l_val_sym(val);
				sym_flag -= 1;
			}
			push_stack(&ret, val);
		} else if (sscanf(ptr, "\"%[^\"]\"", helper) == 1) {	// burada
			// sikiniti 
			// var
			char *dummy = strdup(helper);
			l_val *val = l_val_string(dummy);
			while (sym_flag != 0) {
				val = l_val_sym(val);
				sym_flag -= 1;
			}
			push_stack(&ret, val);
		} else {
			char *dummy = strdup(ptr);
			l_val *val = l_val_word(dummy);
			while (sym_flag != 0) {
				val = l_val_sym(val);
				sym_flag -= 1;
			}
			push_stack(&ret, val);
		}
		// aux = fp+*ctr;
		loop = fscanf(fp, "%s", ptr);
	}
	return ret;
}

void print_word_vals(word_val *   list)
{
	word_val *p = list;

	for (; p != NULL; p = p->next) {
		printf("%s :: ", p->name);
		if (p->flag == USER_FUN) {
			printf("user_func ");
			print_stack(p->data, 3);
			printf(" -->");
		} else {
			printf("prim_func --> ");
		}
	}
}

void print_dict(word_dict * d)
{
	// printf("len : %ld ",d->len);
	for (long int i = 0; i < d->len; i++) {
		printf("%ld  - ", i), print_word_vals(d->data[i]);
		printf("\n");
	}
}
