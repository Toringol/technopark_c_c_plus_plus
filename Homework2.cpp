/*
*	Шепелев Сергей АПО-11 
*	Требуется написать программу, которая способна вычислять арифметические выражения. 
*	Выражения могут содержать: 
*	1) знаки операций '+', '-', '/', '*' 
*	2) Скобки '(', ')' 
*	3) Целые числа, в нотации '123', в том числе они достаточно большие (требуется реализовать 
*	длинную арифметику), все операции должны быть целочисленные
*
*	4) необходимо учитывать приоритеты операций, и возможность унарного минуса, пробелы ничего 
*	не значат 
*	5) Если в выражении встретилась ошибка требуется вывести в стандартный поток вывода "
*	[error]" (без кавычек) 
*
*	Программа должна уметь обрабатывать ошибки во входных данных.
*	В случае возникновения ошибки нужно вывести в поток стандартного вывода сообщение
*	"[error]" (без кавычек) и завершить выполнение программы.
*/


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define STACK_SIZE 20
#define BUFFER_SIZE 1024
#define MAXLINE 100
#define	MAXDIGITS 100
#define MAXOP 100
#define NUMBER 1
#define ENDOFTHEFILE 0

#define PLUS 1		
#define MINUS -1

typedef struct {
	char digits[MAXDIGITS];
	int signbit;
	int lastdigit;
} bignum;

bignum stack_of_operands[STACK_SIZE];
int stack_of_operations[STACK_SIZE];

int stack_pointer_of_operands = 0;
int stack_pointer_of_operations = 0;

int line_i = 0;

void push_in_stack_of_operands(bignum value);
void push_in_stack_of_operations(int value);

bignum pop_from_stack_of_operands(void);
int pop_from_stack_of_operations(void);

void print_bignum(bignum *n);
void int_to_bignum(int s, bignum *n);
void initialize_bignum(bignum *n);
bignum string_to_bignum(char* string, bignum *result);
int max(int a, int b);
void zero_justify(bignum *n);
int compare_bignum(bignum *a, bignum *b);
void add_bignum(bignum *a, bignum *b, bignum *c);
void subtract_bignum(bignum *a, bignum *b, bignum *c);
void digit_shift(bignum *n, int d);
void multiply_bignum(bignum *a, bignum *b, bignum *c);
void divide_bignum(bignum *a, bignum *b, bignum *c);

int check_input_data(const char* input_string);
int getop(char* expression, char* s);
int check_last_operation();
void make_operation();
bignum calculate(char* expression);

int main() {

	char* input_string = (char *)malloc(sizeof(char) * BUFFER_SIZE);

	if (input_string == 0) {
		free(input_string);
		printf("[error]");
		return 0;
	}

	if (scanf("%1024s", input_string) != 1) {
		free(input_string);
		printf("[error]");
		return 0;
	}

	if (check_input_data(input_string) == 0) {
		free(input_string);
		printf("[error]");
		return 0;
	}

	bignum result = calculate(input_string);
	print_bignum(&result);

	free(input_string);
	return 0;
}


int check_input_data(const char* input_string) {
	int counter_of_brackets = 0;
	int flag_of_operations = 0;
	while (*input_string != '\0') {
		if ((*input_string < '0' || *input_string > '9') && *input_string != '+'
			&& *input_string != '-' && *input_string != '/' && *input_string != '*'
			&& *input_string != '(' && *input_string != ')') {
			return 0;
		}

		if ((*input_string == '+' || *input_string == '-' || *input_string == '/'
			|| *input_string == '*') && flag_of_operations == 1) {
			return 0;
		}
		if (*input_string == '+' || *input_string == '-' || *input_string == '/'
			|| *input_string == '*') {
			flag_of_operations = 1;
		}
		else {
			flag_of_operations = 0;
		}

		if (*input_string == '(') {
			counter_of_brackets++;
		}
		if (*input_string == ')') {
			counter_of_brackets--;
		}

		if (counter_of_brackets < 0) {
			return 0;
		}
		input_string++;
	}
	if (counter_of_brackets != 0) {
		return 0;
	}
	return 1;
}

int getop(char* expression, char* s) {
	int i = 0;
	static int c;
	while ((s[0] = c = expression[line_i++]) == ' ' || c == '\t');
	s[1] = '\0';
	if (!isdigit(c))
		return c;
	if (isdigit(c))
		while (isdigit(s[++i] = expression[line_i++]));
	s[i] = '\0';
	line_i--;

	return NUMBER;
}

int check_last_operation() {
	int last_element = pop_from_stack_of_operations();
	push_in_stack_of_operations(last_element);
	return last_element;
}

