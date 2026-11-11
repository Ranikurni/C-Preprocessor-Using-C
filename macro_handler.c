
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "macro_handler.h"

#define MAX_MACROS 100
#define MAX_PARAMS 8

struct Macro
{
    char name[64];
    char parameters[MAX_PARAMS][64];
    char value[512];
    int param_count;
    int is_function;
};

static struct Macro macros[MAX_MACROS];
static int macro_count = 0;

/* Find macro */
static int find_macro(const char *name)
{
    int i;

    for (i = 0; i < macro_count; i++)
    {
        if (strcmp(macros[i].name, name) == 0)
            return i;
    }

    return -1;
}

/* Remove leading and trailing spaces */
static void trim(char *str)
{
    int start = 0;
    int end = strlen(str);

    while (isspace((unsigned char)str[start]))
        start++;

    while (end > start &&
           isspace((unsigned char)str[end - 1]))
        end--;

    memmove(str, str + start, end - start);
    str[end - start] = '\0';
}

/* Store macro definition */
static void store_macro(char *line)
{
    char *p = line + 7;
    char name[64];
    struct Macro m = {0};
    int n = 0;

    while (isspace((unsigned char)*p))
        p++;

    while ((isalnum((unsigned char)*p) || *p == '_')
           && n < 63)
    {
        name[n++] = *p++;
    }

    name[n] = '\0';

    if (n == 0)
        return;

    strcpy(m.name, name);

    /* Function-like macro */
    if (*p == '(')
    {
        m.is_function = 1;
        p++;

        while (*p != ')' && *p != '\0')
        {
            char parameter[64];
            int j = 0;

            while (isspace((unsigned char)*p))
                p++;

            while ((isalnum((unsigned char)*p) ||
                    *p == '_') && j < 63)
            {
                parameter[j++] = *p++;
            }

            parameter[j] = '\0';

            if (j == 0 || m.param_count >= MAX_PARAMS)
                return;

            strcpy(m.parameters[m.param_count], parameter);
            m.param_count++;

            while (isspace((unsigned char)*p))
                p++;

            if (*p == ',')
                p++;
            else if (*p != ')')
                return;
        }

        if (*p != ')')
            return;

        p++;
    }

    while (*p == ' ' || *p == '\t')
        p++;

    strncpy(m.value, p, sizeof(m.value) - 1);
    m.value[strcspn(m.value, "\n")] = '\0';

    int index = find_macro(m.name);

    if (index >= 0)
        macros[index] = m;
    else if (macro_count < MAX_MACROS)
        macros[macro_count++] = m;
}

/* Replace macro parameters */
static void substitute_parameters(
    const char *body,
    struct Macro *m,
    char args[MAX_PARAMS][256],
    char *result)
{
    int i = 0;
    int j = 0;

    while (body[i] != '\0' && j < 1000)
    {
        if (isalpha((unsigned char)body[i]) ||
            body[i] == '_')
        {
            char word[64];
            int k = 0;
            int found = -1;

            while ((isalnum((unsigned char)body[i]) ||
                    body[i] == '_') && k < 63)
            {
                word[k++] = body[i++];
            }

            word[k] = '\0';

            int p;

            for (p = 0; p < m->param_count; p++)
            {
                if (strcmp(word, m->parameters[p]) == 0)
                {
                    found = p;
                    break;
                }
            }

            if (found >= 0)
            {
                int a = 0;

                while (args[found][a] != '\0' && j < 1000)
                    result[j++] = args[found][a++];
            }
            else
            {
                int a = 0;

                while (word[a] != '\0' && j < 1000)
                    result[j++] = word[a++];
            }
        }
        else
        {
            result[j++] = body[i++];
        }
    }

    result[j] = '\0';
}

/* Read function macro arguments */
static int read_arguments(
    const char *line,
    int open,
    char args[MAX_PARAMS][256],
    int *end_pos)
{
    int count = 0;
    int len = 0;
    int depth = 0;
    int i = open + 1;
    int quote = 0;
    int escape = 0;

    memset(args, 0, MAX_PARAMS * 256);

    while (line[i] != '\0')
    {
        char c = line[i];

        if (quote)
        {
            if (len < 255)
                args[count][len++] = c;

            if (escape)
                escape = 0;
            else if (c == '\\')
                escape = 1;
            else if (c == quote)
                quote = 0;

            i++;
            continue;
        }

        if (c == '"' || c == '\'')
        {
            quote = c;
            if (len < 255)
                args[count][len++] = c;
        }
        else if (c == '(')
        {
            depth++;
            if (len < 255)
                args[count][len++] = c;
        }
        else if (c == ')')
        {
            if (depth > 0)
            {
                depth--;
                if (len < 255)
                    args[count][len++] = c;
            }
            else
            {
                if (len > 0 || count > 0)
                {
                    args[count][len] = '\0';
                    trim(args[count]);
                    count++;
                }

                *end_pos = i + 1;
                return count;
            }
        }
        else if (c == ',' && depth == 0)
        {
            if (count >= MAX_PARAMS - 1)
                return -1;

            args[count][len] = '\0';
            trim(args[count]);
            count++;
            len = 0;
        }
        else
        {
            if (len < 255)
                args[count][len++] = c;
        }

        i++;
    }

    return -1;
}

/* Expand macros */



static void expand_macros_depth(char *line, FILE *output, int depth)
{
    int i = 0;

    if (depth > 20)
    {
        fputs(line, output);
        return;
    }

    while (line[i] != '\0')
    {
        if (line[i] == '"' || line[i] == '\'')
        {
            char quote = line[i];

            fputc(line[i++], output);

            while (line[i] != '\0')
            {
                char c = line[i++];
                fputc(c, output);

                if (c == '\\' && line[i] != '\0')
                    fputc(line[i++], output);
                else if (c == quote)
                    break;
            }

            continue;
        }

        if (isalpha((unsigned char)line[i]) || line[i] == '_')
        {
            char word[64];
            int j = 0;

            while ((isalnum((unsigned char)line[i]) ||
                    line[i] == '_') && j < 63)
            {
                word[j++] = line[i++];
            }

            word[j] = '\0';

            int index = find_macro(word);

            if (index < 0)
            {
                fputs(word, output);
                continue;
            }

            struct Macro *m = &macros[index];

            if (!m->is_function)
            {
                expand_macros_depth(m->value, output, depth + 1);
                continue;
            }

            int k = i;

            while (isspace((unsigned char)line[k]))
                k++;

            if (line[k] == '(')
            {
                char args[MAX_PARAMS][256];
                char replacement[1024];
                int end_pos;

                int count = read_arguments(
                    line, k, args, &end_pos);

                if (count == m->param_count && count >= 0)
                {
                    substitute_parameters(
                        m->value, m, args, replacement);

                    expand_macros_depth(
                        replacement, output, depth + 1);

                    i = end_pos;
                }
                else
                {
                    fputs(word, output);
                }
            }
            else
            {
                fputs(word, output);
            }
        }
        else
        {
            fputc(line[i], output);
            i++;
        }
    }
}

static void expand_macros(char *line, FILE *output)
{
    expand_macros_depth(line, output, 0);
}




/* Main macro processing function */
void process_macros(FILE *input, FILE *output)
{
    char line[2048];

    macro_count = 0;

    while (fgets(line, sizeof(line), input) != NULL)
    {
        char *p = line;

        while (isspace((unsigned char)*p))
            p++;

        if (strncmp(p, "#define", 7) == 0 &&
            (p[7] == ' ' || p[7] == '\t'))
        {
            store_macro(p);
        }
        else
        {
            expand_macros(line, output);
        }
    }
}

