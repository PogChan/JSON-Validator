#include <stdlib.h>

#include "argo.h"
#include "global.h"
#include "debug.h"


/**
 * @brief Validates command line arguments passed to the program.
 * @details This function will validate all the arguments passed to the
 * program, returning 0 if validation succeeds and -1 if validation fails.
 * Upon successful return, the various options that were specified will be
 * encoded in the global variable 'global_options', where it will be
 * accessible elsewhere in the program.  For details of the required
 * encoding, see the assignment handout.
 *
 * @param argc The number of arguments passed to the program from the CLI.
 * @param argv The argument strings passed to the program from the CLI.
 * @return 0 if validation succeeds and -1 if validation fails.
 * @modifies global variable "global_options" to contain an encoded representation
 * of the selected program options.
 */

int validargs(int argc, char **argv) {
//USAGE: bin/argo [-h] [-c|-v] [-p|-p INDENT]

    if(argc == 1) { //DECIDE whether or not to add this
    fprintf(stderr, "no arguments specified");
        return -1; //empty 
    }
    global_options = global_options & 0;

    if((*(*(argv+1)))== ARGO_MINUS && (*(*(argv+1)+1)) == 'h'){ //we found the help header.
        global_options  =  HELP_OPTION; //sets the most signifcant bit to 1
        return 0; //and just terminate since we found the -h flag
    }else if(((*(*(argv+1)))== ARGO_MINUS) && ((*(*(argv+1)+1)) == 'c')){//bin/argo -c ...
       
        if(argc == 2){
            global_options  = (global_options | CANONICALIZE_OPTION); //ok set to have -c enabled
            return 0;
        }else if(((*(*(argv+2)))== ARGO_MINUS) && ((*(*(argv+2)+1)) == 'p') && (argc == 3)){ //  bin/argo -c -p and thats it?
            global_options  = (global_options | PRETTY_PRINT_OPTION);  //enable -p
            global_options  = (global_options | CANONICALIZE_OPTION); //ok set to have -c enabled
            global_options  = (global_options | 4);
            return 0;
        }else if(((*(*(argv+2)))== ARGO_MINUS) && ((*(*(argv+2)+1)) == 'p') && (argc == 4)){ //  bin/argo -c -p (INDENT)?
        if(argo_is_digit(*(*(argv+3)))){ //if the first char is an integer then we have to put it 
            int intCounter = 0;
            for(int i = 0; i < 3; i++){
                if(argo_is_digit(*(*(argv+3) + i))){
                    intCounter++;
                }else if(*(*(argv+3) + i)==0){
                    break;
                }else{ //since its not a digit, nor null temrination then its something else so we return error.
                    fprintf(stderr, "input isn't an object");
                    return -1;
                }
            }
    
            int indent =0;
            for(int i = 0; i< intCounter; i++){
                indent += (*(*(argv+3)+i) - 48);
                indent *= 10;
            }
            indent /= 10;
            if((indent  >= 0 ) && (indent <= 255)){
                global_options  = (global_options | indent);
                global_options  = (global_options | PRETTY_PRINT_OPTION);  //enable -p
                global_options  = (global_options | CANONICALIZE_OPTION); //ok set to have -c enabled
                return 0;
            }else{
                fprintf(stderr, "-c -p indent is overflow");
                return -1;
            }
        }
        }
    }else if((*(*(argv+1))== ARGO_MINUS) && (*(*(argv+1)+1) == 'v') && (argc == 2)){ // bin/argo -v ...
        global_options  = (global_options | VALIDATE_OPTION);
        return 0;
    }
    fprintf(stderr, "invalid arguments");
    return -1;
}

