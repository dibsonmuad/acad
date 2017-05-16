#include "quad_table.h"

void gen_quad(int q_type, int op1, int op2, int res)
{
    quad* temp_quad = (quad*)malloc(sizeof(quad));

    temp_quad->q_type = q_type;
    temp_quad->op1    = op1;
    temp_quad->op2    = op2;
    temp_quad->result = res;
    temp_quad->number = gl_nquads;
    /* just initializing the instruction no. to -1*/
    temp_quad->inst_no   = -1;
    quadtable[gl_nquads] = temp_quad;

    gl_nquads++;
}

char* gen_readable_quad(quad* pq)
{
    char q_no_str[8], q_str[6], op1_str[16], op2_str[16], res_str[16], *str;
    char *final_str, inst_no_str[8];

    str = (char*)malloc(sizeof(char) * 10);
    memset(str, 0, 10);
    // check appropriate operand
    switch (pq->q_type)
    {
    case (Q_JMP):
        strncpy(str, "jmp", 3);
        break;
    case (Q_JEQ):
        strncpy(str, "jeq", 3);
        break;
    case (Q_READ):
        strncpy(str, "read", 4);
        break;
    case (Q_WRITE):
        strncpy(str, "write", 5);
        break;
    case (Q_READLN):
        strncpy(str, "readln", 6);
        break;
    case (Q_WRITELN):
        strncpy(str, "writeln", 7);
        break;
    case (Q_PLUS):
        strcpy(str, "+");
        break;
    case (Q_MINUS):
        strncpy(str, "-", 1);
        break;
    case (Q_MUL):
        strncpy(str, "*", 1);
        break;
    case (Q_DIV):
        strncpy(str, "/", 1);
        break;
    case (Q_ASSGN):
        strncpy(str, ":=", 2);
        break;
    case (Q_EQ):
        strncpy(str, "=", 1);
        break;
    case (Q_GTE):
        strncpy(str, ">=", 2);
        break;
    case (Q_LTE):
        strncpy(str, "<=", 2);
        break;
    case (Q_NEQ):
        strncpy(str, "<>", 2);
        break;
    case (Q_GT):
        strncpy(str, ">", 1);
        break;
    case (Q_LT):
        strncpy(str, "<", 1);
        break;
    case (Q_ADDR):
        strncpy(str, "addr", 4);
        break;
    case (Q_VADDR):
        strncpy(str, "vaddr", 5);
        break;

    default:
        printf("Unknown quad.. exiting\n");
        exit(0);
    }
    free(str);
    snprintf(q_str, strlen(str), "%-8s", str);

    sprintf(q_no_str, "%-8d", pq->number);
    // op1
    if (pq->op1 == TRUE)
        snprintf(op1_str, 16, "%-15s", "TRUE");
    else if (pq->op1 == FALSE)
        snprintf(op1_str, 16, "%-15s", "FALSE");
    else if (pq->op1 == DUMMY)
        snprintf(op1_str, 16, "%-15s", " ");
    else if (symboltable[pq->op1]->mode == 1 && pq->q_mode != 1)
        snprintf(op1_str, 16, "@%-14s", symboltable[pq->op1]->name);
    else
        snprintf(op1_str, 16, "%-15s", symboltable[pq->op1]->name);

    // op2
    if (pq->op2 == TRUE)
        snprintf(op2_str, 16, "%-15s", "TRUE");
    else if (pq->op2 == FALSE)
        snprintf(op2_str, 16, "%-15s", "FALSE");
    else if (pq->op2 == DUMMY)
        snprintf(op2_str, 16, "%-15s", " ");
    else if (symboltable[pq->op2]->mode == 1 && pq->q_mode != 1)
        snprintf(op2_str, 16, "@%-14s", symboltable[pq->op2]->name);
    else
        snprintf(op2_str, 16, "%-15s", symboltable[pq->op2]->name);

    // result
    if (pq->result == DUMMY)
        snprintf(res_str, 16, "%-15s", " ");
    else if ((pq->q_type == Q_JMP) || (pq->q_type == Q_JEQ))
        snprintf(res_str, 16, "%-15d", pq->result);
    else if (symboltable[pq->result]->mode == 1 && pq->q_mode != 1)
        snprintf(res_str, 16, "@%-14s", symboltable[pq->result]->name);
    else
        snprintf(res_str, 16, "%-15s", symboltable[pq->result]->name);

    snprintf(inst_no_str, 8, "%-8d", pq->inst_no);

    final_str = (char*)malloc(75 * sizeof(char));
    sprintf(final_str,
            "|%s|%s|%s|%s|%s|%s",
            q_no_str,
            q_str,
            op1_str,
            op2_str,
            res_str,
            inst_no_str);
    return final_str;
}

void print_quad_table()
{
    int i;
    FILE* log_quads;
    char* q_file = (char*)malloc(strlen(gl_prog_prefix) + 4);
    sprintf(q_file, "%s%s", gl_prog_prefix, ".qds");
    log_quads = fopen(q_file, "w");

    for (i = 0; i < gl_nquads; i++)
        fprintf(log_quads, "%s\n", gen_readable_quad(quadtable[i]));
    fclose(log_quads);
}
