#include "emu.h"

#define M_OPCODE 61440
#define M_OP1REG  3584
#define M_OP2REG   448

#define M_OP1MEM  4095
#define M_OP2MEM   511


/* The emulator does the following
    1. load the program from disk to memory
    2. follow the fetch - decode - execute cycle
*/
int p[4096];

/* the program counter */
int pc;

/*there are 8 registers in all */
int reg[8];

/* data starts here */
int data_offset;

/* sign is set after a SUB operation. Will be used for conditional jumps */
int sign;

/* this is to keep track of the branch instructions*/
int prev_op;

int load(char* infile_name){
  int lineno;
  char buf[80];
  FILE* infile = fopen(infile_name,"r");
  if(infile == NULL){
    printf("Invalid file name\n");
    exit(1);
  }
  lineno = 0;

  while((fgets(buf,80,infile))!= NULL){
    //printf("buf %s",buf);
    p[lineno] = atoi(buf);
    //printf("read in %d\n",p[lineno]);
    lineno = lineno+1;
  }
  fclose(infile);
  return lineno;
}

int fetch(){
  return p[pc++];
}

decoded_instruction decode(int ins_reg){
  decoded_instruction di;
  di.opcode = (ins_reg & M_OPCODE) >> 12;
  di.op1reg = (ins_reg & M_OP1REG) >> 9;
  di.op2reg = (ins_reg & M_OP2REG) >> 6;

  di.op1mem = (ins_reg & M_OP1MEM);//2**12 -1
  di.op2mem = (ins_reg & M_OP2MEM);//2**9 -1
  return di;
}

int execute(decoded_instruction di){
  int temp_mem,input;
  char dummy[80];
  switch(di.opcode){
  case(OP_LD):
    reg[di.op1reg] = p[di.op2mem + data_offset];
    break;
  case(OP_LDI):
    reg[di.op1reg] = di.op2mem; //no offset it is just an immediate value
    break;
  case(OP_LDA):
    //loaded the indirect address into temp_mem
    temp_mem = p[di.op2mem + data_offset];
    reg[di.op1reg] = p[temp_mem + data_offset];
    break;
  case(OP_ST):
    p[di.op2mem + data_offset] = reg[di.op1reg];
    break;
  case(OP_STA):
    temp_mem = p[di.op2mem + data_offset];
    p[temp_mem + data_offset] = reg[di.op1reg];
    break;
  case(OP_ADD):
    reg[di.op1reg] = reg[di.op1reg] + reg[di.op2reg];
    break;
  case(OP_SUB):
    reg[di.op1reg] = reg[di.op1reg] - reg[di.op2reg];
    if (reg[di.op1reg] < 0)
      sign = -1;
    else if(reg[di.op1reg] == 0)
      sign = 0;
    else
      sign = 1;
    break;
  case(OP_MUL):
    reg[di.op1reg] = reg[di.op1reg] * reg[di.op2reg];
    break;
  case(OP_DIV):
    reg[di.op1reg] = reg[di.op1reg] / reg[di.op2reg];
    break;
  case(OP_B):
    pc = di.op1mem;
    break;
  case(OP_BEQ):
    /*look at the sign "bit"*/
    /* the sign bit is set after a subtract operation */
    /*eq should be false to branch successfully
      so the sign bit should not be equal to zero */
    //if((prev_op == OP_BLT)||(prev_op == OP_BGT))
    //sign = 0;
    if (sign == 0)
      pc = di.op1mem;
    break;
  case(OP_BLT):
    if(sign == -1)
      pc = di.op1mem;
    break;
  case(OP_BGT):
    if(sign == 1)
      pc = di.op1mem;
    break;
  case(OP_READ):
    //printf("waiting for input\n");
    fscanf(stdin,"%d",&input);
    p[di.op1mem + data_offset] = input;
    break;
  case(OP_WRITE):
    //printf("%d\n",p[di.op1mem + data_offset]);
    //printf("writing\n");
    printf("%d ",p[di.op1mem + data_offset]);
    //printf("%d\n",p[di.op1mem + data_offset ]);
    break;
  case(OP_NEWLINE):
    if(di.op2mem == 0)
      fgets(dummy,80,stdin);//fscanf(stdin,"%*[^\n]"
    else
      printf("\n");
    break;
  default:
    printf("unknown opcode\n");
    return -1;
  }
  return 1;
}

int main(int argc,char* argv[]){
  /* instruction register contains the instruction to be executed */
  int ins_reg;
  /* total length of the source code */
  int code_len;
  int error;
  /* this is the decoded instruction returned by the decode function*/
  decoded_instruction di;
  if(argc != 2){
    printf("usage: emu binaryfile\nExiting...\n");
    exit(0);
  }
  prev_op = 0;//nothing special, just initializing
  code_len = load(argv[1]);
  data_offset = code_len+1;//one intervening space will be vacant
  //printf("codelen is %d and data offset is %d\n",code_len,data_offset);
  //exit(0);
  pc = 0;
  while(pc < code_len){
    ins_reg = fetch();
    //printf("instruction is %d\n",ins_reg);
    di = decode(ins_reg);
    error = execute(di);
    if(error == -1)
      exit(1);
  }
}
