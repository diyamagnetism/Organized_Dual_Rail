// This program is a state machine that reads in a stim circuit file and adds erasure detectors 

# include <stdio.h>
# include <stdbool.h>
# include <ctype.h>
# include <string.h>
#include "parse_circ.h"



# define MAX_LENGTH 1000
# define QUBIT_MAX 500
# define STACK_MAX 5000

enum state {TOP, QUBIT, PAULI_DET, TICK, CX, MEAS, HERALD_EM, HERALD_ME, E_DET, ERROR};


// Function to initialize the stack
void initialize(Stack *stack) {
    // Set top index to -1 to indicate an empty stack
    stack->top = -1;  
}

// Function to check if the stack is empty
bool isEmpty(Stack *stack) {
    // If top is -1, the stack is empty
    return stack->top == -1;  
}

// Function to check if the stack is full
bool isFull(Stack *stack) {
    // If top is MAX_SIZE - 1, the stack is full
    return stack->top == STACK_MAX - 1;  
}

// Function to push an element onto the stack
void push(Stack *stack, int value) {
    // Check for stack overflow
    if (isFull(stack)) {
        printf("Stack Overflow\n");
        return;
    }
    // Increment top and add the value to the top of the stack
    stack->arr[++stack->top] = value;
    printf("Pushed %d onto the stack\n", value);
}

// Function to pop an element from the stack
int pop(Stack *stack) {
    // Check for stack underflow
    if (isEmpty(stack)) {
        printf("Stack Underflow\n");
        return -1;
    }
    // Return the top element 
    int popped = stack->arr[stack->top];
    // decrement top pointer
    stack->top--;
    printf("Popped %d from the stack\n", popped);
    // return the popped element
    return popped;
}

// Function to peek the top element of the stack
int peek(Stack *stack) {
    // Check if the stack is empty
    if (isEmpty(stack)) {
        printf("Stack is empty\n");
        return -1;
    }
    // Return the top element without removing it
    return stack->arr[stack->top];
}

void put_and_reset(FILE* file_out, char *newline);

int get_meas_count(char *line);

void create_herald_line(float e_err, int *qubit_list, int num_qubits, char *herald_line) {
	strcpy(herald_line, "HERALDED_ERASURE (");

	char num_buff[20];

    // add erasure error to herald line
    sprintf(num_buff, "%f)", e_err);
	size_t n = strlen(num_buff);
	strncat(herald_line, num_buff, n);
	num_buff[0] = "\0";

	// add qubit numbers to herald line
	for (size_t i = 0; i < num_qubits; i++) {
		sprintf(num_buff, " %d", qubit_list[i]);
		n = strlen(num_buff);
		strncat(herald_line, num_buff, n);
		num_buff[0] = "\0";
	}

}

void create_edet_line(Stack *edet_stack, char *edet_line, int abs_meas) {
	strcpy(edet_line, "DETECTOR rec[-");

	char num_buff[20];

	// plus one is so that the rec integer starts at 1 rather than 0
	int curr_rec = (abs_meas + 1) - pop(edet_stack);

    // add erasure error to herald line
    sprintf(num_buff, "%d]", curr_rec);
	size_t n = strlen(num_buff);
	strncat(edet_line, num_buff, n);
	num_buff[0] = "\0";

}


void get_new_pauli_det_str(char *old_pauli_det_str, char *new_pauli_det_str, int meas_to_edet, int edet_to_meas, int meas_num) {
	
	char stringy_int[5];

	int rec_list[6];
	int new_rec_list[6];
	int digit_list[6];

	int i_list[6];
	int ch_rec_list[100];

	int old_str_len = strlen(old_pauli_det_str);
	int rec_i = 0;
	bool record_state = false;
	
	// int rec_len = len(rec_list);

	for (size_t i = 0; i < old_str_len; i++) {
		

		// will record positive integers, not the negative sign!
		if (record_state==true) {
			stringy_int[i] = old_pauli_det_str[i];
			i_list[rec_i] = i;
			if (strcmp(old_pauli_det_str[i], ']') == 0) {
				rec_list[rec_i] = atoi(stringy_int);
				rec_i++;
				record_state = false;
			}
		}

		if (strcmp(old_pauli_det_str[i], '-') == 0) {
			record_state = true;
		}
		
	}

	int rec_len = rec_i;
	

	if (rec_len == 1) {
		new_rec_list[0] = rec_list[0] + meas_to_edet;
	}
	else if (rec_len == 2) {
		new_rec_list[0] = rec_list[0] + meas_to_edet;
		new_rec_list[1] = rec_list[1] + meas_to_edet + meas_num + edet_to_meas;
	}
	else {
		for (size_t i = 0; i < rec_len - 1 ; i++) {
			rec_list[i] = rec_list[i] + meas_to_edet;
		}
		new_rec_list[rec_len - 1] = rec_list[rec_len - 1] + meas_to_edet + meas_num + edet_to_meas;
	}

	int exp = 0;
	int num_digits = 0;
	for (size_t i = 0 ; i < rec_len; i++) {
		while (rec_list[i] < 10^(exp)) {
			exp++;
		}
		num_digits = exp + 1;
		digit_list[i] = num_digits;
	}


	strcpy(ch_rec_list, itoa(new_rec_list));

	int new_i = 0;
	char ch;
	int rec_i = 0;
	int ch_i = 0;

	for (size_t i = 0; i < old_str_len; i++) {
		strcpy(ch, old_pauli_det_str[i]);

		if (record_state == true) {
			for (size_t d = 0; d < digit_list[rec_i] + 1; d++) {
				new_pauli_det_str[new_i] = ch_rec_list[ch_i];
				new_i++;
				ch_i++;
			}
			record_state = false;
		}
		
		new_pauli_det_str[new_i] = ch;

		if (strcmp(old_pauli_det_str[i], '-') == 0) {
			record_state = true;
		}
		new_i++;
	}
}

