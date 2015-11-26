---
scalars:
    uint8:
        c_type: uint8_t
        c_parse_type: unsigned

    int8:
        c_type: int8_t
        c_parse_type: signed

    uint16:
        c_type: uint16_t
        c_parse_type: unsigned

    int16:
        c_type: int16_t
        c_parse_type: signed

    uint32:
        c_type: uint32_t
        c_parse_type: unsigned

    int32:
        c_type: int32_t
        c_parse_type: signed

    float32:
        c_type: float
        c_parse_type: float

    float64:
        c_type: double
        c_parse_type: double

c_scalar_parse_fns:
    unsigned: xx_strtoul
    signed: xx_strtol
    float: strtof
    double: strtod

c_scalar_type_traits:
    uint8_t:
        min: 0
        max: UINT8_MAX
        format_str: "%d"

    int8_t:
        min: INT8_MIN
        max: INT8_MAX
        format_str: "%d"

    uint16_t:
        min: 0
        max: UINT16_MAX
        format_str: "%d"

    int16_t:
        min: INT16_MIN
        max: INT16_MAX
        format_str: "%d"

    uint32_t:
        min: 0
        max: UINT32_MAX
        format_str: "%d"

    int32_t:
        min: INT32_MIN
        max: INT32_MAX
        format_str: "%d"

    float:
        format_str: "%f"

    double:
        format_str: "%f"
---
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

struct xx_option {
    const char *name;
    int (*handler)(int argc, char **argv);
};

