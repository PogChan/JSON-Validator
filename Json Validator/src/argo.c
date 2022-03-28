#include <stdlib.h>
#include <stdio.h>

#include "argo.h"
#include "global.h"
#include "debug.h"

/**
 * @brief Checks for -c, -c -p, or -c -p (INDENT) using global_options
 * 
 * @return char 
 * -c = 'c'
 * -c -p = 'p'
 * -c -p (INDENT) = 'i'
 */
char cpiFlag (){
    int tempGlob = global_options;

    int cp = 0;
    int c = 0;
    int cpi = 0;
    
    tempGlob &= ~255; //removes indent if there was one
    if(tempGlob == (CANONICALIZE_OPTION | PRETTY_PRINT_OPTION)){ //if -c -p
        tempGlob = global_options; //reset.
        tempGlob &= ~(CANONICALIZE_OPTION | PRETTY_PRINT_OPTION); //TAke away -c, -p to see if its indented

        if(tempGlob == 0){//no indent , -c, -p
          cp = 1;

        }else if(tempGlob> 0){ //-c, -p (INDENT)

            cpi = 1;
        }

    }else if(tempGlob == (CANONICALIZE_OPTION)){ //-c
        c = 1;
    }

    if(c == 1){
        return 'c';
    }else if(cp ==1){
        return 'p'; 
    }else if(cpi == 1){
        return 'i';
    }

    return 'c';
}

int indent() {
    int tempGlob = global_options;
    tempGlob &= ~(CANONICALIZE_OPTION | PRETTY_PRINT_OPTION);
    return tempGlob;
}

ARGO_VALUE* getEmptyArgVal(){
    if(argo_next_value < NUM_ARGO_VALUES){
        return &argo_value_storage[argo_next_value];
    }else{
        argo_next_value = 0;  
        return &argo_value_storage[argo_next_value];
    }
    return &argo_value_storage[argo_next_value];
}

int argo_read_array(ARGO_ARRAY *s, FILE *f){

    char c= fgetc(f);
    int memberCounter =0;
    while(argo_is_whitespace(c)){ //remove all white space
            c = fgetc(f);
    }

    ARGO_VALUE *tempVal = argo_read_value(f);
    if(tempVal == NULL){
        fprintf(stderr, "REad argo value is empty");
        return -1;
    }
    s->element_list->next= tempVal;
    s->element_list = s->element_list->next;
    while(argo_is_whitespace(c)){ //remove all white space
            c = fgetc(f);
    }
    if(c == ARGO_COMMA){
        if(argo_read_array(s, f)==0){
            
        }else {
              fprintf(stderr, "REad array failed ");
        return -1;
        }
    }else{
        s->element_list->next->type= ARGO_NO_TYPE;
        return 0; 
    }




    if(s == NULL){
    fprintf(stderr, "nothing was read");
        return -1;
    }

    fprintf(stderr, "failed to read");
    return -1;
}

int argo_read_object(ARGO_OBJECT *s, FILE *f){
    ARGO_STRING name;
    char c= fgetc(f);
    int memberCounter =0;
    while(argo_is_whitespace(c)){ //remove all white space
            c = fgetc(f);
    }
    if(c == ARGO_QUOTE){
        
        if(argo_read_string(&name, f)==0){
        } else{
            fprintf(stderr, "failed to read object name");
            return -1;
        }
    }else{
            fprintf(stderr, "name not available");
            return -1;
    }

    //READ THE ACTUAL VALUE TO PUT INTO THE CONTENTS.

    ARGO_VALUE *tempVal = argo_read_value(f);
    if(tempVal == NULL){
        fprintf(stderr, "REad argo value is empty");
        return -1;
    }
    s->member_list->next= tempVal;
    s->member_list->next->name = name;
    s->member_list = s->member_list->next;
    while(argo_is_whitespace(c)){ //remove all white space
            c = fgetc(f);
    }
    if(c == ARGO_COMMA){
        if(argo_read_object(s, f)==0){
            
        }else {
              fprintf(stderr, "REad objectfailed ");
        return -1;
        }
    }else{
    s->member_list->next->type= ARGO_NO_TYPE;
        return 0; 
    }




    if(s == NULL){
    fprintf(stderr, "nothing was read");
        return -1;
    }

    fprintf(stderr, "failed to read");
    return -1;
}