int get_meas_count(char *line) {
	int meas_num = 0;
	while (strtok(line, ' ') != NULL) {
		meas_num++;
	}

	return meas_num;

}

void put_and_reset(FILE* file_out, char *newline) {
	fputs(newline, file_out);
	newline[0] = '\0';
}


void create_erasure_file(float e_err, char *filename_in, char *filename_out, const int* e_det_list, int e_det_list_len) {

	FILE* file_in = fopen(filename_in, 'r');
	FILE* file_out = fopen(filename_out, 'w');
	
	// initial state is QUBIT
	enum state curr = QUBIT;

	
    char line[MAX_LENGTH];
	char newline[MAX_LENGTH];
	char herald_line[MAX_LENGTH];

    int qubit_count = 0;
	int meas_to_edet, edet_to_meas, abs_meas, meas_num = 0;
	int cx_count = 0;

	int qubit_list[QUBIT_MAX];

	Stack *edet_stack;
	initialize(edet_stack);


	while (fgets(line, MAX_LENGTH, file_in ) != NULL) 
	{
		switch (curr)

		{
			
			case TOP: 
			
				if (strstr(line, "QUBIT") != NULL)
				{
					curr = QUBIT;
					qubit_list[0] = atoi(line[-1]);
					qubit_count++;
					strcpy(newline, line);

				}
				else if (strstr(line, "DETECTOR") != NULL)
				{
					curr = PAULI_DET;
					strcpy(newline, line);
					get_new_pauli_det_str(line, newline, meas_to_edet, edet_to_meas, meas_num);

				}
				else if (strstr(line, "TICK") != NULL)
				{
					curr = HERALD_EM;
					strcpy(newline, line);

				}
				else if (strstr(line, "CX") != NULL)
				{
					curr = CX;
					strcpy(newline, line);

				}
				else if (strstr(line, "M") != NULL)
				{
					strcpy(newline, line);
					meas_num = get_meas_count(line);
					abs_meas += meas_num;
					if (contains(5, e_det_list, e_det_list_len) == true){
						curr = HERALD_ME;
					}
					else {
						curr = TOP;	
					}

				}
				else
				{
					strcpy(newline, line);
				}
				
				put_and_reset(file_out, newline);
				
				break;
			
			
			case QUBIT: 

				if (strstr(line, "QUBIT") == NULL)
				{
					curr = TOP;
					// convert qubit list into a string with herald erasure, err, and qubits
					create_herald_line(e_err,  qubit_list, qubit_count, herald_line);
					strcpy(newline, line);
					put_and_reset(file_out, newline);
					break;

				}

				
				qubit_list[qubit_count] = atoi(line[-1]);
				qubit_count++;
				strcpy(newline, line);
				put_and_reset(file_out, newline);
				break;
			
			case PAULI_DET: 

				if (strstr(line, "DETECTOR") == NULL)
				{
					curr = TOP;
					strcpy(newline, line);
					put_and_reset(file_out, newline);
					break;

				}

				get_new_pauli_det_str(line, newline, meas_to_edet, edet_to_meas, meas_num);
				put_and_reset(file_out, newline);
				break;
				
				
				

			case CX: 
				cx_count = (cx_count + 1) % 4;
		
				if (contains(cx_count, e_det_list, e_det_list_len) == true){
					curr = HERALD_EM;
				}
				else {
					curr = TOP;
				}

				strcpy(newline, line);
				put_and_reset(file_out, newline);

				break;

			

			case HERALD_EM:
				curr = TOP;
				strcpy(newline, line);
				put_and_reset(file_out, newline);


				for (size_t i = 0; i < qubit_count; i++) {
					push(edet_stack, (abs_meas + i));
				}
				edet_to_meas += qubit_count;
				abs_meas+= qubit_count;
				

				break;
			

			
			case HERALD_ME: 
				curr = TOP;
				strcpy(newline, line);
				put_and_reset(file_out, newline);

				for (size_t i = 0; i < qubit_count; i++) {
					push(edet_stack, (abs_meas + i));
				}
				edet_to_meas += qubit_count;
				abs_meas+= qubit_count;
				
				break;
			
			case E_DET: 
				char edet_line[MAX_LENGTH];
				while (isEmpty(edet_stack) != true) {
					create_edet_line(edet_stack, edet_line, abs_meas);
					fputs(edet_line, file_out);
					edet_line[0] = '\0';
				}
				curr = TOP;

				break;
			
			case ERROR:
				printf("Oh no! This file is invalid :( )");
			break;
			
		}
	}
}

