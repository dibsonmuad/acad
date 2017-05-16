
#include "codegen.h"
#include "parser.h"
#include "tokens.h"

extern char* gl_prog_prefix;
extern FILE* gl_prog_stream;
extern int gl_nerrors;

int main(int argc, char** argv)
{
    char *err_file, c;
    int i = 0;
    FILE* st_input; // = fopen("ids.txt","r");
    // stack SAS = create_stack();
    bool val;
    if (argc != 2)
    {
        printf("mcp: No input files\n");
        exit(0);
    }

    if ((gl_prog_stream = fopen(argv[1], "rw")) == NULL
        || (st_input = fopen("ids.txt", "r")) == NULL)
    {
        printf("invalid file name or ids.txt not found...Exiting\n");
        exit(1);
    }

    err_file = (char*)malloc(strlen(argv[1]) + 4);
    //  printf("strlen err_file:%d: and arv[1] is
    //  :%d:\n",strlen(err_file),strlen(argv[1]));

    c = argv[1][0];
    while (1)
    {
        if (c == '.' || c == '\0')
            break;
        else
        {
            // printf("%c",argv[1][i]);
            err_file[i] = argv[1][i];
            i++;
            c = argv[1][i];
        }
    }
    err_file[i] = '\0';

    gl_prog_prefix = (char*)malloc(strlen(err_file) * sizeof(char));
    strcpy(gl_prog_prefix, err_file); // careful before changing the order
    // of statements here.

    sprintf(err_file, "%s%s", gl_prog_prefix, ".err\0");
    // printf("name of error file is : %s\n",err_file);

    gl_error_log = fopen(err_file, "w");

    preload(st_input);
    // initialise the semantic action stack
    // caused a few segfaults before I figured this out
    init_stack(&SAS);
    val = P();
    fclose(gl_error_log);

    if (gl_nerrors == 0)
    {
        assign_address();
        print_st();
        codegen();
        print_quad_table();
    }

    exit(val);
}
