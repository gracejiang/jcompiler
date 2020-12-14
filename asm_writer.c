/*
*
*   ASM_WRITER
*
*   This file reads in a token, converts it to assembly, and writes it 
*   into the given assembly file.
*
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "token.h"
#include "stack.h"


// VARIABLES
// next val 
// 0: normal
// 1: function name
// 2: potential argument
int next_val = 0; // handling functions

// if counter
int if_counter = 1; // number of ifs
int comparison_counter = 1; // number of comparisons


// HELPER FUNCTION INITIALIZATIONS
void setup_local_mem(FILE * asm_file); // setting up asm file
void write_prologue(FILE * asm_file); // setting up prologue for functions
void write_epilogue(FILE * asm_file); // setting up epilogue for functions
void push_to_top(FILE * asm_file, char* push_value); // push a value to top of stack
void write_comparison_end(FILE * asm_file); // write second part of comparison (after branching)
void write_if(FILE * asm_file, stack_list * if_stack); // write an if statement
void write_else_endif(FILE * asm_file, stack_list * if_stack, stack_list * else_stack, int command); // write an else or endif statement

// REGISTERS
// R1: first num off stack & result of operations
// R2: second num off stack
// R3: consts, and occasionally 3rd num off stack (in rot)
// R5: call funcs stack, bottom of frame xd
// R6: nums stack
// R7: rv

// convert j token to asm script and write into asm file
void writeAsmCommand(FILE * asm_file, token * tok, stack_list * if_stack, stack_list * else_stack) {

    // ASSEMBLY INSTRUCTIONS

    // altering stack
	char *pop_one = "\tLDR R1, R6, #0\n\tADD R6, R6, #1\n";
    char *pop_two = "\tLDR R1, R6, #0\n\tADD R6, R6, #1\n\tLDR R2, R6, #0\n\tADD R6, R6, #1\n";
    char *push_result_top = "\tADD R6, R6, #-1\n\tSTR R1, R6, #0\n";

    // arithmetic operators
    char *add = "\tADD R1, R1, R2\n";
    char *sub = "\tSUB R1, R1, R2\n";
    char *mul = "\tMUL R1, R1, R2\n";
    char *div = "\tDIV R1, R1, R2\n";
    char *mod = "\tMOD R1, R1, R2\n";

    // logical operators
    char *and = "\tAND R1, R1, R2\n";
    char *or = "\tOR R1, R1, R2\n";
    char *not = "\tNOT R1, R1\n";

    // stack operations
    char *swap = "\
        \tLDR R1, R6, #0\n\
        \tADD R6, R6, #1\n\
        \tLDR R2, R6, #0\n\
        \tSTR R1, R6, #0\n\
        \tADD R6, R6, #-1\n\
        \tSTR R2, R6, #0\n";
    char *rot = "\
        \tLDR R1, R6, #0\n\
        \tADD R6, R6, #1\n\
        \tLDR R2, R6, #0\n\
        \tADD R6, R6, #1\n\
        \tLDR R3, R6, #0\n\
        \tSTR R2, R6, #0\n\
        \tADD R6, R6, #-1\n\
        \tSTR R1, R6, #0\n\
        \tADD R6, R6, #-1\n\
        \tSTR R3, R6, #0\n";
    char *drop = "\tADD R6, R6, #1\n";
    char *dup = "\
        \tLDR R1, R6, #0\n\
        \tADD R6, R6, #-1\n\
        \tSTR R1, R6, #0\n";

    // comparison operators
    char *less_than = "\tCMP R1, R2\n\tBRzp ";
    char *less_equal = "\tCMP R1, R2\n\tBRp ";
    char *equal = "\tCMP R1, R2\n\tBRnp ";
    char *greater_equal = "\tCMP R1, R2\n\tBRn ";
    char *greater_than = "\tCMP R1, R2\n\tBRnz ";
    

    // functions (call stack) –> arguments
    char *load_arg1 = "\tLDR R1, R5, #";
    char *load_arg2 = "\n\tSTR R1, R6, #-1\n\tADD R6, R6, #-1\n";

    // misc
    char *num_to_str = malloc(4);

    // initializing a function
    if (next_val == 1) {
        fputs(tok->command_str, asm_file);
        fputs("\n", asm_file);
        write_prologue(asm_file);
        next_val = 0;
        return;
    } 
    
    // any other command
    switch(tok->command) {

        // define const
        case CONST:
            if (tok->num != NULL) {
                push_to_top(asm_file, tok->num);
            } else {
                // should never hit here
                printf("Command Str: %s\n", tok->command_str);
                printf("Error, NULL token number even though const declared.\n");
            }
            break;

        // add
        case ADD:
            fputs(pop_two, asm_file);
            fputs(add, asm_file);
            fputs(push_result_top, asm_file);
            break;

        // subtract
        case SUB:
            fputs(pop_two, asm_file);
            fputs(sub, asm_file);
            fputs(push_result_top, asm_file);
            break;

        // multiply
        case MUL:
            fputs(pop_two, asm_file);
            fputs(mul, asm_file);
            fputs(push_result_top, asm_file);
            break;

        // divide
        case DIV:
            fputs(pop_two, asm_file);
            fputs(div, asm_file);
            fputs(push_result_top, asm_file);
            break;

        // modula
        case MOD:
            fputs(pop_two, asm_file);
            fputs(mod, asm_file);
            fputs(push_result_top, asm_file);
            break;

        // and
        case AND:
            fputs(pop_two, asm_file);
            fputs(and, asm_file);
            fputs(push_result_top, asm_file);
            break;

        // or
        case OR:
            fputs(pop_two, asm_file);
            fputs(or, asm_file);
            fputs(push_result_top, asm_file);
            break;

        // not
        case NOT:
            fputs(pop_one, asm_file);
            fputs(not, asm_file);
            fputs(push_result_top, asm_file);
            break;

        // less than
        case LT:
            fputs(pop_two, asm_file);
            fputs(less_than, asm_file);
            write_comparison_end(asm_file);
            break;

        // less than equal to
        case LE:
            fputs(pop_two, asm_file);
            fputs(less_equal, asm_file);
            write_comparison_end(asm_file);
            break;

        // equal to
        case EQ:
            fputs(pop_two, asm_file);
            fputs(equal, asm_file);
            write_comparison_end(asm_file);
            break;

        // greater than equal to
        case GE:
            fputs(pop_two, asm_file);
            fputs(greater_equal, asm_file);
            write_comparison_end(asm_file);
            break;

        // greater than
        case GT:
            fputs(pop_two, asm_file);
            fputs(greater_than, asm_file);
            write_comparison_end(asm_file);
            break;

        // "defun" function definer
        case DEFUN:
            next_val = 1;
            break;

        // returning a function
        case RETURN:
            write_epilogue(asm_file);
            fputs("RET\n", asm_file);
            break;

        // calling a function
        case FUNC:
            fputs("\tJSR ", asm_file);
            fputs(tok->command_str, asm_file);
            fputs("\n", asm_file);
            break;

        // drop
        case DROP:
            fputs(drop, asm_file);
            break;

        // duplicate
        case DUP:
            fputs(dup, asm_file);
            break;

        // swap
        case SWAP:
            fputs(swap, asm_file);
            break;

        // rotate
        case ROT:
            fputs(rot, asm_file);
            break;

        // if
        case IF:
            write_if(asm_file, if_stack);
            break;

        // else
        case ELSE:
            write_else_endif(asm_file, if_stack, else_stack, ELSE);
            break;

        // endif
        case ENDIF:
            write_else_endif(asm_file, if_stack, else_stack, ENDIF);
            break;

        // use argument inside function
        case ARG:        
            fputs(load_arg1, asm_file);
            int new_arg_num = tok->arg_num + 2;
            sprintf(num_to_str, "%d", new_arg_num);
            fputs(num_to_str, asm_file);
            fputs(load_arg2, asm_file);
            break;

        // invalid token
        case INVALID:
            // should not reach here
            printf("Your j-file has an invalid token: %s\n", tok->command_str);
            break;
    }
    
}

// HELPER FUNCTION DEFINITIONS

// setup where local memory for R6 is
void setup_local_mem(FILE * asm_file) {
    char* local_mem = ".CODE\n.FALIGN\n\n";
    fputs(local_mem, asm_file);
}

// func prologue
void write_prologue(FILE * asm_file) {
    char *prologue = "\tADD R6, R6, #-3\n\tSTR R7, R6, #1\n\tSTR R5, R6, #0\n\tADD R5, R6, #0\n";
    fputs(prologue, asm_file);
}

// func epilogue
void write_epilogue(FILE * asm_file) {
    char *epilogue = "\tLDR R7, R6, #0\n\tSTR R7, R5, #2\n\tADD R6, R5, #0\n\tLDR R5, R6, #0\n\tLDR R7, R6, #1\n\tADD R6, R6, #3\n\tADD R6, R6, #-1\n";
    fputs(epilogue, asm_file);
}

// push inputted value to top of stack
void push_to_top(FILE * asm_file, char* push_str) {
    int push_value = 0;

    // check if in bounds
    push_value = atoi(push_str);

    char *define_const = "\tCONST R3, #";
    char *define_hiconst = "\tHICONST R3, #";
    char *push_to_stack = "\tADD R6, R6, #-1\n\tSTR R3, R6, #0\n";

    // no shifting needed
    if (push_value >= -255 && push_value <= 256) {
        // put num into r3
        fputs(define_const, asm_file);
        fputs(push_str, asm_file);
        fputs("\n", asm_file);

        // pushing to stack
        fputs(push_to_stack, asm_file);
    }

    // shifting needed
    else {	
        // shift bits & convert
        int lower_8_bits = push_value & 0xFF;
        int upper_8_bits = (push_value >> 8) & 0xFF;
        char* lower_8_bits_str = malloc(1000 * sizeof(char));
        char* upper_8_bits_str = malloc(1000 * sizeof(char));
        sprintf(lower_8_bits_str , "%d" , lower_8_bits);
        sprintf(upper_8_bits_str , "%d" , upper_8_bits);

        // put num into r5
        fputs(define_const, asm_file);
        fputs(lower_8_bits_str, asm_file);
        fputs("\n", asm_file);

        fputs(define_hiconst, asm_file);
        fputs(upper_8_bits_str, asm_file);
        fputs("\n", asm_file);

        // pushing to stack
        fputs(push_to_stack, asm_file);
    }
   
}

// comparisons after branching (lt, le, eq, ge, gt)
void write_comparison_end(FILE * asm_file) {
    char * num_to_str = malloc(128);
    char * compare_end_1 = "\n\tCONST R1, #1\n\tADD R6, R6, #-1\n\tSTR R1, R6, #0\n\tJMP ";
    char * compare_end_2 = "\n\tCONST R1, #0\n\tADD R6, R6, #-1\n\tSTR R1, R6, #0\n";
    
    fputs("COMPARE", asm_file);
    sprintf(num_to_str, "%d", comparison_counter);
    fputs(num_to_str, asm_file);
    fputs(compare_end_1, asm_file);

    fputs("ENDCOMPARE", asm_file);
    fputs(num_to_str, asm_file);
    fputs("\n", asm_file);

    fputs("COMPARE", asm_file);
    fputs(num_to_str, asm_file);
    fputs(compare_end_2, asm_file);

    fputs("ENDCOMPARE", asm_file);
    fputs(num_to_str, asm_file);
    fputs("\n", asm_file);

    comparison_counter = comparison_counter + 1;
}

// write an if statement
void write_if(FILE * asm_file, stack_list * if_stack) {

    char * num_to_str = malloc(128);
    sprintf(num_to_str, "%d", if_counter);

    // get top element
    fputs("\tLDR R1, R6, #0\n\tADD R6, R6, #1\n", asm_file);

    // check if true
    fputs("\tCONST R2, #0\n", asm_file);
    fputs("\tCMP R1, R2\n", asm_file);

    // jump to else if not true
    fputs("\tBRz ELSE", asm_file);
    fputs(num_to_str, asm_file);
    fputs("\n", asm_file);

    // update stack
    push_stack(if_stack, if_counter);

    // update counter
    if_counter = if_counter + 1;
}

// write an else or endif statement
void write_else_endif(FILE * asm_file, stack_list * if_stack, stack_list * else_stack, int command) {
    // ELSE command: 22
    // ENDIF command: 23

    char * num_to_str = malloc(128);
    int num = get_stack(if_stack);
    sprintf(num_to_str, "%d", num);

    // else command
    if (command == 22) {
        // end to endif at end of if portion
        fputs("\tJMP ENDIF", asm_file);
        fputs(num_to_str, asm_file);
        fputs("\n", asm_file);

        // start else portion
        fputs("ELSE", asm_file);
        fputs(num_to_str, asm_file);
        fputs("\n", asm_file);

        // update else stack
        push_stack(else_stack, num);

        // rest of else function
        return;
    }

    // endif command
    // else already been seen
    if (in_stack(else_stack, num)) {
        // add end if
        fputs("ENDIF", asm_file);
        fputs(num_to_str, asm_file);
        fputs("\n", asm_file);


        // update if and else stacks
        pop_stack(if_stack);
        pop_stack(else_stack);
    } 
    
    // else has not been seen
    else {
        // add else
        fputs("ELSE", asm_file);
        fputs(num_to_str, asm_file);
        fputs("\n", asm_file);

        // add end if
        fputs("ENDIF", asm_file);
        fputs(num_to_str, asm_file);
        fputs("\n", asm_file);

        // update if stack
        pop_stack(if_stack);
    }
}