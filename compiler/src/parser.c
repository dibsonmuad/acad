
#include "parser.h"
//#include <stdio.h>
//#include <stdlib.h>

void parse_error() {
  //keep reading until next token is ;||else||fi||od
  //beware that all valid tokens will be inserted into the symbol table, but
  // no code will be generated
  printf("Inside parse error\n");
  int token_type = gl_nexttoken->type;
  gl_nerrors++;
  fprintf(gl_error_log,"LINE:%d: unexpected token\n",gl_lineno+1);
  fprintf(stderr,"LINE:%d: unexpected token\n",gl_lineno+1);

  while(1){
    if((token_type == SCOLON) || (token_type == ELSE) || \
	(token_type == FI) || (token_type == OD) || (token_type == PCNT))
      break;
    else{
      get_token();
      print_token(gl_nexttoken);
      token_type = gl_nexttoken->type;
    }
  }
}

bool P(){
  //printf("in P\n");
  get_token();
  print_token(gl_nexttoken);
  if(gl_nexttoken->type == PROGRAM){
    get_token();
    print_token(gl_nexttoken);
    if( D1()){
      if(gl_nexttoken->type == PCNT){
	printf("Time to call S\n");
	gl_reading_stmts = 1;
	get_token();
	print_token(gl_nexttoken);
	if(S()){
	  if(gl_nexttoken->type == PCNT)
	    return true;
	  else
	    //	    syntax_error(nexttoken,PCNT);
	    return false;
	}
	else {//S()
	  parse_error();
	  if(gl_nexttoken->type == PCNT)
	    return true;
	  else {
	    get_token();//consumes SCOLON,ELSE,FI or OD
	    S();//shady ... Well it works for now
	    return true;
	  }

	}

      }
	else
	//	syntax_error(nexttoken,PCNT);
	  return false;
    }
    else return false;
  }
  else
    return false;
}

bool D1() {
  // printf("in D1\n");
  if(IL()){
    if(gl_nexttoken->type == COLON){
      get_token();
      print_token(gl_nexttoken);
      if(D()){
	return true;
      }
      else {
	parse_error();
	if(gl_nexttoken->type == PCNT)
	  return true;
	else {
	  get_token();
	  D1();
	  return true;
	}
      }
    }
    else {
      parse_error();
      if(gl_nexttoken->type == PCNT)
	return true;
      else {
	get_token();
	D1();
	return true;
      }
    }
  }
  else
    if(gl_nexttoken->type == PCNT){
      return true;
  }
  else {//D1()
      parse_error();
      if(gl_nexttoken->type == PCNT)
	return true;
      else {
	get_token();
	D1();
	return true;
      }
  }

}

