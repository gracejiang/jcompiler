/*
*
*   TOKEN
*
*   This is the token class used to keep track of properties
*   for each token.
*
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "token.h"

// HELPER FUNCTION DECLARATIONS
int is_num(char * str);
int is_deci_num(char * str);
int is_hexi_num(char * str);
char* hexi_to_deci(char * str);
int get_num(char * str);
int is_arg(char * str);


// INITIALIZATION FUNCTION
// initialize token value
void initToken(token * tok, char* token_str) {

    // const
    if (is_num(token_str) == 1) {
        tok->command = CONST;
        tok->num = token_str;

        if (is_hexi_num(token_str)) {
            tok->num = hexi_to_deci(token_str);
        }
    }

    // function declaration
    else if (strcmp(token_str, "defun") == 0) {
		tok->command = DEFUN;
	}

    // return for a func
    else if (strcmp(token_str, "return") == 0) {
        tok->command_str = token_str;
        tok->command = RETURN;
    }

    // add
    else if (strcmp(token_str , "+") == 0) {
        tok->command = ADD;
	}

    // subtract
	else if (strcmp(token_str, "-") == 0) {
		tok->command = SUB;
	}

    // multiply
	else if (strcmp(token_str, "*") == 0) {
		tok->command = MUL;
	}
    
    // divide
	else if (strcmp(token_str, "/") == 0) {
		tok->command = DIV;
	}
    
    // mod
	else if (strcmp(token_str, "\%") == 0) {
		tok->command = MOD;
	}

    // and
	else if (strcmp(token_str, "and") == 0) {
		tok->command = AND;
	}
    
    // or
	else if (strcmp(token_str, "or") == 0) {
		tok->command = OR;
	}

    // not
	else if (strcmp(token_str, "not") == 0) {
		tok->command = NOT;
	} 

    // less than
    else if (strcmp(token_str, "lt") == 0) {
		tok->command = LT;
	} 

    // less than equal to
    else if (strcmp(token_str, "le") == 0) {
		tok->command = LE;
	} 

    // equal to
    else if (strcmp(token_str, "eq") == 0) {
		tok->command = EQ;
	} 

    // greater than equal to
    else if (strcmp(token_str, "ge") == 0) {
		tok->command = GE;
	} 

    // greater than
    else if (strcmp(token_str, "gt") == 0) {
		tok->command = GT;
	} 

    // drop
    else if (strcmp(token_str, "drop") == 0) {
		tok->command = DROP;
	}

    // dup
    else if (strcmp(token_str, "dup") == 0) {
		tok->command = DUP;
	}

    // swap
    else if (strcmp(token_str, "swap") == 0) {
		tok->command = SWAP;
	}

    // rot
    else if (strcmp(token_str, "rot") == 0) {
		tok->command = ROT;
	}

    // if
    else if (strcmp(token_str, "if") == 0) {
		tok->command = IF;
	}

    // else
    else if (strcmp(token_str, "else") == 0) {
		tok->command = ELSE;
	}

    // endif
    else if (strcmp(token_str, "endif") == 0) {
		tok->command = ENDIF;
	}

    // initializing a function or argument
    else {
        if (is_arg(token_str)) {
            tok->command = ARG;

            // set arg num

            // 1 digit
            if ((int)token_str[4] == 0) {
                tok->arg_num = (int)token_str[3] - 48;
            } 
            
            // 2 digit
            else {
                tok->arg_num = 10*(int)token_str[3] + (int)token_str[4];
            }
        }

        else {
            tok->command_str = token_str;
            tok->command = FUNC;
        }
    }

}


// HELPER FUNCTION DEFINITIONS

// checks if string is a number
int is_num(char * str) {
    return (is_deci_num(str) || is_hexi_num(str));
}

// checks if a string is a decimal number
int is_deci_num(char * str) {
    int i = 0;

    // not a deci int
    if ((int)str[0] > 57) {
        return 0;
    } 
    
    // not an int, -, or empty
    else if (((int)str[0] < 48) && ((int)str[0] != 45) && ((int)str[0] != 0)) {
        return 0;
    }

    // "-" case
    else if ((int)str[0] == 45 && (int)str[1] == 0) {
        return 0;
    }

    // check rest of elems
    for (i = 1; i < 100; i++) {
        if ((int)str[i] == 0) {
            return 1;
        } else if ((int)str[i] < 48 || (int)str[i] > 57) {
            return 0;
        }
    }
    return 1;
}

// checks if a string is a hexidecimal number
int is_hexi_num(char * str) {
    int i = 0;

    // first char is 0 and second char is x
    if ((int)str[0] != 48 || ((int)str[1] != 120)) {
        return 0;
    }

    // check rest of elems
    for (i = 2; i < 100; i++) {
        if ((int)str[i] == 0) {
            return 1;
        } else if ((int)str[0] > 70 || (int)str[0] < 48 || ((int)str[0] > 57 && (int)str[0] < 65)) {
            return 0;
        }
    }
    return 1;
}

// get deci num from hexi num
char* hexi_to_deci(char * str) {
    int sum = 0;
    char *num_as_str = malloc(16);
    int i = 0;
    int j = 0;
    int const_multi = 1;

    // convert each digit to deci by pow of 16
    for (i = 2; i <= 5; i++) {
        for (j = 0; j < (5 - i); j++) {
            const_multi = const_multi * 16;
        }
        
        if ((int)str[i] >= 48 && (int)str[i] <= 57) {
            sum = sum + ((int)str[i] - 48) * const_multi;
        } else if ((int)str[i] >= 65 && (int)str[i] <= 70) {
            sum = sum + ((int)str[i] - 55) * const_multi;
        }

        const_multi = 1;
    }
     
    sprintf(num_as_str, "%d", sum);
    return num_as_str;
}

// converts string to int assuming string is valid
int get_num(char * str) {
    int num = 0;
    char * converted_str;
    if (str[0] == (int)'-') {
        converted_str = strtok(str, "-");
        if (converted_str != NULL) {
            num = atoi(converted_str) * -1;
        }
    } else {
        num = atoi(str);
    }
    return num;
}

// checks if string is valid arg from arg1-arg20
int is_arg(char * str) {
    // a = 97, r = 114, g = 103
    if ((int)str[0] != 97 || (int)str[1] != 114 || (int)str[2] != 103) {
        return 0;
    } 

    // 1st dig must be between 1-9
    if ((int)str[3] >= 49 && (int)str[3] <= 57 && (int)str[4] == 0) {
        return 1;
    }

    // 0: 48
    // 9: 57
    // 1st dig must be between 1-2, 2nd dig must be between 0-0
    if (((int)str[5] == 0) && (((int)str[3] == 49 && (int)str[4] >= 48 && (int)str[4] <= 57) || ((int)str[3] == 50 && (int)str[4] == 48))) {
        return 1;
    }    

    return 0;
}
