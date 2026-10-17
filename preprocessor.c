#include "preprocessor.h"
#include "utils.h"
#include "macro.h"
#include "comments.h"
#include "includes.h"
#include "expand.h"

typedef struct {
    char *data;
    int len;
    int cap;
} Buffer;

static void buf_init(Buffer *b)
{
    b->cap = 4096;
    b->len = 0;
    b->data = (char *)xmalloc(b->cap);
    b->data[0] = '\0';
}

static void buf_ensure(Buffer *b, int need)
{
    while (b->len + need + 1 >= b->cap) {
        b->cap *= 2;
        b->data = (char *)realloc(b->data, b->cap);

        if (b->data == NULL)
            die("out of memory");
    }
}

static void buf_add(Buffer *b, const char *s)
{
    int n = (int)strlen(s);

    buf_ensure(b, n);

    for (int i = 0; i < n; i++)
        b->data[b->len++] = s[i];

    b->data[b->len] = '\0';
}

static void buf_add_char(Buffer *b, char c)
{
    buf_ensure(b, 1);
    b->data[b->len++] = c;
    b->data[b->len] = '\0';
}

static void buf_free(Buffer *b)
{
    free(b->data);
    b->data = NULL;
    b->len = 0;
    b->cap = 0;
}

char *process_file(const char *path);

static void do_include(const char *arg_text, Buffer *out)
{
    while (*arg_text == ' ' || *arg_text == '\t')
        arg_text++;

    int quoted = 0;
    char end_char = 0;

    if (*arg_text == '"') {
        quoted = 1;
        end_char = '"';
        arg_text++;
    } else if (*arg_text == '<') {
        quoted = 0;
        end_char = '>';
        arg_text++;
    }

    char header[MAX_PATH];
    int hk = 0;

    while (*arg_text != '\0' &&
           *arg_text != end_char &&
           hk < MAX_PATH - 1) {
        header[hk++] = *arg_text++;
    }

    header[hk] = '\0';

    char resolved[MAX_PATH];

    if (find_include(header, quoted, resolved, MAX_PATH)) {
        char *sub = process_file(resolved);

        buf_add(out, sub);
        buf_add(out, "\n");

        free(sub);
    } else {
        buf_add(out, "#include ");
        buf_add_char(out, quoted ? '"' : '<');
        buf_add(out, header);
        buf_add_char(out, quoted ? '"' : '>');
        buf_add(out, "\n");
    }
}

char *process_file(const char *path)
{
    if (already_included(path))
        return xstrdup("");

    mark_included(path);

    char *raw = read_file(path);

    if (raw == NULL) {
        fprintf(stderr, "mypreprocessor: cannot open %s\n", path);
        return xstrdup("");
    }

    char *clean = strip_comments(raw);
    free(raw);

    Buffer out;
    buf_init(&out);

    int i = 0;
    int n = (int)strlen(clean);

    /*
     * Conditional compilation state.
     *
     * active == 1:
     *     Current section should be processed.
     *
     * active == 0:
     *     Current section is inside a false conditional block.
     *
     * condition_stack stores the previous active state so
     * nested #ifdef/#ifndef blocks can be handled.
     */
    int active = 1;
    int condition_stack[64];
    int condition_top = 0;

    while (i < n) {
        int start = i;

        while (i < n && clean[i] != '\n')
            i++;

        int end = i;

        char line[MAX_LINE];
        int len = end - start;

        if (len >= MAX_LINE)
            len = MAX_LINE - 1;

        for (int t = 0; t < len; t++)
            line[t] = clean[start + t];

        line[len] = '\0';

        if (i < n && clean[i] == '\n')
            i++;

        int p = 0;

        while (line[p] == ' ' || line[p] == '\t')
            p++;

        /*
         * Preprocessor directive
         */
        if (line[p] == '#') {
            p++;

            while (line[p] == ' ' || line[p] == '\t')
                p++;

            /*
             * #ifndef NAME
             */
            if (strncmp(line + p, "ifndef", 6) == 0) {
                char name[MAX_NAME];

                sscanf(line + p + 6, "%127s", name);

                if (condition_top < 64)
                    condition_stack[condition_top++] = active;

                active = (macro_find(name) == NULL);
            }

            /*
             * #ifdef NAME
             */
            else if (strncmp(line + p, "ifdef", 5) == 0) {
                char name[MAX_NAME];

                sscanf(line + p + 5, "%127s", name);

                if (condition_top < 64)
                    condition_stack[condition_top++] = active;

                active = (macro_find(name) != NULL);
            }

            /*
             * #endif
             */
            else if (strncmp(line + p, "endif", 5) == 0) {
                if (condition_top > 0)
                    active = condition_stack[--condition_top];
            }

            /*
             * If the current conditional section is inactive,
             * ignore all other directives except conditional
             * directives handled above.
             */
            else if (!active) {
                /* Skip inactive directive */
            }

            /*
             * #include
             */
            else if (strncmp(line + p, "include", 7) == 0) {
                do_include(line + p + 7, &out);
            }

            /*
             * #define
             */
            else if (strncmp(line + p, "define", 6) == 0) {
                parse_define(line + p + 6);
            }

            /*
             * #undef
             */
            else if (strncmp(line + p, "undef", 5) == 0) {
                parse_undef(line + p + 5);
            }

            /*
             * Preserve unknown directives.
             */
            else {
                buf_add(&out, line);
                buf_add(&out, "\n");
            }
        }

        /*
         * Normal C source line.
         *
         * Only process it when the current conditional
         * section is active.
         */
        else if (active) {
            char *expanded = expand_line(line);

            buf_add(&out, expanded);
            buf_add(&out, "\n");

            free(expanded);
        }
    }

    free(clean);

    char *result = out.data;
    out.data = NULL;

    buf_free(&out);

    return result;
}

int run_preprocessor(const char *input_file,
                     const char *output_file,
                     int verbose)
{
    macro_list_clear();
    reset_include_dirs();
    reset_included();

    add_include_dir(".");
    apply_pending_include_dirs();

    char outpath[MAX_PATH];

    if (output_file != NULL) {
        strncpy(outpath, output_file, MAX_PATH - 1);
        outpath[MAX_PATH - 1] = '\0';
    } else {
        make_output_name(input_file, outpath, MAX_PATH);
    }

    char *result = process_file(input_file);

    write_file(outpath, result);
    free(result);

    if (verbose)
        printf("[my preprocessor] Wrote extended source to: %s\n",
               outpath);

    return 0;
}