bool D() {
  int t_arr,arr_dim,index;
  array_t* p_arr;
  stack arr_types,arr_dim_stk;

  init_stack(&arr_types);
  init_stack(&arr_dim_stk);

  if(gl_nexttoken->type == ARRAY) {
    /*Sem Action : pop the stack until empty and assign the data_type ARRAY
      to all the elements */
    while(SAS.top != 0){
      t_arr = pop(&SAS);
      push(&arr_types,t_arr);
      symboltable[t_arr]->data_type = ARRAY;
      p_arr = (array_t*)malloc(sizeof(array_t));
      symboltable[t_arr]->dtype_specifics = (void*) p_arr;
    }

    get_token();
    print_token(gl_nexttoken);

    if(gl_nexttoken->type == LSB) {
      push(&SAS,SEPERATOR);
      get_token();
      print_token(gl_nexttoken);
      if(gl_nexttoken->type == CONS){
	// Sem action : push the cons to calculate array dim
	push(&SAS,gl_nexttoken->index);
	get_token();
	print_token(gl_nexttoken);
	if(D2()){
	  if(gl_nexttoken->type == RSB) {
	    //Sem actions: calculate array size and dims

	    //This reverses the array dimensions to their correct order
	    while((arr_dim = pop(&SAS)) != SEPERATOR)
		push(&arr_dim_stk,arr_dim);
	    index =0;
	    p_arr->size = 1;
	    while((arr_dim = pop(&arr_dim_stk)) != -1) {
	      //current dimension starting from first: like if we had
	      //x y z , x first, y second, z third will be the "pop order"

	      p_arr->dim++;
	      p_arr->dims[index] = atoi(symboltable[arr_dim]->name);
	      p_arr->size = p_arr->size * p_arr->dims[index];
	      index++;
	    }
	    // Do I need this?
	    //p_arr->dim++;

	    while((t_arr = pop(&arr_types)) != -1) {
	      symboltable[t_arr]->dtype_specifics = (void*)p_arr;
	    }
	    get_token();
	    print_token(gl_nexttoken);
	    if(gl_nexttoken->type == SCOLON){
	      get_token();
	      print_token(gl_nexttoken);
	      if(D1()){
		return true;
	      }
	      else return false;/*D1*/
	    }
	    else return false;/*SCOLON*/
	  }
	  else return false;/*RSB*/
	}
	else return false;/*D2*/
      }
      else return false;/*CONS*/
    }
    else return false;/*LSB*/
  }
  else if(gl_nexttoken->type == INTEGER) {
    //Sem Action : pop the stack until empty and assign the data_type INTEGER
    // to all the elements
    print_stack(&SAS);
    while(SAS.top != 0)
      symboltable[pop(&SAS)]->data_type = INTEGER;
    get_token();
    print_token(gl_nexttoken);
    if(gl_nexttoken->type == SCOLON){
      get_token();
      print_token(gl_nexttoken);
      if(D1()){
	return true;
      }
      else return false;
    }
    else return false;
  }
  else {
    printf("false in D.. token is \n");
    print_token(gl_nexttoken);
    return false;
  }
}


bool D2() {

  if(gl_nexttoken->type == COMMA) {
    get_token();
    print_token(gl_nexttoken);
    if(gl_nexttoken->type == CONS){
      //Sem action : multi dim array , keep pushing the cons
      push(&SAS,gl_nexttoken->index);
      get_token();
      print_token(gl_nexttoken);
      if(D2()){
	return true;
      }
      else return false;
    }
    else return false;
  }
  else if(gl_nexttoken->type == RSB){
    //get_token();
    //print_token(gl_nexttoken);
    return true;
  }
  else return false;
}

bool IL(){
  if(gl_nexttoken->type == ID){
    //Performing the following actions We can get here from a declaration
    //or a read-write statement push the index of the ID into the SAS
    push(&SAS,gl_nexttoken->index);
    get_token();
    print_token(gl_nexttoken);
    if(IL1()){
      return true;
    }
    else return false;
  }
  else return false;
}

bool IL1(){
  if(gl_nexttoken->type == COMMA){
    get_token();
    print_token(gl_nexttoken);
    if(IL()){
      return true;
    }
    else return false;
  }
  else if(gl_nexttoken->type == COLON){
    //get_token();
    //print_token(gl_nexttoken);
    return true;
  }
  else if(gl_nexttoken->type == RP){
    return true;
  }
  else return false;
}

