#include <stdio.h>
#include <stdlib.h>

#include "argo.h"
#include "global.h"
#include "debug.h"

#ifdef _STRING_H
#error "Do not #include <string.h>. You will get a ZERO."
#endif

#ifdef _STRINGS_H
#error "Do not #include <strings.h>. You will get a ZERO."
#endif

#ifdef _CTYPE_H
#error "Do not #include <ctype.h>. You will get a ZERO."
#endif

int main(int argc, char **argv)
{
    int passOrNah = validargs(argc, argv);

    if(global_options == HELP_OPTION){
        USAGE(*argv, EXIT_SUCCESS);
    }
    if(passOrNah== -1){
        USAGE(*argv, EXIT_FAILURE);
    }

    ARGO_VALUE *readValue = argo_read_value(stdin);
    if(readValue == NULL){
        USAGE(*argv, EXIT_FAILURE);
    }else{
        if(argo_write_value(readValue, stdout) ==0){
            return EXIT_SUCCESS;
        }else{
            return EXIT_FAILURE;
        }
    }

    /*
    if(global_options == VALIDATE_OPTION){ //-v 
        
    }else if(global_options == CANONICALIZE_OPTION ){ //-c

    }else if(global_options ==(CANONICALIZE_OPTION | PRETTY_PRINT_OPTION)){ //-c -p

    }else if(global_options < (CANONICALIZE_OPTION | PRETTY_PRINT_OPTION | 0x100)){ //-c -p INDENT 8 bits.

    }
    */
    
    // TO BE IMPLEMENTED
    return EXIT_FAILURE;
}

/*
 * Just a reminder: All non-main functions should
 * be in another file not named main.c
 */