/**
 * @brief  Read JSON input from a specified input stream, parse it,
 * and return a data structure representing the corresponding value.
 * @details  This function reads a sequence of 8-bit bytes from
 * a specified input stream and attempts to parse it as a JSON value,
 * according to the JSON syntax standard.  If the input can be
 * successfully parsed, then a pointer to a data structure representing
 * the corresponding value is returned.  See the assignment handout for
 * information on the JSON syntax standard and how parsing can be
 * accomplished.  As discussed in the assignment handout, the returned
 * pointer must be to one of the elements of the argo_value_storage
 * array that is defined in the const.h header file.
 * In case of an error (these include failure of the input to conform
 * to the JSON standard, premature EOF on the input stream, as well as
 * other I/O errors), a one-line error message is output to standard error
 * and a NULL pointer value is returned.
 *
 * @param f  Input stream from which JSON is to be read.
 * @return  A valid pointer if the operation is completely successful,
 * NULL if there is any error.
 */

ARGO_VALUE *argo_read_value(FILE *f) {
    char c = fgetc(f);
    ARGO_OBJECT* tempObj = NULL;
    ARGO_ARRAY* tempArr= NULL;
    ARGO_STRING* tempStr = NULL;
    ARGO_VALUE* result = NULL;
    ARGO_NUMBER *tempNum = NULL;
    if(c != EOF){ //if not eof

    //ok so now that we now its not eof. we need to see any indicators of a json strcuture. if its not present. then we open a while loop
    //that just keeps loopping untill we find one and while we not finding we just keep popping off random fluff in our way.
        if(argo_is_whitespace(c) && c != ARGO_QUOTE && c != ARGO_LBRACE && c !=ARGO_LBRACK
     && !argo_is_digit(c) && c != 'f' && c!= 't' && c!= 'n'){
        while (argo_is_whitespace(c)&& c != ARGO_QUOTE && c != ARGO_LBRACE && c !=ARGO_LBRACK
     && !argo_is_digit(c)  && c != 'f' && c!= 't' && c!= 'n')
        {
            c = fgetc(f); //keep popping till we see one of these
        }
     }else if(argo_is_whitespace(c)|| c == ARGO_QUOTE || c == ARGO_LBRACE || c ==ARGO_LBRACK
     || !argo_is_digit(c) || c == 'f' || c== 't' || c== 'n'){        
        if(c == ARGO_LBRACE){//object
            if(argo_read_object(tempObj, f)==0){
                result->type = ARGO_OBJECT_TYPE;
                if(tempObj==NULL){
                    fprintf(stderr, "failed to read object");
               return NULL;
                }
                result->content.object = *tempObj; 
            }else{
                fprintf(stderr, "failed to read object");
                return NULL;
            }
            return result;
        }else if(c == ARGO_LBRACK){//array
            if(argo_read_array(tempArr, f)==0){
                result->type = ARGO_ARRAY_TYPE;
                if(tempArr==NULL){
                    fprintf(stderr, "failed to read array");
                return NULL;
                }

                result->content.array = *tempArr; 

            }else{
                fprintf(stderr, "failed to read array");
                return NULL;
            }
            return result;
        }else if(c == ARGO_QUOTE){//string
            
            if(argo_read_string(tempStr, f)==0){
                result->type = ARGO_STRING_TYPE;
                if(tempStr==NULL){
                    fprintf(stderr, "failed to read string");
                return NULL;
                }

                result->content.string = *tempStr; 
            }else{
                fprintf(stderr, "failed to read string");
                return NULL;
            }
return result;
        }else if(c == 't' || c == 'f' || c == 'n'){//basic
result->type = ARGO_BASIC_TYPE;
            if(c=='t'){
                result->content.basic = ARGO_TRUE;
            }else if(c =='f'){
                result->content.basic = ARGO_FALSE;
            }else if(c =='n'){
                result->content.basic = ARGO_NULL;
            }
            return result;
        }else if(argo_is_digit(c) && ungetc(c,f) ==c){ //digit
            if(argo_read_number(tempNum, f) ==0 ){
                result->type = ARGO_NUMBER_TYPE;
                result->content.number= *tempNum;
            }else {
                fprintf(stderr, "failed to read number");
                return NULL;
            }
        }
     }
     argo_next_value++;
     return getEmptyArgVal();
    }else{
        fprintf(stderr, "EOF");
    }

    return result;
}