void make_operation() {
	bignum result;
	initialize_bignum(&result);
	bignum operand1;
	bignum operand2;
	initialize_bignum(&operand1);
	initialize_bignum(&operand2);
	if (check_last_operation() == '+') {
		operand1 = pop_from_stack_of_operands();
		operand2 = pop_from_stack_of_operands();
		add_bignum(&operand1, &operand2, &result);
		push_in_stack_of_operands(result);
	}
	if (check_last_operation() == '-') {
		operand2 = pop_from_stack_of_operands();
		operand1 = pop_from_stack_of_operands();
		subtract_bignum(&operand1, &operand2, &result);
		push_in_stack_of_operands(result);
	}
	if (check_last_operation() == '*') {
		operand1 = pop_from_stack_of_operands();
		operand2 = pop_from_stack_of_operands();
		multiply_bignum(&operand1, &operand2, &result);
		push_in_stack_of_operands(result);
	}
	if (check_last_operation() == '/') {
		operand2 = pop_from_stack_of_operands();
		operand1 = pop_from_stack_of_operands();
		divide_bignum(&operand1, &operand2, &result);
		push_in_stack_of_operands(result);
	}
	pop_from_stack_of_operations();
}


bignum calculate(char* expression) {
	bignum space;
	initialize_bignum(&space);
	int type = 0;
	char s[MAXOP];
	bignum result;
	while (expression != '\0') {
		while ((type = getop(expression, s)) != '\n') {
			switch (type) {
			case NUMBER:
				push_in_stack_of_operands(string_to_bignum(s, &result));
				break;
			case ENDOFTHEFILE:
				while (stack_pointer_of_operations != 0) {
					make_operation();
				}
				return pop_from_stack_of_operands();
			case '+':
				while (stack_pointer_of_operations != 0 && (check_last_operation() == '+'
					|| check_last_operation() == '-' || check_last_operation() == '*'
					|| check_last_operation() == '/')) {
					make_operation();
				}
				push_in_stack_of_operations('+');
				break;
			case '-':
				while (stack_pointer_of_operations != 0 && (check_last_operation() == '+'
					|| check_last_operation() == '-' || check_last_operation() == '*'
					|| check_last_operation() == '/')) {
					make_operation();
				}
				push_in_stack_of_operations('-');
				break;
			case '/':
				while (stack_pointer_of_operations != 0 && (check_last_operation() == '*'
					|| check_last_operation() == '/')) {
					make_operation();
				}
				push_in_stack_of_operations('/');
				break;
			case '*':
				while (stack_pointer_of_operations != 0 && (check_last_operation() == '*'
					|| check_last_operation() == '/')) {
					make_operation();
				}
				push_in_stack_of_operations('*');
				break;
			case '(':
				push_in_stack_of_operations('(');
				break;
			case ')':
				while (check_last_operation() != '(') {
					make_operation();
				}
				pop_from_stack_of_operations();
				break;
			default:
				break;
			}
		}
	}
	return space;
}

void push_in_stack_of_operands(bignum value) {
	if (stack_pointer_of_operands < STACK_SIZE)
		stack_of_operands[stack_pointer_of_operands++] = value;
}

bignum pop_from_stack_of_operands(void) {
	bignum space;
	initialize_bignum(&space);
	if (stack_pointer_of_operands > 0)
		return stack_of_operands[--stack_pointer_of_operands];
	else {
		return space;
	}
}

void push_in_stack_of_operations(int value) {
	if (stack_pointer_of_operations < STACK_SIZE)
		stack_of_operations[stack_pointer_of_operations++] = value;
	else
		printf("[error]");
}

int pop_from_stack_of_operations(void) {
	if (stack_pointer_of_operations > 0)
		return stack_of_operations[--stack_pointer_of_operations];
	else {
		printf("[error]");
		return 0;
	}
}

void print_bignum(bignum *n)
{
	int i;

	if (n->signbit == MINUS) printf("-");
	for (i = n->lastdigit; i >= 0; i--)
		printf("%c", '0' + n->digits[i]);

	printf("\n");
}

void int_to_bignum(int s, bignum *n)
{
	int i;
	int t;

	if (s >= 0) n->signbit = PLUS;
	else n->signbit = MINUS;

	for (i = 0; i < MAXDIGITS; i++) n->digits[i] = (char)0;

	n->lastdigit = -1;

	t = abs(s);

	while (t > 0) {
		n->lastdigit++;
		n->digits[n->lastdigit] = (t % 10);
		t = t / 10;
	}

	if (s == 0) n->lastdigit = 0;
}

void reverseStr(char* str) {
	int n = strlen(str);
	for (int i = 0; i < n / 2; i++) {
		char temp = str[i];
		str[i] = str[n - i - 1];
		str[n - i - 1] = temp;
	}

}

bignum string_to_bignum(char* string, bignum *result) {
	initialize_bignum(result);
	while (isspace(*string))
		string++;
	for (int i = 0; i < MAXDIGITS; i++) result->digits[i] = (char)0;
	if (*string == '-') {
		result->signbit = MINUS;
		string++;
	}
	else
		result->signbit = PLUS;
	result->lastdigit = -1;
	char tempString[MAXDIGITS];
	int counter = 0;
	for (size_t i = 0; i <= strlen(string); i++) {
		counter = i;
		if (string[i] >= '0' && string[i] <= '9');
		else break;
	}
	for (int i = 0; i < counter; i++) {
		tempString[i] = string[i];
	}
	tempString[counter] = '\0';
	reverseStr(tempString);
	for (int i = 0; i < counter; i++) {
		result->lastdigit++;
		result->digits[result->lastdigit] = tempString[i] - '0';
	}
	if (string == 0) result->lastdigit = 0;
	return *result;
}

