
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "file_inclusion.h"

void process_includes(FILE *input, FILE *output)
{
    char line[1024];
    char filename[256];
    char *p;
    FILE *header;
    int ch;

    while (fgets(line, sizeof(line), input) != NULL)
    {
        p = line;

        while (isspace((unsigned char)*p))
            p++;

        if (strncmp(p, "#include", 8) == 0)
        {
            p += 8;

            while (isspace((unsigned char)*p))
                p++;

            if (*p == '<' || *p == '"')
            {
                char closing = (*p == '<') ? '>' : '"';
                char *end;

                p++;

                end = strchr(p, closing);

                if (end != NULL &&
                    (size_t)(end - p) < sizeof(filename))
                {
                    size_t len = (size_t)(end - p);

                    memcpy(filename, p, len);
                    filename[len] = '\0';

                    header = fopen(filename, "r");

                    if (header != NULL)
                    {
                        while ((ch = fgetc(header)) != EOF)
                            fputc(ch, output);

                        fclose(header);
                        continue;
                    }
                }
            }
        }

        // Preserve unresolved includes
        fputs(line, output);
    }
}