/**
 * @brief  Read JSON input from a specified input stream, attempt to
 * parse it as a JSON string literal, and return a data structure
 * representing the corresponding string.
 * @details  This function reads a sequence of 8-bit bytes from
 * a specified input stream and attempts to parse it as a JSON string
 * literal, according to the JSON syntax standard.  If the input can be
 * successfully parsed, then a pointer to a data structure representing
 * the corresponding value is returned.
 * In case of an error (these include failure of the input to conform
 * to the JSON standard, premature EOF on the input stream, as well as
 * other I/O errors), a one-line error message is output to standard error
 * and a NULL pointer value is returned.
 *
 * @param f  Input stream from which JSON is to be read.
 * @return  Zero if the operation is completely successful,
 * nonzero if there is any error.
 */

int argo_read_string(ARGO_STRING *s, FILE *f) {
    char c = fgetc(f);
    int length = 0;
    while(c != ARGO_QUOTE){
        if(c == ARGO_BS||c == ARGO_FF ||c == ARGO_LF||c == ARGO_CR||c == ARGO_HT){
            if(argo_append_char(s, c) != 0){
            fprintf(stderr, "failed appending character to string");
            return -1;
            }
        }else if(c == ARGO_BSLASH){
            c = fgetc(f);
            if(c == 'u'){
                c = fgetc(f);
                long finalHex =0;
                long cint =0;

                if(argo_is_digit(c)){
                    cint=c-48;
                }else if(((c) >= 'A' && (c) <= 'F')){
                    cint = c- 55;
                }else if(((c) >= 'a' && (c) <= 'f')){
                    cint = c -87;
                }

                cint = cint*16*16*16;
                finalHex+=cint;

                c=fgetc(f);
                cint=0;
                if(argo_is_digit(c)){
                    cint=c-48;
                }else if(((c) >= 'A' && (c) <= 'F')){
                    cint = c- 55;
                }else if(((c) >= 'a' && (c) <= 'f')){
                    cint = c -87;
                }

                cint = cint*16*16;
                finalHex+=cint;

                c=fgetc(f);
                cint=0;
                if(argo_is_digit(c)){
                    cint=c-48;
                }else if(((c) >= 'A' && (c) <= 'F')){
                    cint = c- 55;
                }else if(((c) >= 'a' && (c) <= 'f')){
                    cint = c -87;
                }

                cint = cint*16;
                finalHex+=cint;
                
                c=fgetc(f);
                cint=0;
                if(argo_is_digit(c)){
                    cint=c-48;
                }else if(((c) >= 'A' && (c) <= 'F')){
                    cint = c- 55;
                }else if(((c) >= 'a' && (c) <= 'f')){
                    cint = c -87;
                }
                finalHex+=cint;
                if(finalHex)
                 if(argo_append_char(s, finalHex) != 0){
                        fprintf(stderr, "failed appending character to string");
                        return -1;
                    }
            }
        }
        else if(argo_append_char(s, c) != 0){
            fprintf(stderr, "failed appending character to string");
            return -1;
        }
    }
    c=fgetc(f);//gets rid of the clsoing quote.
    fprintf(stderr, "failed reading string");
    return -1;
}