/*statements */
bool S() {

  int t_op1,t_op2,t_res,id_index;
  static stack rw_stack;
  //static stack write_stack;
  token_t* temp_tok;


  //ok its a valid token now
  //print_token(gl_nexttoken);
  switch(gl_nexttoken->type) {
  case(ID):
    //Sem action: just push it for now
    push(&SAS,gl_nexttoken->index);
    get_token();
    print_token(gl_nexttoken);
    if(T1()){
      if(gl_nexttoken->type == ASSGN){
	get_token();
	print_token(gl_nexttoken);
	if(E()){
	  //Sem action : 1. when E returns I'll find 2 IDs on the stack.
	  t_op1 = pop(&SAS);
	  t_res = pop(&SAS);
	  /* had to add these lines to avoid an additional instruction
	   LD indirect */
	  /*if(symboltable[pq->op1]->mode == 1 && pq->q_mode != 1){
	   temp_tok =  gentemp();
	   gen_quad(Q_ASSGN,op1,DUMMY,temp_tok->index);
	   if(symboltable[pq->op1]->mode == 1 && pq->q_mode != 1)
	   gen_quad(Q_ASSGN,temp_tok->index,DUMMY,t_res);
	   }*/
	  gen_quad(Q_ASSGN,t_op1,DUMMY,t_res);
	  if(S1()){
	    return true;
	  }
	  else return false;
	}
	else return false;
      }
      else return false;
    }
    else return false;
    break;
  case(READ):

    //Sem action :
    push(&SAS,SEPERATOR);

    get_token();
    if(gl_nexttoken->type==LP){
      get_token();
      print_token(gl_nexttoken);
      if(IL()){
	if(gl_nexttoken->type==RP){
	  //Sem action
	  while((t_res = pop(&SAS)) != SEPERATOR)
	    push(&rw_stack,t_res);

	  //SAS is back to its original state
	  while(rw_stack.top != 0)
	    gen_quad(Q_READ,DUMMY,DUMMY,pop(&rw_stack));

	  get_token();
	  print_token(gl_nexttoken);
	  if(S1()){
	    return true;
	  }
	  else
	    return false;
	}
	else
	  return false;
      }
      else
	return false;
    }
    else
      return false;
    break;
  case(WRITE):
    //Sem action :
    push(&SAS,SEPERATOR);

    get_token();
    print_token(gl_nexttoken);
    if(gl_nexttoken->type== LP){
      get_token();
      print_token(gl_nexttoken);
      if(IL()){
	if(gl_nexttoken->type==RP){
	  //Sem action
	  while((t_res = pop(&SAS)) != SEPERATOR)
	    push(&rw_stack,t_res);

	  //SAS is back to its original state
	  while(rw_stack.top != 0)
	    gen_quad(Q_WRITE,DUMMY,DUMMY,pop(&rw_stack));

	  get_token();
	  print_token(gl_nexttoken);
	  if(S1()){
	    return true;
	  }
	  else
	    return false;
	}
	else
	  return false;
      }
      else
	return false;
    }
    else
      return false;
    break;
  case(READLN):
    //Sem action:
    push(&SAS,SEPERATOR);
    get_token();
    print_token(gl_nexttoken);
    if(gl_nexttoken->type== LP){
      get_token();
      print_token(gl_nexttoken);
      if(IL()){
	if(gl_nexttoken->type==RP){
	  //Sem action
	  while((t_res = pop(&SAS)) != SEPERATOR)
	    push(&rw_stack,t_res);

	  //SAS is back to its original state
	  while(rw_stack.top != 0)
	    gen_quad(Q_READ,DUMMY,DUMMY,pop(&rw_stack));
	  gen_quad(Q_READLN,DUMMY,DUMMY,DUMMY);
	  get_token();
	  print_token(gl_nexttoken);
	  if(S1()){
	    return true;
	  }
	  else
	    return false;
	}
	else
	  return false;
      }
      else
	return false;
    }
    else
      return false;
    break;
  case(WRITELN):
    //Sem action
    push(&SAS,SEPERATOR);
    get_token();
    print_token(gl_nexttoken);
    if(gl_nexttoken->type== LP){
      get_token();
      print_token(gl_nexttoken);
      if(IL()){
	if(gl_nexttoken->type==RP){
	  //Sem action
	  while((t_res = pop(&SAS)) != SEPERATOR)
	    push(&rw_stack,t_res);

	  //SAS is back to its original state
	  while(rw_stack.top != 0)
	    gen_quad(Q_WRITE,DUMMY,DUMMY,pop(&rw_stack));
	  gen_quad(Q_WRITELN,DUMMY,DUMMY,DUMMY);
	  get_token();
	  print_token(gl_nexttoken);
	  if(S1()){
	    return true;
	  }
	  else
	    return false;
	}
	else
	  return false;
      }
      else
	return false;
    }
    else
      return false;
    break;
  case(IF):
    get_token();
    print_token(gl_nexttoken);
    if(C1()){
      //Sem actions
      t_op1 = pop(&SAS);
      push(&SAS,gl_nquads);
      gen_quad(Q_JEQ,t_op1,FALSE,UNKNOWN);

      if(gl_nexttoken->type == THEN){
	get_token();
	print_token(gl_nexttoken);
	if(S()){
	  if(gl_nexttoken->type == ELSE){
	    //Sem actions
	    //The value of the jmp quad above
	    t_res = pop(&SAS);
	    push(&SAS,gl_nquads);

	    gen_quad(Q_JMP,DUMMY,DUMMY,UNKNOWN);
	    quadtable[t_res]->result = gl_nquads;
	    //if IF was false, it jumps here.
	    get_token();
	    print_token(gl_nexttoken);
	    if(S()){
	      if(gl_nexttoken->type == FI){
		//Sem action
		t_res = pop(&SAS);
		//t_res is the quad no. of "else" quad
		quadtable[t_res]->result = gl_nquads;
		//next quad else jumps here if IF was true

		get_token();
		print_token(gl_nexttoken);
		if(S1()){
		  return true;
		}
		else
		  return false;
	      }
	      else
		return false;
	    }
	    else
	      return false;
	  }
	  else
	    return false;
	}
	else
	  return false;
      }
      else
	return false;
    }
    else
      return false;
    break;
  case(WHILE):
    //Sem action
    push(&SAS,gl_nquads);

    get_token();
    print_token(gl_nexttoken);
    if(C1()){
      //Sem actions
      t_op1 = pop(&SAS);
       push(&SAS,gl_nquads);
       gen_quad(Q_JEQ,t_op1,FALSE,UNKNOWN);

      if(gl_nexttoken->type == DO){
	get_token();
	print_token(gl_nexttoken);
	if(S()){
	  if(gl_nexttoken->type == OD){
	    //Sem actions
	    t_op1 = pop(&SAS);
	    t_op2 = pop(&SAS);
	    gen_quad(Q_JMP,DUMMY,DUMMY,t_op2);
	    //if Condition was false,jump here
	    quadtable[t_op1]->result = gl_nquads;

	    get_token();
	    print_token(gl_nexttoken);
	    if(S1()){
	      return true;
	    }
	    else return false;
	  }
	  else return false;
	}
	else return false;
      }
      else return false;
    }
    else return false;

    break;
  case(FOR):
    get_token();
    print_token(gl_nexttoken);

    if(gl_nexttoken->type == ID){
      id_index = gl_nexttoken->index;
      get_token();
      print_token(gl_nexttoken);
      if(gl_nexttoken->type == ASSGN){
	get_token();
	print_token(gl_nexttoken);
	if(E()){
	  //Sem actions
	  t_op1 = pop(&SAS);//pop the ID or CONS pushed by E
	  gen_quad(Q_ASSGN,t_op1,DUMMY,id_index);

	  if(gl_nexttoken->type == TO){
	    get_token();
	    print_token(gl_nexttoken);
	    if(E()){
	      //Sem actions
	      t_op2 = pop(&SAS);//the ID or CONS pushed by E
	      push(&SAS,gl_nquads);
	      temp_tok = gen_temp();
	      /* to make my quads consistent, all my conditionals should
		 evaluate to FALSE */
	      /*gen_quad(Q_GT,id_index,t_op2,temp_tok->index);*/
	      gen_quad(Q_LTE,id_index,t_op2,temp_tok->index);

	      push(&SAS,gl_nquads);
	      /*gen_quad(Q_JEQ,temp_tok->index,TRUE,UNKNOWN);*/
	      gen_quad(Q_JEQ,temp_tok->index,FALSE,UNKNOWN);
	      if(gl_nexttoken->type == DO){
		get_token();
		print_token(gl_nexttoken);
		if(S()){
		  if(gl_nexttoken->type == OD){
		    //Sem actions
		    t_op1 = pop(&SAS); //contains quad no. of JEQ
		    t_op2 = pop(&SAS);//contains quad no. of GT ..
		    //This is to make sure that '1' is in the
		    //symbol table
		    temp_tok = insert((char*)"1",CONS);
		    gen_quad(Q_PLUS,id_index,temp_tok->index,id_index);
		    //back to check termination
		    gen_quad(Q_JMP,DUMMY,DUMMY,t_op2);
		    //go back and patch JEQ
		    quadtable[t_op1]->result = gl_nquads;

		    get_token();
		    print_token(gl_nexttoken);
		    if(S1()){
		      return true;
		    }
		    else return false;
		  }
		  else return false;
		}
		else return false;
	      }
	      else return false;
	    }
	    else return false;
	  }
	  else return false;
	}
	else return false;
      }
      else return false;
    }
    else return false;
    break;
  default:
      parse_error();
      if(gl_nexttoken->type == PCNT)
	return true;
      else {
	get_token();
	S();
	return true;
      }
  }
}

