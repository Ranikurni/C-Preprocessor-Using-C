
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "comment_removal.h"
#include "file_inclusion.h"
#include "macro_handler.h"

int main(int argc, char *argv[])
{
    FILE *fp, *temp1, *temp2, *op;
    char output[256];
    char *ptr;

    if (argc != 2)
    {
        printf("Usage: ./mypreprocessor inputfile.c\n");
        return 1;
    }

    fp = fopen(argv[1], "r");

    if (fp == NULL)
    {
        printf("Cannot open input file\n");
        return 1;
    }

    if (strlen(argv[1]) >= sizeof(output))
    {
        printf("Filename too long\n");
        fclose(fp);
        return 1;
    }

    strcpy(output, argv[1]);

    ptr = strrchr(output, '.');

    if (ptr != NULL)
        strcpy(ptr, ".i");
    else
        strcat(output, ".i");

    temp1 = tmpfile();
    temp2 = tmpfile();
    op = fopen(output, "w");

    if (temp1 == NULL || temp2 == NULL || op == NULL)
    {
        printf("File creation failed\n");

        fclose(fp);

        if (temp1 != NULL)
            fclose(temp1);

        if (temp2 != NULL)
            fclose(temp2);

        if (op != NULL)
            fclose(op);

        return 1;
    }

    // Stage 1: Comment removal
    remove_comments(fp, temp1);

    rewind(temp1);

    // Stage 2: File inclusion
    process_includes(temp1, temp2);

    rewind(temp2);

    // Stage 3: Macro substitution
    process_macros(temp2, op);

    fclose(fp);
    fclose(temp1);
    fclose(temp2);
    fclose(op);

    printf("Preprocessing completed successfully!\n");
    printf("Generated file: %s\n", output);

    return 0;
}

