#ifndef PARSE_CIRC_H

#define PARSE_CIRC_H

# define STACK_MAX 5000

// Define a structure for the stack
typedef struct {
    // Array to store stack elements
    int arr[STACK_MAX];  
    // Index of the top element in the stack
    int top;        
} Stack;

void initialize(Stack *stack);

bool isEmpty(Stack *stack);

bool isFull(Stack *stack);

void push(Stack *stack, int value);

int pop(Stack *stack);

int peek(Stack *stack);


void create_erasure_file(float e_err, char *filename_in, char *filename_out, int* e_det_list);


void get_new_pauli_det_str(char *old_pauli_det_str, char *new_pauli_det_str, int meas_to_edet, int edet_to_meas, int meas_num);

void create_herald_line(float e_err, int *qubit_list, int num_qubits, char *herald_line);


void create_edet_line(Stack *edet_stack, char *edet_line, int abs_meas);



#endif 