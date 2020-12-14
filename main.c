/*
*
*   MAIN
*
*   This is the main function. It reads in a j file, tokenizes the input,
*   and processes each token.
*
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "asm_writer.h"
#include "stack.h"


// FUNCTION INITIALIZATIONS
void processLine(FILE * asm_file, char curr_line[1024], stack_list * if_stack, stack_list * else_stack); // process a line


// MAIN FUNCTION
int main(int argc, char *argv[]) {
    if (argc > 1) {

        // get argument as file name
        char * jfile_name = argv[1];
        FILE * jfile_pointer = NULL;
        char *curr_line = malloc(8192);
        
        // try opening file
        if ((jfile_pointer = fopen(jfile_name, "r")) == NULL) {
            printf("Error opening file %s. Are you sure this is a valid path?\n", jfile_name);
            exit(1);
        }

        // make asm file
        char * asmfile_name = jfile_name;
        int asmfile_name_size = strlen(argv[1]);
        asmfile_name[asmfile_name_size - 2] = '\0';
        asmfile_name = strcat(asmfile_name, ".asm\0");
        FILE * asmfile_pointer = fopen(asmfile_name, "w+");

        // set up asm file
        setup_local_mem(asmfile_pointer);

        // set up if stack
        stack_list * if_stack = (stack_list *) malloc(sizeof(stack_list));
        if_stack->head = NULL;
        if_stack->tail = NULL;
        if_stack->size = 0;

        // set up else stack
        stack_list * else_stack = (stack_list *) malloc(sizeof(stack_list));
        else_stack->head = NULL;
        else_stack->tail = NULL;
        else_stack->size = 0;

        // do command for each line
        while (fgets(curr_line, 256, jfile_pointer) != NULL) {
            processLine(asmfile_pointer, curr_line, if_stack, else_stack);
        }
        
        // close files
        fclose(jfile_pointer);
        fclose(asmfile_pointer);
    }
}

// FUNCTION DEFINITIONS

// process a line
void processLine(FILE * asm_file, char curr_line[1024], stack_list * if_stack, stack_list * else_stack) {

    // converting input to token strings
    char * token_str = malloc(8192);
    token * tok = malloc(sizeof(token));
    tok->command = -1;
    token_str = strtok(curr_line, " \t\n\r");

    // convert each token string to a token and write the corresponding
    // asm command to the asm file
    while (token_str != NULL && token_str[0] != ';') {

        // initialize token from token_str
        initToken(tok, token_str);
        tok->command_str = token_str;

        // write asm command from token
        writeAsmCommand(asm_file, tok, if_stack, else_stack);

        // get next token in line
        token_str = strtok(NULL, " \t\n\r");
    }

}