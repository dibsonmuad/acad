#include "symbol_table.h"

/*source file will be read into the buffer one line at a time */
/////////////////////////////////////////////////////////////
token_t* create_token(char* token, int token_type) {
  token_t* temp_token = (token_t*) malloc(sizeof(token_t));
  temp_token->name = (char*)malloc(sizeof(strlen(token)+1));
  /*fgets already inserts null char */
  strcpy(temp_token->name,token);
  temp_token->type = token_type;
  /*until I understand what it means.. two weeks later.. I just did*/
  return(temp_token);
}

/////////////////////////////////////////////////////////////
void preload(FILE* symbol_file) {

  /* load the symbol table with the keywords and common symbols */
  char buf[80],token_name[80];
  int   i=0;
  while(fgets(buf,80,symbol_file) != NULL) {
    sscanf(buf,"%s",token_name);
    //printf("read in %s and lengh is %d\n",token_name,strlen(token_name));
    symboltable[i] = create_token(token_name,i+1);
    symboltable[i]->index = nentries;
    i++;
    /***** VERY IMP ************/
    nentries++;
    /* these will be known symbols and token types */
  }
}


/////////////////////////////////////////////////////////////
token_t* lookup(char* token_name) {

  /* look for a token inside the symbol table*/
  int i;
  for(i=0;i<nentries;i++)
    if(strcmp(token_name,symboltable[i]->name) == 0) {
      /* string found */
      //printf("just looked up this one '%s'\n",symboltable[i]->name);
      return(symboltable[i]);/*i indicates the entry in the sym table*/
    }
  return(NULL);
}

/////////////////////////////////////////////////////////////
token_t* insert(char* lexeme,int type) {
  /*This assumes a succesful insert all the time*/
  token_t* temp_token;
  token_t* token_lookedup = lookup(lexeme);

  if(token_lookedup == NULL) { /*not found*/
/* 2. takes care of memory allocation here*/
/*    temp_token = create_token(lexeme,nentries+1);*/
    temp_token = create_token(lexeme,type);
    symboltable[nentries] = temp_token;
    symboltable[nentries]->index = nentries;
    nentries++;
    printf("Just inserted %s\n",lexeme);
    return(temp_token);
  }

  else {
    //printf("Insert says Hey , I found \"%s\" in the symbol table\n",lexeme);
    //print_token(token_lookedup);
    return(token_lookedup);/* or nentries? or token id?*/
  }

}

/////////////////////////////////////////////////////////////
  /*prints symbol table to the stdout... for debugging*/
void print_st() {
  FILE* fsymtable;
  token_t* tok;
  char* sym = (char*)malloc(strlen(gl_prog_prefix)+4);
  array_t* p_arr;
  int i;

  sprintf(sym,"%s%s",gl_prog_prefix,".stb");
  fsymtable = fopen(sym,"w");

  //printf("Here is the symboltable\n*** Token ***\t\t*** Type ***\t\t***length***    \n");
  //printf("    -----    \t\t    ----    \n");
  for (i=0;i<nentries;i++) {
    tok = symboltable[i];
    fprintf(fsymtable,"%d\t    %s    \t    %d    \t %d\t %d\n",tok->index,tok->name,tok->type,tok->data_type,tok->start_address);
  }

  for (i=0;i<nentries;i++){
    if(symboltable[i]->data_type == ARRAY) {
      p_arr = (array_t*)symboltable[i]->dtype_specifics;
      fprintf(fsymtable,"%s is an array of dimension %d with size %d\n",
	      symboltable[i]->name,p_arr->dim,p_arr->size);
    }
  }
  fclose(fsymtable);
  free(sym);

}
/////////////////////////////////////////////////////////////
void print_token(token_t* t) {
  if(t == NULL)
    printf("NULL token.. possibly an undeclared identifier somewhere\n");
  else
    printf("%d, %s, %d, %d, %d\n",t->index,t->name,t->type,t->data_type,t->start_address);
}

/////////////////////////////////////////////////////////////
token_t* gen_temp(){
  token_t* temp_token;
  char* str,buf[80],buf_tmp[10];

  strcpy(buf,"__tmp\0");
  sprintf(buf_tmp,"%d",gl_ntemps);
  strcat(buf,buf_tmp);
  str = (char*) malloc(strlen(buf));
  strcpy(str,buf);
  temp_token = insert(str,ID);
  if(temp_token == NULL) {
    printf("couldn't insert token\n");
    exit(1);
  }
  /* for the time being only integer temporaries are generated */
  temp_token->data_type = INTEGER;
  gl_ntemps++;
  free(str);
  return temp_token;
}

/* once the parser is done with its job, this function should be called to
   look at the symbol table and assign absolute addresses to symbols and
   temporary variables in our program */
void assign_address(){
  token_t* tok;
  array_t* p_arr;
  int current_address = 0;
  for(int i=0;i<nentries;i++){
    tok = symboltable[i];
    /*ID*/
    if(tok->type == ID){
      if(tok->data_type == INTEGER){
	tok->start_address = current_address;
	current_address += 1;
      }
      else if(tok->data_type == ARRAY){
	p_arr = (array_t*)tok->dtype_specifics;
	tok->start_address = current_address;
	current_address += p_arr->size;
      }
    }
  }
}

// int main(){
//   token_t* temp_token;
//   int i;
//   for(i = 0; i<=100;i++){
//     temp_token = gen_temp();
//     print_token(temp_token);
//   }
// }