bool S1(){
  switch(gl_nexttoken->type){
  case(SCOLON):
    get_token();
    print_token(gl_nexttoken);
    if(S())
      return true;
    else {
      parse_error();
      if(gl_nexttoken->type == PCNT)
	return true;
      else {
	get_token();
	S();
	return true;
      }
    }
  case(PCNT):
    return true;
  case(ELSE):
    return true;
  case(FI):
    return true;
  case(OD):
    return true;
  default:
    parse_error();
    if(gl_nexttoken->type == PCNT)
      return true;
    else {
      get_token();
      S();
      return true;
    }
  }
}



bool C1(){
  if(E()){
    if(C2()){
      return true;
    }
    else return false;
  }
  else return false;
}

bool C2(){
  int t_op1,t_op2;
  token_t* temp_tok;

  switch(gl_nexttoken->type){
  case(GT):
    get_token();
    print_token(gl_nexttoken);
    if(E()){
      //Sem action x>y should be > x y t1
      t_op2 = pop(&SAS);//RIGHT HS
      t_op1 = pop(&SAS);//LEFT HS

      temp_tok = gen_temp();
      gen_quad(Q_GT,t_op1,t_op2,temp_tok->index);
      push(&SAS,temp_tok->index);
      return true;
    }
    else return false;
  case(LT):
    get_token();
    print_token(gl_nexttoken);
    if(E()){
      //Sem action x>y should be > x y t1
      t_op2 = pop(&SAS);//RIGHT HS
      t_op1 = pop(&SAS);//LEFT HS

      temp_tok = gen_temp();
      gen_quad(Q_LT,t_op1,t_op2,temp_tok->index);
      push(&SAS,temp_tok->index);
      return true;
    }
    else return false;
  case(EQ):
    get_token();
    print_token(gl_nexttoken);
    if(E()){
      //Sem action x>y should be > x y t1
      t_op2 = pop(&SAS);//RIGHT HS
      t_op1 = pop(&SAS);//LEFT HS

      temp_tok = gen_temp();
      gen_quad(Q_EQ,t_op1,t_op2,temp_tok->index);
      push(&SAS,temp_tok->index);
      return true;
    }
    else return false;
  case(LTE):
    get_token();
    print_token(gl_nexttoken);
    if(E()){
      //Sem action x>y should be > x y t1
      t_op2 = pop(&SAS);//RIGHT HS
      t_op1 = pop(&SAS);//LEFT HS

      temp_tok = gen_temp();
      gen_quad(Q_LTE,t_op1,t_op2,temp_tok->index);
      push(&SAS,temp_tok->index);
      return true;
    }
    else return false;
  case(GTE):
    get_token();
    print_token(gl_nexttoken);
    if(E()){
      //Sem action x>y should be > x y t1
      t_op2 = pop(&SAS);//RIGHT HS
      t_op1 = pop(&SAS);//LEFT HS

      temp_tok = gen_temp();
      gen_quad(Q_GTE,t_op1,t_op2,temp_tok->index);
      push(&SAS,temp_tok->index);
      return true;
    }
    else return false;
  case(NE):
    get_token();
    print_token(gl_nexttoken);
    if(E()){
      //Sem action x>y should be > x y t1
      t_op2 = pop(&SAS);//RIGHT HS
      t_op1 = pop(&SAS);//LEFT HS

      temp_tok = gen_temp();
      gen_quad(Q_NEQ,t_op1,t_op2,temp_tok->index);
      push(&SAS,temp_tok->index);
      return true;
    }
    else return false;
  default:
    return false;
  }
}