void initialize_bignum(bignum *n)
{
	int_to_bignum(0, n);
}

int max(int a, int b)
{
	if (a > b) return(a); else return(b);
}


void zero_justify(bignum *n)
{
	while ((n->lastdigit > 0) && (n->digits[n->lastdigit] == 0))
		n->lastdigit--;

	if ((n->lastdigit == 0) && (n->digits[0] == 0))
		n->signbit = PLUS;
}

void subtract_bignum(bignum *a, bignum *b, bignum *c)
{
	int borrow;
	int v;
	int i;

	initialize_bignum(c);

	if ((a->signbit == MINUS) || (b->signbit == MINUS)) {
		b->signbit = -1 * b->signbit;
		add_bignum(a, b, c);
		b->signbit = -1 * b->signbit;
		return;
	}

	if (compare_bignum(a, b) == PLUS) {
		subtract_bignum(b, a, c);
		c->signbit = MINUS;
		return;
	}

	c->lastdigit = max(a->lastdigit, b->lastdigit);
	borrow = 0;

	for (i = 0; i <= (c->lastdigit); i++) {
		v = (a->digits[i] - borrow - b->digits[i]);
		if (a->digits[i] > 0)
			borrow = 0;
		if (v < 0) {
			v = v + 10;
			borrow = 1;
		}

		c->digits[i] = (char)v % 10;
	}

	zero_justify(c);
}

void add_bignum(bignum *a, bignum *b, bignum *c)
{
	int carry;
	int i;

	initialize_bignum(c);

	if (a->signbit == b->signbit) c->signbit = a->signbit;
	else {
		if (a->signbit == MINUS) {
			a->signbit = PLUS;
			subtract_bignum(b, a, c);
			a->signbit = MINUS;
		}
		else {
			b->signbit = PLUS;
			subtract_bignum(a, b, c);
			b->signbit = MINUS;
		}
		return;
	}

	c->lastdigit = max(a->lastdigit, b->lastdigit) + 1;
	carry = 0;

	for (i = 0; i <= (c->lastdigit); i++) {
		c->digits[i] = (char)(carry + a->digits[i] + b->digits[i]) % 10;
		carry = (carry + a->digits[i] + b->digits[i]) / 10;
	}

	zero_justify(c);
}

int compare_bignum(bignum *a, bignum *b)
{
	int i;

	if ((a->signbit == MINUS) && (b->signbit == PLUS)) return(PLUS);
	if ((a->signbit == PLUS) && (b->signbit == MINUS)) return(MINUS);

	if (b->lastdigit > a->lastdigit) return (PLUS * a->signbit);
	if (a->lastdigit > b->lastdigit) return (MINUS * a->signbit);

	for (i = a->lastdigit; i >= 0; i--) {
		if (a->digits[i] > b->digits[i]) return(MINUS * a->signbit);
		if (b->digits[i] > a->digits[i]) return(PLUS * a->signbit);
	}

	return(0);
}

void digit_shift(bignum *n, int d)
{
	int i;

	if ((n->lastdigit == 0) && (n->digits[0] == 0)) return;

	for (i = n->lastdigit; i >= 0; i--)
		n->digits[i + d] = n->digits[i];

	for (i = 0; i < d; i++) n->digits[i] = 0;

	n->lastdigit = n->lastdigit + d;
}

void multiply_bignum(bignum *a, bignum *b, bignum *c)
{
	bignum row;
	bignum tmp;
	int i, j;

	initialize_bignum(c);

	row = *a;

	for (i = 0; i <= b->lastdigit; i++) {
		for (j = 1; j <= b->digits[i]; j++) {
			add_bignum(c, &row, &tmp);
			*c = tmp;
		}
		digit_shift(&row, 1);
	}

	c->signbit = a->signbit * b->signbit;

	zero_justify(c);
}


void divide_bignum(bignum *a, bignum *b, bignum *c)
{
	bignum row;
	bignum tmp;
	int asign, bsign;
	int i;

	initialize_bignum(c);

	c->signbit = a->signbit * b->signbit;

	asign = a->signbit;
	bsign = b->signbit;

	a->signbit = PLUS;
	b->signbit = PLUS;

	initialize_bignum(&row);
	initialize_bignum(&tmp);

	c->lastdigit = a->lastdigit;

	for (i = a->lastdigit; i >= 0; i--) {
		digit_shift(&row, 1);
		row.digits[0] = a->digits[i];
		c->digits[i] = 0;
		while (compare_bignum(&row, b) != PLUS) {
			c->digits[i] ++;
			subtract_bignum(&row, b, &tmp);
			row = tmp;
		}
	}

	zero_justify(c);

	a->signbit = asign;
	b->signbit = bsign;
}