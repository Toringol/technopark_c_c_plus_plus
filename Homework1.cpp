/*
*	Шепелев Сергей АПО-11 
*	Составить программу для перевода чисел из системы счисления по основанию P 
*	в систему счисления по основанию Q, где 2 меньше или равно Q меньше P меньше или равно 36.
*	Значения P, Q и строковое представление S числа в исходной
*	системе счисления вводятся с клавиатуры в следующем порядке: P Q S
*
*	S не выходит за границы size_t.
*
*	Процедура перевода не может использовать соответствующей функции
*	системной библиотеки и должна быть оформлена в виде отдельной функции,
*	на вход которой подаются значения P, Q и S, размещенные в динамической памяти.
*	На выход функция должна возвращать строковое представление S1 числа в целевой системе счисления.
*
*	Программа должна уметь обрабатывать ошибки во входных данных.
*	В случае возникновения ошибки нужно вывести в поток стандартного вывода сообщение
*	"[error]" (без кавычек) и завершить выполнение программы.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024

int flag_of_checking_register_of_symbols = 0;
char S1[BUFFER_SIZE] = "";

size_t power(size_t number, size_t degree);
char* reverse(char* line);
int character_to_integer(char temp_character);

int check_input_mistakes(size_t P, size_t Q, const char* S);
void free_memory(size_t *P, size_t *Q, char* S);
size_t any_num_sys_to_dec(const char* S, size_t P);
void dec_to_target_num_sys(size_t number, size_t Q);
char* any_num_sys_to_any(size_t P, size_t Q, const char* S);


int main() {

	size_t *P = (size_t *)malloc(sizeof(size_t));
	size_t *Q = (size_t *)malloc(sizeof(size_t));
	char *S = (char *)malloc(sizeof(char) * BUFFER_SIZE);

	if (scanf("%zd%zd%1024s", P, Q, S) != 3 || (*P == 0 || *Q == 0 || *S == 0)) {
		free_memory(P, Q, S);
		printf("[error]");
		return 0;
	}

	if (check_input_mistakes(*P, *Q, S) == -1) {
		free_memory(P, Q, S);
		return 0;
	}

	printf("%s", any_num_sys_to_any(*P, *Q, S));

	free_memory(P, Q, S);

	return 0;
}


char* reverse(char* line) {
	char tempCharacter;
	for (int i = 0; i <= (strlen(line) - 1) / 2; i++) {
		tempCharacter = line[i];
		line[i] = line[strlen(line) - 1 - i];
		line[strlen(line) - 1 - i] = tempCharacter;
	}
	return line;
}


int character_to_integer(char temp_character) {
	if (temp_character >= '0' && temp_character <= '9') {
		return (temp_character - 48);
	}
	if (temp_character >= 'A' && temp_character <= 'Z') {
		flag_of_checking_register_of_symbols = 0;
		return (temp_character - 55);
	}
	if (temp_character >= 'a' && temp_character <= 'z') {
		flag_of_checking_register_of_symbols = 1;
		return (temp_character - 87);
	}
	else {
		printf("[error]");
		return 0;
	}
}

size_t power(size_t number, size_t degree) {
	size_t result = 1;
	size_t number_in_degree_of_2 = number;
	while (degree > 0) {
		if ((degree & 1) == 1) {
			result *= number_in_degree_of_2;
		}
		number_in_degree_of_2 *= number_in_degree_of_2;
		degree = degree >> 1;
	}
	return result;
}

size_t any_num_sys_to_dec(const char* S, size_t P) {
	size_t number_in_dec = 0;
	size_t counter = 0;
	for (int i = strlen(S) - 1; i >= 0; i--, counter++) {
		if (character_to_integer(S[i]) == 0) {
			continue;
		}
		number_in_dec += character_to_integer(S[i]) * power(P, counter);
	}
	return number_in_dec;
}

void dec_to_target_num_sys(size_t number, size_t Q) {
	static const char *k_base_high_symbols = "0123456789ABCDEFGHIJKLMNOPQRASUVWXYZ";
	static const char *k_base_low_symbols = "0123456789abcdefghijklmnopqrasuvwxyz";
	size_t i = 0;
	if (flag_of_checking_register_of_symbols == 0) {
		do {
			if ((number % Q) >= 10) {
				S1[i++] = k_base_high_symbols[number % Q];
			}
			else {
				S1[i++] = number % Q + '0';
			}
		} while ((number /= Q) > 0);
	}
	else {
		do {
			if ((number % Q) >= 10) {
				S1[i++] = k_base_low_symbols[number % Q];
			}
			else {
				S1[i++] = number % Q + '0';
			}
		} while ((number /= Q) > 0);
	}
	S1[i] = '\0';
}

char* any_num_sys_to_any(size_t P, size_t Q, const char* S) {
	size_t temp = 0;
	if (P != 10) {
		temp = any_num_sys_to_dec(S, P);
	}
	else {
		temp = atoi(S);
	}
	dec_to_target_num_sys(temp, Q);
	reverse(S1);
	return S1;
}

void free_memory(size_t *P, size_t *Q, char* S) {
	free(P);
	free(Q);
	free(S);
}

int check_input_mistakes(size_t P, size_t Q, const char* S) {
	if (P == 0 || Q == 0 || S == 0) {
		printf("[error]");
		return -1;
	}
	if (Q < 2 || Q >= P || P > 36) {
		printf("[error]");
		return -1;
	}
	if (P == 10) {
		int i = 0;
		while (S[i] != '\0') {
			if (S[i] < '0' || S[i] > '9') {
				printf("[error]");
				return -1;
			}
			i++;
		}
	}
	int i = 0;
	while (S[i] != '\0') {
		if (!((S[i] >= '0' && S[i] <= '9') ||
			(S[i] >= 'a' && S[i] <= 'z') ||
			(S[i] >= 'A' && S[i] <= 'Z'))) {
			printf("[error]");
			return -1;
		}
		i++;
	}
	return 0;
}