bool T(){
  if(gl_nexttoken->type == ID){
    //Sem action
    push(&SAS,gl_nexttoken->index);
    get_token();
    print_token(gl_nexttoken);
    if(T1()){
      return true;
    }
    else return false;
  }
  else if(gl_nexttoken->type == CONS){
    //Sem Action
    push(&SAS,gl_nexttoken->index);
    get_token();
    print_token(gl_nexttoken);
    return true;
  }
  else return false;
}

bool T1() {
  //This would pop the array elements back from the SAS and put them in
  //proper order in another stack.
  stack rev_arr;
  int t_arr_index,t_arr_id,count,i;
  array_t* p_arr;
  token_t* temp_tok,*temp_tok1,*temp_tok2;
  char buf[10];

  init_stack(&rev_arr);
  switch(gl_nexttoken->type){
  case(LSB):
    //Reading an array : Sem Actions
    push(&SAS,SEPERATOR);
    get_token();
    print_token(gl_nexttoken);
    if(E()){
      if(E1()){
	if(gl_nexttoken->type == RSB){
	  //the array indices are pushed on to the SAS
	  while((t_arr_index = pop(&SAS))!= SEPERATOR)
	    push(&rev_arr,t_arr_index);
	  t_arr_id = pop(&SAS);
	  temp_tok = gen_temp();

	  gen_quad(Q_ADDR,t_arr_id,DUMMY,temp_tok->index);
	  quadtable[gl_nquads-1]->q_mode = 1;

	  //remember to push the id back after pushing the index


	  p_arr = (array_t*)symboltable[t_arr_id]->dtype_specifics;
	  //take these lines off later..
	  printf("array properties: dim %d ",p_arr->dim);
	  for(i=0;i<p_arr->dim;i++)
	    printf("%d ",p_arr->dims[i]);
	  printf("\n");
	  if(p_arr == NULL){printf("t_arr_id not an array\n");exit(1);}

	  //Here is where I calculate the index
	  temp_tok1 = gen_temp();
	  count = 0;
	  while((t_arr_index = pop(&rev_arr))!= -1){
	    gen_quad(Q_ASSGN,t_arr_index,DUMMY,temp_tok1->index);//say tmp_30
	    quadtable[gl_nquads-1]->q_mode = 1;
	    //temp_index = t_arr_index;
	    for(i=1+count; i<p_arr->dim;i++){
	      sprintf(buf,"%d",p_arr->dims[i]);
	      temp_tok2 = insert(buf,CONS);
	      printf("Looked up %s \n",buf);
	      print_token(temp_tok2);
	      //temp_index = temp_index * p_arr->dim[i];
	      //tmp_30 = tmp_30 * temp_tok2->index;
	      gen_quad(Q_MUL,temp_tok1->index,\
		       temp_tok2->index,temp_tok1->index);
	      quadtable[gl_nquads-1]->q_mode = 1;

	    }
	    //out of for loop
	    //actual_index = actual_index + temp_index;
	    gen_quad(Q_PLUS,temp_tok->index,temp_tok1->index,temp_tok->index);
	    quadtable[gl_nquads-1]->q_mode = 1;
	    count++;
	  }
	  push(&SAS,temp_tok->index);
	  //indirect addressing
	  symboltable[temp_tok->index]->mode = 1;
	  //dont have to do this because temp_tok has the absolute address of
	  // the array base and offset
	  //push(&SAS,t_arr_id);
	  get_token();
	  print_token(gl_nexttoken);
	  return true;
	}
	else return false;
      }
      else return false;//E1()
    }
    else return false;//E()
  case(ID):
    return true;
  case(CONS):
    return true;
  case(TO):
    return true;
  case(DO):
    return true;
  case(THEN):
    return true;
  case(ELSE):
    return true;
  case(FI):
    return true;
  case(OD):
    return true;
  case(PCNT):
    return true;
  case(RSB):
    return true;
  case(LT):
    return true;
  case(GT):
    return true;
  case(EQ):
    return true;
  case(LTE):
    return true;
  case(GTE):
    return true;
  case(NE):
    return true;
  case(PLUS):
    return true;
  case(MINUS):
    return true;
  case(MUL):
    return true;
  case(DIV):
    return true;
  case(ASSGN):
    return true;
  case(COMMA):
    return true;
  case(SCOLON):
    return true;
  default:
    return false;
    break;
  }
}