/**
 * @brief  Read JSON input from a specified input stream, attempt to
 * parse it as a JSON number, and return a data structure representing
 * the corresponding number.
 * @details  This function reads a sequence of 8-bit bytes from
 * a specified input stream and attempts to parse it as a JSON numeric
 * literal, according to the JSON syntax standard.  If the input can be
 * successfully parsed, then a pointer to a data structure representing
 * the corresponding value is returned.  The returned value must contain
 * (1) a string consisting of the actual sequence of characters read from
 * the input stream; (2) a floating point representation of the corresponding
 * value; and (3) an integer representation of the corresponding value,
 * in case the input literal did not contain any fraction or exponent parts.
 * In case of an error (these include failure of the input to conform
 * to the JSON standard, premature EOF on the input stream, as well as
 * other I/O errors), a one-line error message is output to standard error
 * and a NULL pointer value is returned.
 *
 * @param f  Input stream from which JSON is to be read.
 * @return  Zero if the operation is completely successful,
 * nonzero if there is any error.
 */

int argo_read_number(ARGO_NUMBER *n, FILE *f) {
    int c = fgetc(f);
    ARGO_STRING* strInt = NULL;
    double intRep;
    long exp;
    double random = 1.0;
    int x = 0;
    double floatRep = 0;
    
    int y = 0;
    y+=0;
    int isExponent = 0; //false at first 
    while(argo_is_digit(c)==0){
        argo_append_char(strInt, c);
        if(x == 0){
        intRep += c-48;
        x++;
        }else{
            intRep*=10;
        intRep += c-48;
        
        }
        
        c =fgetc(f);
    }
    if(c == ARGO_PERIOD){
        n->valid_float=1;
        y =strInt->length;
        while (!argo_is_exponent(c) && argo_is_whitespace(c))
        {
            argo_append_char(strInt, c);
            floatRep += c-48;
            c= fgetc(f);
        }
        if(argo_is_exponent(c)){
        
         }else{

         }
    }
    


    
    if(n->valid_float ==0){
        n->int_value = intRep;
    }
    n->string_value = *strInt;

    return 0;
}


int argo_write_object(ARGO_VALUE *v, FILE *f){
    int cpi = cpiFlag();
    if(v->type == ARGO_NO_TYPE){
        return 0;
    }
    if(&(v->name) == NULL){
        fprintf(stderr, "object name is null");
return -1;
    }
    if(argo_write_string(&(v->name), f)==0){ //"name"
        fputc(ARGO_COLON, f); //"name": (argo values)
        

        if(cpi == 'i' || cpi=='p'){
            fputc(ARGO_SPACE, f);
        }
        if(argo_write_value(v, f)==0){ //ok now we need to print the actual argo value, throw it to write_argo val
            if(v->next->type != ARGO_NO_TYPE){
                fputc(ARGO_COMMA, f);
                if(cpi == 'i' || cpi=='p'){
                    fputc(ARGO_LF, f);
                    for(int i =0; i< indent_level * indent(); i++){
                        fputc(ARGO_SPACE, f);
                    }
                }
                if(argo_write_object(v->next, f) == 0){
                     return 0;
                }else{
                    fprintf(stderr, "failed to write");
return -1;
                }
            }else{
                return 0;
            }
        }else{
            fprintf(stderr, "failed to write");
return -1;
        }
    }else{
            fprintf(stderr, "failed to write");
return -1;
    }
    return 0;
}

int argo_write_array(ARGO_VALUE *v, FILE *f){
    int cpi = cpiFlag();
    if(v->type == ARGO_NO_TYPE){
        return 0;
    }
    if(argo_write_value(v, f)==0){
        if(v->next->type != ARGO_NO_TYPE){
        fputc(ARGO_COMMA, f);
        if(cpi == 'i' || cpi=='p'){
            fputc(ARGO_LF, f);
            for(int i =0; i< indent_level * indent(); i++){
                fputc(ARGO_SPACE, f);
            }
        }
        if(argo_write_array(v->next, f)==0){
            return 0;
        }else{
            fprintf(stderr, "failed to write");
        }
    }else{
        return 0;
    }
    }else{
        fprintf(stderr, "failed to write");
return -1;
    }
    return -1;
}