#define xx_die(error, args...) \
    fprintf(stderr, error, ##args); \
    fputc('\n', stderr); \
    exit(-1)

#define xx_calloc_auto(n, ptr) (calloc(n, sizeof(*ptr)))

#define xx_fread_auto(ptr, n, f) (fread(ptr, sizeof(*ptr), n, f))

#define xx_fwrite_auto(ptr, n, f) (fwrite(ptr, sizeof(*ptr), n, f))

int xx_strprefixed(const char *str, const char *prefix) {
    return (strncmp(prefix, str, strlen(prefix)) == 0);
}

signed xx_strtol(const char *nptr, char **endptr) {
    return strtol(nptr, endptr, 0);
}

unsigned xx_strtoul(const char *nptr, char **endptr) {
    return strtoul(nptr, endptr, 0);
}

{{#each scalars}}
    {{c_type}} xx_strto{{@key}}(const char *str) {
        if(str[0] == 0) {
            goto parse_error;
        }

        errno = 0;

        const char *endptr;

        {{c_parse_type}} value = {{get ../c_scalar_parse_fns c_parse_type}}(
            str, (char **)(&endptr)
        );

        if(
            errno != 0
            || *endptr != 0

            {{#if (defined (get ../c_scalar_type_traits c_type "min"))}}
                || value < {{get ../c_scalar_type_traits c_type "min"}}
            {{/if}}

            {{#if (defined (get ../c_scalar_type_traits c_type "max"))}}
                || value > {{get ../c_scalar_type_traits c_type "max"}}
            {{/if}}
        ) {
            goto parse_error;
        }

        return ({{c_type}})(value);

        parse_error:
        xx_die("'%s' can't be {{@key}}.", str);
    }
{{/each}}

{{#each scalars}}
    int xx_read_{{@key}}(int argc, char **argv) {
        int consumed = 0;

        int n = 1;

        if(argc >= 1 && !xx_strprefixed(argv[0], "--")) {
            n = xx_strtoint32(argv[0]);
            assert(n >= 0);

            ++consumed;
        }

        if(n == 0) {
            return consumed;
        }

        {{c_type}} *buf = xx_calloc_auto(n, buf);
        assert(buf);

        assert(xx_fread_auto(buf, n, stdin) == n);

        {{set 'c_type_format_str' (get ../c_scalar_type_traits c_type 'format_str')}}

        for(int i = 0; i < n; ++i) {
            printf("{{c_type_format_str}}", buf[i]);

            if(i < n - 1) {
                printf(" ");
            }
        }

        printf("\n");

        free(buf);

        return consumed;
    }
{{/each}}

int xx_read_pcstr(int argc, char **argv) {
    assert(argc >= 1);

    int field_len = xx_strtoint32(argv[0]);

    char *buf = 0;
    size_t buf_len = 0;

    ssize_t read_len = getdelim(&buf, &buf_len, 0, stdin);
    assert(read_len != -1);

    for(int i = read_len; i < field_len; ++i) {
        assert(fgetc(stdin) != EOF);
    }

    puts(buf);

    free(buf);

    return 1;
}

int xx_read_cstr(int argc, char **argv) {
    char *buf = 0;
    size_t buf_len = 0;

    assert(getdelim(&buf, &buf_len, 0, stdin) != -1);

    puts(buf);

    free(buf);

    return 0;
}

int xx_read_char(int argc, char **argv) {
    int consumed = 0;

    int n = 1;

    if(argc >= 1 && !xx_strprefixed(argv[0], "--")) {
        n = (int)(xx_strtouint32(argv[0]));
        ++consumed;
    }

    if(n == 0) {
        return consumed;
    }

    char *buf = xx_calloc_auto(n, buf);
    assert(buf);

    assert(xx_fread_auto(buf, n, stdin) == n);

    puts(buf);

    free(buf);

    return consumed;
}

struct xx_option xx_read_options[] = {
    {{#each scalars}}
        {
            .name = "--{{@key}}",
            .handler = xx_read_{{@key}}
        },
    {{/each}}
    {
        .name = "--pcstr",
        .handler = xx_read_pcstr
    },
    {
        .name = "--cstr",
        .handler = xx_read_cstr
    },
    {
        .name = "--char",
        .handler = xx_read_char
    }
};

int xx_read_option_count = (
    sizeof(xx_read_options) / sizeof(xx_read_options[0])
);

void xx_read(int argc, char **argv) {
    setvbuf(stdin, 0, _IONBF, 0);

    for(int i = 0; i < argc; ++i) {
        const char *name = argv[i];

        for(int j = 0; j < xx_read_option_count; ++j) {
            struct xx_option *option = &xx_read_options[j];

            if(strcmp(option->name, name) == 0) {
                i += option->handler(argc - i - 1, argv + i + 1);
                goto next_arg;
            }
        }

        xx_die("Invalid read option: %s\n", name);

        next_arg:;
    }
}

FILE *f;

{{#each scalars}}
    int xx_write_{{@key}}(int argc, char **argv) {
        assert(argc >= 1);

        int i;

        for(i = 0; i < argc; ++i) {
            if(xx_strprefixed(argv[i], "--")) {
                assert(i > 0);
                break;
            }

            {{c_type}} value = xx_strto{{@key}}(argv[i]);

            assert(xx_fwrite_auto(&value, 1, f) == 1);
        }

        return i;
    }
{{/each}}

int xx_write_pcstr(int argc, char **argv) {
    assert(argc >= 2);

    unsigned field_len = xx_strtouint32(argv[0]);
    size_t str_len = strlen(argv[1]);

    if(field_len <= str_len) {
        xx_die("'%s' doesn't fit in %d bytes as a pcstr.", argv[1], field_len);
    }

    assert(fwrite(argv[1], str_len + 1, 1, f) == 1);

    for(int i = str_len + 1; i < field_len; ++i) {
        assert(fputc(0, f) != EOF);
    }

    return 2;
}

int xx_write_cstr(int argc, char **argv) {
    assert(argc >= 1);

    assert(fwrite(argv[0], strlen(argv[0]) + 1, 1, f) == 1);

    return 1;
}

int xx_write_char(int argc, char **argv) {
    assert(argc >= 1);

    assert(fwrite(argv[0], strlen(argv[0]), 1, f) == 1);

    return 1;
}

struct xx_option xx_write_options[] = {
    {{#each scalars}}
        {
            .name = "--{{@key}}",
            .handler = xx_write_{{@key}}
        },
    {{/each}}
    {
        .name = "--pcstr",
        .handler = xx_write_pcstr
    },
    {
        .name = "--cstr",
        .handler = xx_write_cstr
    },
    {
        .name = "--char",
        .handler = xx_write_char
    }
};

int xx_write_option_count = (
    sizeof(xx_write_options) / sizeof(xx_write_options[0])
);

void xx_flush() {
    fseek(f, 0, SEEK_SET);

    for(int c; c = fgetc(f), c != EOF;) {
        assert(fputc(c, stdout) != EOF);
    }
}

void xx_write(int argc, char **argv) {
    if(isatty(fileno(stdout))) {
        xx_die("stdout is a TTY.");
    }

    f = tmpfile();
    assert(f);

    for(int i = 0; i < argc; ++i) {
        const char *name = argv[i];

        for(int j = 0; j < xx_write_option_count; ++j) {
            struct xx_option *option = &xx_write_options[j];

            if(strcmp(option->name, name) == 0) {
                i += option->handler(argc - i - 1, argv + i + 1);
                goto next_arg;
            }
        }

        xx_die("Invalid write option: %s\n", name);

        next_arg:;
    }

    xx_flush();
}

int main(int argc, char **argv) {
    if(argc < 2) {
        xx_die("Missing arguments.");
    }

    if(strcmp(argv[1], "-r") == 0) {
        xx_read(argc - 2, argv + 2);
    }
    else {
        xx_write(argc - 1, argv + 1);
    }
}
