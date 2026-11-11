
#include <stdio.h>
#include "comment_removal.h"

void remove_comments(FILE *input, FILE *output)
{
    int ch, next;
    int state = 0;

    /*
       States:
       0 - Normal code
       1 - Single-line comment
       2 - Multi-line comment
       3 - String literal
       4 - Character literal
    */

    while ((ch = fgetc(input)) != EOF)
    {
        if (state == 0)
        {
            if (ch == '/')
            {
                next = fgetc(input);

                if (next == '/')
                {
                    state = 1;
                }
                else if (next == '*')
                {
                    state = 2;
                }
                else
                {
                    fputc(ch, output);

                    if (next != EOF)
                        ungetc(next, input);
                }
            }
            else if (ch == '"')
            {
                state = 3;
                fputc(ch, output);
            }
            else if (ch == '\'')
            {
                state = 4;
                fputc(ch, output);
            }
            else
            {
                fputc(ch, output);
            }
        }

        else if (state == 1)
        {
            if (ch == '\n')
            {
                fputc('\n', output);
                state = 0;
            }
        }

        else if (state == 2)
        {
            if (ch == '\n')
            {
                fputc('\n', output);
            }

            if (ch == '*')
            {
                next = fgetc(input);

                if (next == '/')
                {
                    state = 0;
                }
                else if (next != EOF)
                {
                    ungetc(next, input);
                }
            }
        }

        else if (state == 3)
        {
            fputc(ch, output);

            if (ch == '\\')
            {
                next = fgetc(input);

                if (next != EOF)
                    fputc(next, output);
            }
            else if (ch == '"')
            {
                state = 0;
            }
        }

        else if (state == 4)
        {
            fputc(ch, output);

            if (ch == '\\')
            {
                next = fgetc(input);

                if (next != EOF)
                    fputc(next, output);
            }
            else if (ch == '\'')
            {
                state = 0;
            }
        }
    }
}