/**
 * @brief  Write canonical JSON representing a specified value to
 * a specified output stream.
 * @details  Write canonical JSON representing a specified value
 * to specified output stream.  See the assignment document for a
 * detailed discussion of the data structure and what is meant by
 * canonical JSON.
 *
 * @param v  Data structure representing a value.
 * @param f  Output stream to which JSON is to be written.
 * @return  Zero if the operation is completely successful,
 * nonzero if there is any error.
 */

int argo_write_value(ARGO_VALUE *v, FILE *f) {
    //fprintf("this is start of write val: %d\n", indent_level);
    int x = indent();
        //fprintf("this is the start of write val:%d\n", x);
    char cpi = cpiFlag();
    if(v == NULL){
        fprintf(stderr, "input argo value is null");
return -1;
    }
    if((v->type) == ARGO_OBJECT_TYPE){
        fputc(ARGO_LBRACE, f);
        indent_level++;

        //fprintf("%d\n", x);

        //fprintf("%d\n", indent_level);
        if(cpi == 'i' || cpi == 'p'){
             fputc(ARGO_LF, f);
             for(int i =0; i< indent_level * indent(); i++){
                 fputc(ARGO_SPACE, f);
             }
         }
         
        if(argo_write_object(v->content.object.member_list->next, f)==0){
            indent_level--;
            //fprintf("right before termination: %d\n", indent_level);
            fputc(ARGO_LF, f);
            if(cpi == 'i' || cpi == 'p'){
             for(int i =0; i< indent_level * indent(); i++){
                 fputc(ARGO_SPACE, f);
             }
            }
            fputc(ARGO_RBRACE, f);
            return 0;
        }else{
            fprintf(stderr, "failed to write");
return -1;
        }
    }else if((v->type) == ARGO_ARRAY_TYPE){
        fputc(ARGO_LBRACK, f);
        indent_level++;
        if(cpi == 'i' || cpi == 'p'){
             fputc(ARGO_LF, f);
             for(int i =0; i< indent_level * indent(); i++){
                 fputc(ARGO_SPACE, f);
             }
        }

        ARGO_VALUE *arrayContent = v->content.object.member_list;
        if(argo_write_array(arrayContent->next, f)==0){
            indent_level--;
            if(cpi == 'i' || cpi == 'p'){
                 fputc(ARGO_LF, f);
             for(int i =0; i< indent_level * indent(); i++){
                 fputc(ARGO_SPACE, f);
             }
            }
            
            fputc(ARGO_RBRACK, f);
            return 0;
        }else{
            fprintf(stderr, "failed to write");
return -1;
        }
        
    }else if((v->type) == ARGO_BASIC_TYPE){
        if(v->content.basic ==ARGO_NULL){
            fprintf(f, ARGO_NULL_TOKEN);
        }else if(v->content.basic ==ARGO_FALSE){
            fprintf(f, ARGO_FALSE_TOKEN);
        }else if(v->content.basic ==ARGO_TRUE){
            fprintf(f, ARGO_TRUE_TOKEN);
        }
        return 0;
    }else if((v->type) == ARGO_NUMBER_TYPE){
        if(argo_write_number(&(v->content.number), f) == 0){
            return 0;
        }else{
            fprintf(stderr, "failed to write");
return -1;
        }
    }else if((v->type) == ARGO_STRING_TYPE){
            if(argo_write_string(&(v->content.string), f) == 0){
                return 0;
            }else{
            fprintf(stderr, "failed to write");
return -1;
            }
        }
    fprintf(stderr, "failed to write");
return -1;
}