bool E(){
  if(T()){
    if(E2()){
      return true;
    }
    else return false;
  }
  else return false;
}

bool E1(){
  if(gl_nexttoken->type == COMMA){
    get_token();
    print_token(gl_nexttoken);
    if(E()){
	if(E1()){
	  return true;
	}
	else return false;
    }
    else return false;
  }
  else if(gl_nexttoken->type == RSB){
    return true;
  }
  else return false;
}

bool E2(){
  if(E()){
    if(O()){
      if(E2()){
	return true;
      }
      else return false;
    }
    else return false;
  }
  else {
    switch(gl_nexttoken->type) {
 case(TO):
   return true;
  case(DO):
    return true;
  case(THEN):
    return true;
  case(ELSE):
    return true;
  case(FI):
    return true;
  case(OD):
    return true;

  case(PCNT):
   return true;
  case(RSB):
   return true;
  case(LT):
   return true;
  case(GT):
   return true;
  case(EQ):
   return true;
  case(LTE):
   return true;
  case(GTE):
   return true;
  case(NE):
   return true;
  case(PLUS):
   return true;
  case(MINUS):
   return true;
  case(MUL):
   return true;
  case(DIV):
   return true;
  case(ASSGN):
   return true;
  case(COMMA):
   return true;
  case(SCOLON):
   return true;
    default:
      return false; break;
    }
  }
}

bool O(){
  token_t* temp_tok;

  switch(gl_nexttoken->type){
  case(PLUS):
    get_token();
    print_token(gl_nexttoken);
    //Sem actions
    temp_tok = gen_temp();
    gen_quad(Q_PLUS,pop(&SAS),pop(&SAS),temp_tok->index);
    push(&SAS,temp_tok->index);
    return true; break;

  case(MINUS):
    get_token();
    print_token(gl_nexttoken);
    //Sem actions
    temp_tok = gen_temp();
    gen_quad(Q_MINUS,pop(&SAS),pop(&SAS),temp_tok->index);
    push(&SAS,temp_tok->index);
    return true; break;
  case(MUL):
    get_token();
    print_token(gl_nexttoken);
    //Sem actions
    temp_tok = gen_temp();
    gen_quad(Q_MUL,pop(&SAS),pop(&SAS),temp_tok->index);
    push(&SAS,temp_tok->index);
    return true; break;
  case(DIV):
    get_token();
    print_token(gl_nexttoken);
    //Sem actions
    temp_tok = gen_temp();
    gen_quad(Q_DIV,pop(&SAS),pop(&SAS),temp_tok->index);
    push(&SAS,temp_tok->index);

    return true; break;
  default:
    return false; break;
  }
}


