#include "lex.h"


int recover() {
  printf("Dummy recover \n");
  return 0;
}

int isdelimiter(char c) {
  if(c=='='||c==';'||c==','||c=='['||c==']'||c=='('||c==')'||c=='+'||c=='*'||c=='-'||c=='/'||c=='%'||c=='<'||c=='>'||c==':')
    return 1;
  else return 0;
}

void get_token() {
  char lexeme_buf[80],*lexeme;
  int lexeme_len=0;
  int state = 1;
  char c;
  int flag =1;
  while(flag) {
    switch (state) { /* assuming that the state is 1 while it starts*/
    case 1:
      c = fgetc(gl_prog_stream);/*from the input stream*/
      if(c == EOF) {printf("Reached End Of File\n"); flag=0;break;}
      if(c== ' ' || c=='\t') {
	state=2;
      }
      else if(c=='\n'){
	gl_lineno++;
	state=2;
      }
	
      else {
	ungetc(c,gl_prog_stream);
	state = 4;
	break;
      }

    case 2:
      c = fgetc(gl_prog_stream);
      if(c == EOF) {printf("Reached End Of File\n"); flag=0;break;}
      if(c==' '|| c=='\t') state = 2;
      else if(c=='\n') {gl_lineno++; state=2;}
      else state = 3;
      break;
      
    case 3:
      ungetc(c,gl_prog_stream);/*push 'letter' or 'digit'  back*/
      state = 4;
      break;
      
    case 4:
      c = fgetc(gl_prog_stream);
      if(c == EOF) {printf("Reached End Of File\n"); flag=0;break;}
      //printf("\n%c",c);
      
      if(isalpha(c)){
	lexeme_buf[lexeme_len++] = c; 
	state = 5;
      }
      
      else {
	ungetc(c,gl_prog_stream);
	state = 7;
      }
      break;
      
    case 5:
      c = fgetc(gl_prog_stream);
      if(c == EOF) {printf("Reached End Of File\n"); flag=0;break;}
      //printf("%c",c);
      if(isalpha(c)) {
	lexeme_buf[lexeme_len++] = c; 
	state = 5;
      }
      else {
	  if(isspace(c)||isdelimiter(c)) 
	state = 6;
      else //error
	state = 100;
      }
      break;
      
    case 6:
      ungetc(c,gl_prog_stream);/*put the space back*/
      state = 1;
      lexeme_buf[lexeme_len++] = '\0';
      //printf("\nJust accepted an ID %s , %d\n",lexeme_buf,strlen(lexeme_buf));
      lexeme = (char*)malloc(strlen(lexeme_buf));
      strcpy(lexeme,lexeme_buf);
       // if reading statements, just lookup a token 
      //generate an error if it is not found
      if(gl_reading_stmts == 1){
	gl_nexttoken = lookup(lexeme);
	if(gl_nexttoken == NULL){
	  fprintf(gl_error_log,"Line:%d: undefined identifier \"%s\"\nUndeclared identifiers will be reported only once\n",gl_lineno+1,lexeme);
	  printf("Line:%d: undefined identifier \"%s\"\n",gl_lineno+1,lexeme);
	  gl_nerrors++;
	  //gl_nexttoken = insert(lexeme,ID);//insert anyway because I don't 
	  // want to change the parser in any way. Note that..I am 
	  // incrementing nerrors. So the symboltable and quad table will
	  // not be printed to output files
	}
      }
      //else in declarations ... so I have to insert.
      gl_nexttoken = insert(lexeme,ID);     
      flag=0;
      break;
          
    case 7:
      c = fgetc(gl_prog_stream);
      if(c == EOF) {printf("Reached End Of File\n"); flag=0;break;}
      //printf("\n%c",c);
      
      if(isdigit(c)){
	lexeme_buf[lexeme_len++] = c; 
	state = 8;
      } 
      
      else {
	ungetc(c,gl_prog_stream);
	state = 10;
      } 
      break;
      
    case 8:
      c = fgetc(gl_prog_stream);
      if(c == EOF) {printf("Reached End Of File\n"); flag=0;break;}
      //printf("%c",c);
      if(isdigit(c)) {
	lexeme_buf[lexeme_len++] = c;
	state = 8;
      } 
      else if(isspace(c)||isdelimiter(c)) 
	state = 9;
      else 
	state = 100;
      break;
      
    case 9:
      ungetc(c,gl_prog_stream);/*put the space or delimiter back*/
      state = 1;
      lexeme_buf[lexeme_len++] = '\0';
      lexeme = (char*)malloc(strlen(lexeme_buf));
      strcpy(lexeme,lexeme_buf);
     
      gl_nexttoken = insert(lexeme,CONS);
      flag=0;
      break;
      
    case 10:
      c = fgetc(gl_prog_stream);
      if(c == EOF) {printf("Reached End Of File\n"); flag=0;break;}
      //printf("\n%c",c);
      if(isdelimiter(c)) {
	state = 11;
      }
      else {
	ungetc(c,gl_prog_stream);
	state = 100; /* the error state*/
      }
      break;
      
    case 11:
      if(c == '<'){
	lexeme_buf[lexeme_len++] = c; 
	state = 12;
      }
      else if(c == '>'||c==':') {
	lexeme_buf[lexeme_len++] = c; 
	state = 16;
      }
      //takes care of comments
      else if(c == '/') {
	lexeme_buf[lexeme_len++] = c;
	state = 17;
      }
      else if(c=='='||c==';'||c==','||c=='['||c==']'||c=='('||c==')'||c=='+'||c=='*'||c=='-'||c=='/'||c=='%') {
	lexeme_buf[lexeme_len++] = c;
	state = 1;
	lexeme_buf[lexeme_len++] = '\0';
	lexeme = (char*)malloc(strlen(lexeme_buf));
	strcpy(lexeme,lexeme_buf);
	gl_nexttoken = lookup(lexeme);
	flag=0;
      }
      else {
	ungetc(c,gl_prog_stream);
	state = 100; /*Will not get here */
      }
      break;
      
    case 12:
      c = fgetc(gl_prog_stream);
      if(c == EOF) {printf("Reached End Of File\n"); flag=0;break;}
      //printf("%c",c);
      if(c == '=') {
	lexeme_buf[lexeme_len++] = c;
	state = 13;
      }
      else if(c == '>') {
	lexeme_buf[lexeme_len++] = c;
	state = 14;
      }
      else state = 15;
      break;
    case 13:
      state = 1;
      lexeme_buf[lexeme_len++] = '\0';
      lexeme = (char*)malloc(strlen(lexeme_buf));
      strcpy(lexeme,lexeme_buf);
      gl_nexttoken = lookup(lexeme);
      flag=0;
      break;

    case 14:
      state = 1;
      lexeme_buf[lexeme_len++] = '\0';
      lexeme = (char*)malloc(strlen(lexeme_buf));
      strcpy(lexeme,lexeme_buf);
      gl_nexttoken = lookup(lexeme);
      flag=0;
      break;

    case 15:
      ungetc(c,gl_prog_stream);/*put the space back*/
      //lexeme_pos--;
      state = 1;
      lexeme_buf[lexeme_len++] = '\0';
      //printf("\nJust accepted an ID %s , %d\n",lexeme_buf,strlen(lexeme_buf));
      lexeme = (char*)malloc(strlen(lexeme_buf));
      strcpy(lexeme,lexeme_buf);
      gl_nexttoken = lookup(lexeme);
      flag=0;
      break;
   
    case 16: /*saw a  '>' or ':'*/
      c = fgetc(gl_prog_stream);
      if(c == EOF) {printf("Reached End Of File\n"); flag=0;break;}
      //printf("%c",c);
      if(c == '=') {
	lexeme_buf[lexeme_len++] = c;
	state = 1;
	lexeme_buf[lexeme_len++] = '\0';
	lexeme = (char*)malloc(strlen(lexeme_buf));
	strcpy(lexeme,lexeme_buf);
	gl_nexttoken = lookup(lexeme);
	flag=0;
	break;
      }
      else state = 15;
      break;
    case 17:
      c = fgetc(gl_prog_stream);
      if(c == EOF) {printf("Reached End Of File\n"); flag=0;break;}
      if(c == '*'){ 
	state = 18;
	lexeme_len = 0;
	lexeme_buf[lexeme_len] = '\0';
      }
      else
	state = 15;
      break;
    case 18:
      c = fgetc(gl_prog_stream);
      if(c == EOF) {fprintf(gl_error_log,"%s", "Reached End Of File..Unclosed comment\n"); exit(0);}
      if(c == '*')
	state = 19;
      else
	state = 18;
      break;
    case 19:
      c = fgetc(gl_prog_stream);
      if(c == EOF) {fprintf(gl_error_log,"%s", "Reached End Of File..Unclosed comment\n"); exit(0);}
      if(c == '/')
	state = 1;
      else 
	state = 18;
      break;

    case 100:
      //ERROR STATE: lexical errors are handled thus
      //0. show the contents of lexeme_buf and lineno.
      lexeme_buf[lexeme_len++] = c;
      lexeme_buf[lexeme_len++] = '\0';
      
      fprintf(gl_error_log,"LINE:%d:Invalid lexeme found: %s \n",gl_lineno+1,lexeme_buf);
      gl_nerrors++;
      //1.keep reading ahead until you find a delimiter.
	while(isdelimiter(c) == 0)
	  c = fgetc(gl_prog_stream);
            
      //1.a  Now c is the first letter of a  delimiting character
      ungetc(c,gl_prog_stream);//So I put it back and 
      //Don't forget to set the state back to 1;
      state = 1;
      lexeme_len =0;
      gl_lex_error = 1;
      break;
    }
    
  }
  
}


// int main(int argc,char** argv) {

//   char prog_buf[80];
//   FILE* st_input = fopen("ids.txt","r");

//   if(argc !=2 ) {
//     printf("usage: a.out <infile>\n");
//     exit(0);
//   }
  
//   gl_prog_stream = fopen(argv[1],"rw");

//   preload(st_input);
  
//   while(gl_prog_stream){
//     get_token();
//     print_token(gl_nexttoken);
//   }
//   exit(0);
//}