/**
 * @brief  Write canonical JSON representing a specified string
 * to a specified output stream.
 * @details  Write canonical JSON representing a specified string
 * to specified output stream.  See the assignment document for a
 * detailed discussion of the data structure and what is meant by
 * canonical JSON.  The argument string may contain any sequence of
 * Unicode code points and the output is a JSON string literal,
 * represented using only 8-bit bytes.  Therefore, any Unicode code
 * with a value greater than or equal to U+00FF cannot appear directly
 * in the output and must be represented by an escape sequence.
 * There are other requirements on the use of escape sequences;
 * see the assignment handout for details.
 *
 * @param v  Data structure representing a string (a sequence of
 * Unicode code points).
 * @param f  Output stream to which JSON is to be written.
 * @return  Zero if the operation is completely successful,
 * nonzero if there is any error.
 */

int argo_write_string(ARGO_STRING *s, FILE *f) {
    ARGO_CHAR *inputChar = (s -> content);

    if(inputChar == NULL){
        fputc(ARGO_QUOTE, f);
        fputc(ARGO_QUOTE, f);
        return 0;
    }


    fputc(ARGO_QUOTE, f);
    for(int i = 0; i < s->length; i++){

            char firstByte;
            char secondByte;
            char thirdByte;
            char fourthByte;

            int tempInputChar = *(inputChar+i);
            if(tempInputChar=='\0'){
                fputc(ARGO_QUOTE, f);
                return 0;
            }
            tempInputChar = tempInputChar >> 12;
            firstByte = tempInputChar;
            if(firstByte >9 ){
                firstByte+=48;
            }else{
                firstByte+=87;
            }
            tempInputChar = *(inputChar+i);
            tempInputChar = tempInputChar >> 8;
            secondByte = tempInputChar & 15; //this only keeps the first byte and removes the rest
            if(secondByte >9 ){
                secondByte+=48;
            }else{
                secondByte +=87;
            }
            

            tempInputChar = *(inputChar+i);
            tempInputChar = tempInputChar >> 4;
            thirdByte = tempInputChar & 15;
            if(thirdByte >9 ){
                thirdByte+=48;
            }else{
                thirdByte +=87;
            }

            tempInputChar = *(inputChar+i);
            fourthByte = tempInputChar & 15;
            if(fourthByte >9 ){
                fourthByte+=48;
            }else{
                fourthByte +=87;
            }
        if((*(inputChar+i) == 0x005C)){
            fputc(0x005C, f);
            fputc(ARGO_BSLASH,f);
        }else if(*(inputChar+i) == 0x0022){
            fputc(0x005C,f);
            fputc(ARGO_QUOTE,f);
        }else if((*(inputChar+i) == ARGO_FSLASH) ||( (*(inputChar+i) > 0x001F) && (*(inputChar+i) < 0x00FF))){ // U+001F < x < U+00FF (NO \, ", YES /)
            fputc(*(inputChar+i), f);
        }
        else if(*(inputChar+i) > 0x00FF){ // check if creater than U+00FF and print '/u' before the hex
           fputc(ARGO_BSLASH, f);
           fputc(ARGO_U, f);
            fputc(firstByte, f); //turns into lower case. 
            fputc((secondByte), f); 
           fputc((thirdByte), f);
           fputc(fourthByte, f);
        }
        
        
     else if(argo_is_control(*(inputChar+i))){
            
            switch (*(inputChar+i)){
                case ARGO_BS:
                    fputc(ARGO_BSLASH, f);
                    fputc(ARGO_B, f);
                    break;
                case ARGO_FF:
                    fputc(ARGO_BSLASH, f);
                    fputc(ARGO_F, f);
                    break;
                case ARGO_LF:
                    fputc(ARGO_BSLASH, f);
                    fputc(ARGO_N, f);
                    break;
                case ARGO_CR:
                    fputc(ARGO_BSLASH, f);
                    fputc(ARGO_R, f);
                    break;
                case ARGO_HT:
                    fputc(ARGO_BSLASH, f);
                    fputc(ARGO_T, f);
                    break;
                default:
                    fputc(ARGO_BSLASH, f);
                    fputc(ARGO_U, f);
                    fputc(firstByte, f); //turns into lower case. 
                    fputc((secondByte), f); 
                    fputc((thirdByte), f);
                    fputc(fourthByte, f);
                    break;
            }
        }
    }
    fputc(ARGO_QUOTE, f);
    return 0;
}


