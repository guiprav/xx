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


#define xx_fwrite_auto(ptr, n, f) (fwrite(ptr, sizeof(*ptr), n, f))

void xx_read(int argc, char **argv) {
    xx_die("Not implemented.");
}

const char *xx_strtok(char *str) {
    const char *token = strtok(str, ",");

    if(str != 0 && token == 0) {
        token = "";
    }

    return token;
}

uint8_t xx_strtouint8(const char *str) {
    if(str[0] == 0) {
        goto parse_error;
    }

    errno = 0;

    const char *endptr;
    unsigned value = strtoul(str, (char **)(&endptr), 0);

    if(errno != 0 || *endptr != 0 || value > UINT8_MAX) {
        goto parse_error;
    }

    return (uint8_t)(value);

    parse_error:
    xx_die("'%s' can't be uint8.", str);
}

int8_t xx_strtoint8(const char *str) {
    if(str[0] == 0) {
        goto parse_error;
    }

    errno = 0;

    const char *endptr;
    int value = strtol(str, (char **)(&endptr), 0);

    if(errno != 0 || *endptr != 0 || value < INT8_MIN || value > INT8_MAX) {
        goto parse_error;
    }

    return (int8_t)(value);

    parse_error:
    xx_die("'%s' can't be int8.", str);
}

uint16_t xx_strtouint16(const char *str) {
    if(str[0] == 0) {
        goto parse_error;
    }

    errno = 0;

    const char *endptr;
    unsigned value = strtoul(str, (char **)(&endptr), 0);

    if(errno != 0 || *endptr != 0 || value > UINT16_MAX) {
        goto parse_error;
    }

    return (uint16_t)(value);

    parse_error:
    xx_die("'%s' can't be uint16.", str);
}

int16_t xx_strtoint16(const char *str) {
    if(str[0] == 0) {
        goto parse_error;
    }

    errno = 0;

    const char *endptr;
    int value = strtol(str, (char **)(&endptr), 0);

    if(errno != 0 || *endptr != 0 || value < INT16_MIN || value > INT16_MAX) {
        goto parse_error;
    }

    return (int16_t)(value);

    parse_error:
    xx_die("'%s' can't be int16.", str);
}

uint32_t xx_strtouint32(const char *str) {
    if(str[0] == 0) {
        goto parse_error;
    }

    errno = 0;

    const char *endptr;
    unsigned value = strtoul(str, (char **)(&endptr), 0);

    if(errno != 0 || *endptr != 0) {
        goto parse_error;
    }

    return value;

    parse_error:
    xx_die("'%s' can't be uint32.", str);
}

int32_t xx_strtoint32(const char *str) {
    if(str[0] == 0) {
        goto parse_error;
    }

    errno = 0;

    const char *endptr;
    int value = strtol(str, (char **)(&endptr), 0);

    if(errno != 0 || *endptr != 0) {
        goto parse_error;
    }

    return value;

    parse_error:
    xx_die("'%s' can't be int32.", str);
}

float xx_strtofloat32(const char *str) {
    if(str[0] == 0) {
        goto parse_error;
    }

    errno = 0;

    const char *endptr;
    float value = strtof(str, (char **)(&endptr));

    if(errno != 0 || *endptr != 0) {
        goto parse_error;
    }

    return value;

    parse_error:
    xx_die("'%s' can't be float32.", str);
}

double xx_strtofloat64(const char *str) {
    if(str[0] == 0) {
        goto parse_error;
    }

    errno = 0;

    const char *endptr;
    double value = strtod(str, (char **)(&endptr));

    if(errno != 0 || *endptr != 0) {
        goto parse_error;
    }

    return value;

    parse_error:
    xx_die("'%s' can't be float64.", str);
}

FILE *f;

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

int xx_write_uint8(int argc, char **argv) {
    assert(argc >= 1);

    const char *token = xx_strtok(argv[0]);

    do {
        uint8_t value = xx_strtouint8(token);

        assert(xx_fwrite_auto(&value, 1, f) == 1);
    } while(token = xx_strtok(0));

    return 1;
}

int xx_write_int8(int argc, char **argv) {
    assert(argc >= 1);

    const char *token = xx_strtok(argv[0]);

    do {
        int8_t value = xx_strtoint8(token);

        assert(xx_fwrite_auto(&value, 1, f) == 1);
    } while(token = xx_strtok(0));

    return 1;
}

int xx_write_uint16(int argc, char **argv) {
    assert(argc >= 1);

    const char *token = xx_strtok(argv[0]);

    do {
        uint16_t value = xx_strtouint16(token);

        assert(xx_fwrite_auto(&value, 1, f) == 1);
    } while(token = xx_strtok(0));

    return 1;
}

int xx_write_int16(int argc, char **argv) {
    assert(argc >= 1);

    const char *token = xx_strtok(argv[0]);

    do {
        int16_t value = xx_strtoint16(token);

        assert(xx_fwrite_auto(&value, 1, f) == 1);
    } while(token = xx_strtok(0));

    return 1;
}

int xx_write_uint32(int argc, char **argv) {
    assert(argc >= 1);

    const char *token = xx_strtok(argv[0]);

    do {
        uint32_t value = xx_strtouint32(token);

        assert(xx_fwrite_auto(&value, 1, f) == 1);
    } while(token = xx_strtok(0));

    return 1;
}

int xx_write_int32(int argc, char **argv) {
    assert(argc >= 1);

    const char *token = xx_strtok(argv[0]);

    do {
        int32_t value = xx_strtoint32(token);

        assert(xx_fwrite_auto(&value, 1, f) == 1);
    } while(token = xx_strtok(0));

    return 1;
}

int xx_write_float32(int argc, char **argv) {
    assert(argc >= 1);

    const char *token = xx_strtok(argv[0]);

    do {
        float value = xx_strtofloat32(token);

        assert(xx_fwrite_auto(&value, 1, f) == 1);
    } while(token = xx_strtok(0));

    return 1;
}

int xx_write_float64(int argc, char **argv) {
    assert(argc >= 1);

    const char *token = xx_strtok(argv[0]);

    do {
        double value = xx_strtofloat64(token);

        assert(xx_fwrite_auto(&value, 1, f) == 1);
    } while(token = xx_strtok(0));

    return 1;
}

struct xx_option xx_write_options[] = {
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
    },
    {
        .name = "--uint8",
        .handler = xx_write_uint8
    },
    {
        .name = "--int8",
        .handler = xx_write_int8
    },
    {
        .name = "--uint16",
        .handler = xx_write_uint16
    },
    {
        .name = "--int16",
        .handler = xx_write_int16
    },
    {
        .name = "--uint32",
        .handler = xx_write_uint32
    },
    {
        .name = "--int32",
        .handler = xx_write_int32
    },
    {
        .name = "--float32",
        .handler = xx_write_float32
    },
    {
        .name = "--float64",
        .handler = xx_write_float64
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