/**
 * @brief  Write canonical JSON representing a specified number
 * to a specified output stream.
 * @details  Write canonical JSON representing a specified number
 * to specified output stream.  See the assignment document for a
 * detailed discussion of the data structure and what is meant by
 * canonical JSON.  The argument number may contain representations
 * of the number as any or all of: string conforming to the
 * specification for a JSON number (but not necessarily canonical),
 * integer value, or floating point value.  This function should
 * be able to work properly regardless of which subset of these
 * representations is present.
 *
 * @param v  Data structure representing a number.
 * @param f  Output stream to which JSON is to be written.
 * @return  Zero if the operation is completely successful,
 * nonzero if there is any error.
 */

int argo_write_number(ARGO_NUMBER *n, FILE *f) {
    //&x->content.number
    ARGO_CHAR argo_digits[ARGO_MAX_DIGITS];
    int negNum = 0; //0 for positive number, 1 for negativeNUm
    if((n->valid_int) != 0){ //if is valid int
        long tempInt = n->int_value;
        int icounter =0;
        if(tempInt == 0){
            fputc(48, f);
        }
        if(tempInt < 0){
            negNum = 1;
            tempInt *= -1;
        }
        while(tempInt > 0){
            
            int intraModded = tempInt%10;
            argo_digits[icounter] = intraModded + 48;
            tempInt /= 10;
            icounter++;
        }
        if(negNum==1){
            fputc(ARGO_MINUS,f);
        }
        for(int i = icounter-1; i >= 0; i--){
            fputc(argo_digits[i], f);
        }
        return 0;
    }else if((n->valid_float) != 0){
        int exponent = 0;
        int expIsNeg = 0; //0 = (+) 1 = (-)
        double tempFloat = n->float_value;
        if(tempFloat == 0.0){
            fputc(48, f);
            fputc(ARGO_PERIOD, f);
            fputc(48, f);
            return 0;
        }

        if(tempFloat < 0){ //if the number is negative.
            negNum = 1;
            tempFloat *= -1;
        }
        if(tempFloat >= 1.0){
            expIsNeg = 0;
            while (tempFloat >= 1.0){
                tempFloat /= 10;
                exponent++;
            }
        }else if(tempFloat < 0.1){
            expIsNeg = 1;
            while (tempFloat < 0.1)
            {
                tempFloat *= 10;
                exponent++;
            }
        }

    for (int i = 0; i < ARGO_PRECISION; i++)
    {
        tempFloat*=10;

    }

long intTempFloat = (long)tempFloat;

int jcounter= 0;
while(intTempFloat > 0){
    long intraModded = intTempFloat % 10;

    argo_digits[jcounter] = intraModded + 48;
    intTempFloat /= 10;
    jcounter++;
}

if(negNum==1){
    fputc(ARGO_MINUS,f);
}
fputc(48, f); //(+/-)0
fputc(ARGO_PERIOD, f);//0.
for(int i = jcounter -1; i >= 0; i--){
    fputc(argo_digits[i], f);
}
fputc(ARGO_E, f); //0.(dec)e
if(expIsNeg == 1){
    fputc(ARGO_MINUS, f); //0.(dec)e-
}



ARGO_CHAR argo_digits[ARGO_MAX_DIGITS]; //0.(dec)e(-/+)(exponent)
int gcounter =0;
while (exponent >0)
{
    int intraModded = exponent%10;
    argo_digits[gcounter] = intraModded + 48;
    exponent /= 10;
    gcounter++;
}
for(int i = gcounter -1; i >= 0; i--){
    fputc(argo_digits[i], f);
}
        //gotta put exponent in but thats not just a single digit.
        
        return 0;
    }
    fprintf(stderr, "failed to write");
return -1;
}
