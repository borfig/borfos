/*
 *  borfos - a minimal OS for x86
 *  Copyright (C) 2013 Boris Figovsky.
 *
 *  borfos is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  borfos is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with borfos.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <bprintf.h>

#include <string.h>

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    unsigned alternative : 1;
    unsigned zero_padd : 1;
    unsigned left_adjust : 1;
    unsigned space : 1;
    unsigned sign : 1;
} print_flags_t;

typedef enum {
    pl_none,
    pl_char,
    pl_short,
    pl_long,
    pl_longlong,
} print_length_t;

typedef enum {
    pu_decimal,
    pu_octal,
    pu_hex,
    pu_HEX,
} print_unsigned_t;

size_t vbprintf(char *buffer, size_t max, const char *format, va_list ap)
{
    char *buffer_start = buffer;

    while ('\0' != *format && 0 < max) {
        if ('%' != *format) {
            *buffer = *format;
            ++buffer;
            ++format;
            --max;
            continue;
        }
        ++format;
        if ('%' == *format) {
            *buffer = *format;
            ++buffer;
            ++format;
            --max;
            continue;
        }
        unsigned min_width = 0, precision = 0;
        print_length_t length = pl_none;
        bool alternative = false;
        bool zero_padd = false;
        bool left_adjust = false;
        bool space = false;
        bool sign = false;
        bool has_precision = false;

        for (;; ++format) {
            switch (*format) {
            case '#': alternative = true; break;
            case '0': zero_padd = true; break;
            case '-': left_adjust = true; break;
            case ' ': space = true; break;
            case '+': sign = true; break;
            default: goto DoneFlags;
            }
        }
    DoneFlags:

        if ('*' == *format) {
            /* TODO add *m$ support */
            int width = va_arg(ap, int);
            if (width < 0) {
                left_adjust = true;
                min_width = -width;
            }
            else {
                min_width = width;
            }
            ++format;
        }
        else {
            for (; '0' <= *format && *format <= '9'; ++format) {
                min_width *= 10;
                min_width += *format - '0';
            }
        }

        if ('.' == *format) {
            ++format;
            if ('*' == *format) {
            /* TODO add *.m$ support */
                precision = va_arg(ap, int);
                ++format;
            }
            else {
                for (; '0' <= *format && *format <= '9'; ++format) {
                    precision *= 10;
                    precision += *format - '0';
                }
            }
            has_precision = true;
        }

        switch (*format) {
        case 'h':
            ++format;
            if ('h' == *format) {
                length = pl_char;
                ++format;
            }
            else {
                length = pl_short;
            }
            break;
        case 'l':
            ++format;
            if ('l' == *format) {
                length = pl_longlong;
                ++format;
            }
            else {
                length = pl_long;
            }
            break;
        default:
            break;
        }

        char digits[23];
        char *digits_top = digits;
        print_unsigned_t print_unsigned = pu_decimal;
        bool is_negative = false;
        switch (*format++) {
        case 'd':
        case 'i':
            if (pl_longlong == length) {
                long long num = va_arg(ap, long long);
                if (0x8000000000000000 == (unsigned long long)num) {
                    is_negative = true;
                    *(digits_top++) = '8';
                    *(digits_top++) = '0';
                    *(digits_top++) = '8';
                    *(digits_top++) = '5';
                    *(digits_top++) = '7';
                    *(digits_top++) = '7';
                    *(digits_top++) = '4';
                    *(digits_top++) = '5';
                    *(digits_top++) = '8';
                    *(digits_top++) = '6';
                    *(digits_top++) = '3';
                    *(digits_top++) = '0';
                    *(digits_top++) = '2';
                    *(digits_top++) = '7';
                    *(digits_top++) = '3';
                    *(digits_top++) = '3';
                    *(digits_top++) = '2';
                    *(digits_top++) = '2';
                    *(digits_top++) = '9';
                }
                else {
                    if (num < 0) {
                        is_negative = true;
                        num = -num;
                    }
                    if (num) {
                        do {
                            *(digits_top++) = '0' + (num % 10);
                            num /= 10;
                        } while(num);
                    } else if (!has_precision) {
                        *(digits_top++) = '0';
                    }
                }
            } else {
                long num = va_arg(ap, long);
                switch(length) {
                case pl_short:
                    num = (short)num;
                    break;
                case pl_char:
                    num = (char)num;
                    break;
                default:
                    break;
                }
                if (0x80000000 == (unsigned long)num) {
                    is_negative = true;
                    *(digits_top++) = '8';
                    *(digits_top++) = '4';
                    *(digits_top++) = '6';
                    *(digits_top++) = '3';
                    *(digits_top++) = '8';
                    *(digits_top++) = '4';
                    *(digits_top++) = '7';
                    *(digits_top++) = '4';
                    *(digits_top++) = '1';
                    *(digits_top++) = '2';
                }
                else {
                    if (num < 0) {
                        is_negative = true;
                        num = -num;
                    }
                    if (num) {
                        do {
                            *(digits_top++) = '0' + (num % 10);
                            num /= 10;
                        } while(num);
                    } else if (!has_precision) {
                        *(digits_top++) = '0';
                    }
                }
            }
            {
                size_t digits_count = digits_top - digits;
                size_t precision_count = 0;
                if (has_precision && digits_count < precision) {
                    precision_count = precision_count - digits_count;
                }
                size_t sign_width = (is_negative || space || sign) ? 1 : 0;
                size_t total_width = digits_count + precision_count + sign_width;
                if (min_width <= total_width || left_adjust) {
                    if (sign_width) {
                        *(buffer++) = is_negative ? '-' :
                            (sign ? '+' : ' ');
                        --max;
                    }
                    if (precision_count > max)
                        precision_count = max;
                    memset(buffer, '0', precision_count);
                    buffer += precision_count;
                    max -= precision_count;
                    while (digits_top != digits && max) {
                        *(buffer++) = *(--digits_top);
                        --max;
                    }
                    if (max && min_width > total_width) {
                        unsigned w = min_width - total_width;
                        if (w > max)
                            w = max;
                        memset(buffer, ' ', w);
                        buffer += w;
                        max -= w;
                    }
                }
                else {
                    unsigned w = min_width - total_width;
                    if (w > max)
                        w = max;
                    memset(buffer, (has_precision || !zero_padd) ? ' ' : '0', w);
                    buffer += w;
                    max -= w;
                    if (max && sign_width) {
                        *(buffer++) = is_negative ? '-' :
                            (sign ? '+' : ' ');
                        --max;
                    }
                    if (precision_count > max)
                        precision_count = max;
                    memset(buffer, '0', precision_count);
                    buffer += precision_count;
                    max -= precision_count;
                    while (digits_top != digits && max) {
                        *(buffer++) = *(--digits_top);
                        --max;
                    }
                }

            }
            break;

        case 'u':
            alternative = false;
            if (pl_longlong == length) {
                unsigned long long num = va_arg(ap, unsigned long long);
                if (num) {
                    do {
                        *(digits_top++) = '0' + (num % 10);
                        num /= 10;
                    } while(num);
                } else if (!has_precision) {
                    alternative = false;
                    *(digits_top++) = '0';
                }
            } else {
                unsigned long num = va_arg(ap, unsigned long);
                switch(length) {
                case pl_short:
                    num = (unsigned short)num;
                    break;
                case pl_char:
                    num = (unsigned char)num;
                    break;
                default:
                    break;
                }
                if (num) {
                    do {
                        *(digits_top++) = '0' + (num % 10);
                        num /= 10;
                    } while(num);
                } else {
                    alternative = false;
                    if (!has_precision)
                        *(digits_top++) = '0';
                }
            }
            goto PrintUnsigned;

        case 'o':
            print_unsigned = pu_octal;
            if (pl_longlong == length) {
                unsigned long long num = va_arg(ap, unsigned long long);
                if (num) {
                    do {
                        *(digits_top++) = '0' + (num & 7);
                        num >>= 3;
                    } while(num);
                } else if (!has_precision) {
                    alternative = false;
                    *(digits_top++) = '0';
                }
            } else {
                unsigned long num = va_arg(ap, unsigned long);
                switch(length) {
                case pl_short:
                    num = (unsigned short)num;
                    break;
                case pl_char:
                    num = (unsigned char)num;
                    break;
                default:
                    break;
                }
                if (num) {
                    do {
                        *(digits_top++) = '0' + (num & 7);
                        num >>= 3;
                    } while(num);
                } else {
                    alternative = false;
                    if (!has_precision)
                        *(digits_top++) = '0';
                }
            }
            goto PrintUnsigned;

        case 'x':
            print_unsigned = pu_hex;
            if (pl_longlong == length) {
                unsigned long long num = va_arg(ap, unsigned long long);
                if (num) {
                    do {
                        unsigned digit = num & 0xF;
                        *(digits_top++) = ((digit < 10) ? '0' : ('a'-10))  + digit;
                        num >>= 4;
                    } while(num);
                } else if (!has_precision) {
                    alternative = false;
                    *(digits_top++) = '0';
                }
            } else {
                unsigned long num = va_arg(ap, unsigned long);
                switch(length) {
                case pl_short:
                    num = (unsigned short)num;
                    break;
                case pl_char:
                    num = (unsigned char)num;
                    break;
                default:
                    break;
                }
                if (num) {
                    do {
                        unsigned digit = num & 0xF;
                        *(digits_top++) = ((digit < 10) ? '0' : ('a'-10))  + digit;
                        num >>= 4;
                    } while(num);
                } else {
                    alternative = false;
                    if (!has_precision)
                        *(digits_top++) = '0';
                }
            }
            goto PrintUnsigned;

        case 'X':
            print_unsigned = pu_HEX;
            if (pl_longlong == length) {
                unsigned long long num = va_arg(ap, unsigned long long);
                if (num) {
                    do {
                        unsigned digit = num & 0xF;
                        *(digits_top++) = ((digit < 10) ? '0' : ('A'-10))  + digit;
                        num >>= 4;
                    } while(num);
                } else if (!has_precision) {
                    alternative = false;
                    *(digits_top++) = '0';
                }
            } else {
                unsigned long num = va_arg(ap, unsigned long);
                switch(length) {
                case pl_short:
                    num = (unsigned short)num;
                    break;
                case pl_char:
                    num = (unsigned char)num;
                    break;
                default:
                    break;
                }
                if (num) {
                    do {
                        unsigned digit = num & 0xF;
                        *(digits_top++) = ((digit < 10) ? '0' : ('A'-10))  + digit;
                        num >>= 4;
                    } while(num);
                } else {
                    alternative = false;
                    if (!has_precision)
                        *(digits_top++) = '0';
                }
            }
            goto PrintUnsigned;

        case 'p':
            {
                uintptr_t num = va_arg(ap, uintptr_t);
                if (num) {
                    do {
                        unsigned digit = num & 0xF;
                        *(digits_top++) = ((digit < 10) ? '0' : ('a'-10))  + digit;
                        num >>= 4;
                    } while(num);
                    alternative = true;
                    print_unsigned = pu_hex;
                } else {
                    *(digits_top++) = ')';
                    *(digits_top++) = 'l';
                    *(digits_top++) = 'i';
                    *(digits_top++) = 'n';
                    *(digits_top++) = '(';
                    alternative = false;
                    zero_padd = false;
                }
            }


        PrintUnsigned:
            {
                size_t digits_count = digits_top - digits;
                size_t precision_count = 0;
                if (has_precision && digits_count < precision) {
                    precision_count = precision_count - digits_count;
                }
                size_t alternative_width = 0;
                if (alternative) {
                    switch(print_unsigned) {
                    case pu_hex:
                    case pu_HEX:
                        alternative_width = 2;
                        break;
                    case pu_octal:
                        alternative_width = 1;
                        break;
                    default:
                        break;
                    }
                }
                size_t total_width = digits_count + precision_count + alternative_width;
                if (min_width <= total_width || left_adjust) {
                    if (alternative_width) {
                        *(buffer++) = '0';
                        --max;
                        if (max && print_unsigned != pu_octal) {
                            *(buffer++) = (print_unsigned == pu_hex) ? 'x' : 'X';
                            --max;
                        }
                    }
                    if (precision_count > max)
                        precision_count = max;
                    memset(buffer, '0', precision_count);
                    buffer += precision_count;
                    max -= precision_count;
                    while (digits_top != digits && max) {
                        *(buffer++) = *(--digits_top);
                        --max;
                    }
                    if (max && min_width > total_width) {
                        unsigned w = min_width - total_width;
                        if (w > max)
                            w = max;
                        memset(buffer, ' ', w);
                        buffer += w;
                        max -= w;
                    }
                }
                else {
                    unsigned w = min_width - total_width;
                    if (w > max)
                        w = max;
                    memset(buffer, (has_precision || !zero_padd) ? ' ' : '0', w);
                    buffer += w;
                    max -= w;
                    if (max && alternative_width) {
                        *(buffer++) = '0';
                        --max;
                        if (max && print_unsigned != pu_octal) {
                            *(buffer++) = (print_unsigned == pu_hex) ? 'x' : 'X';
                            --max;
                        }
                    }
                    if (precision_count > max)
                        precision_count = max;
                    memset(buffer, '0', precision_count);
                    buffer += precision_count;
                    max -= precision_count;
                    while (digits_top != digits && max) {
                        *(buffer++) = *(--digits_top);
                        --max;
                    }
                }

            }
            break;

        case 'c':
            if (min_width <= 1 || left_adjust) {
                *buffer = (char)va_arg(ap, int);
                ++buffer;
                --max;
                if (min_width > 1) {
                    unsigned w = min_width - 1;
                    if (w > max)
                        w = max;
                    memset(buffer, ' ', w);
                    buffer += w;
                    max -= w;
                }
            }
            else { /* right adjust */
                unsigned w = min_width - 1;
                if (w > max)
                    w = max;
                memset(buffer, ' ', w);
                buffer += w;
                max -= w;
                if (0 < max) {
                    *buffer = (char)va_arg(ap, int);
                    ++buffer;
                    --max;
                }
            }
            break;

        case 's':
            {
                const char *str = va_arg(ap, const char *);
                if (0 == min_width || left_adjust) {
                    /* copy at most max bytes from string, and record string length. */
                    const char *p = str;
                    while(*p && 0 < max) {
                        *(buffer++) = *(p++);
                        --max;
                    }
                    if (0 < max) {
                        size_t l = p - str;
                        if (min_width > l) {
                            unsigned w = min_width - l;
                            if (w > max)
                                w = max;
                            memset(buffer, ' ', w);
                            buffer += w;
                            max -= w;
                        }
                    }
                }
                else {
                    size_t len = strlen(str);
                    if (min_width > len) {
                        unsigned w = min_width - len;
                        if (w > max)
                            w = max;
                        memset(buffer, ' ', w);
                        buffer += w;
                        max -= w;
                    }
                    if (len > max)
                        len = max;
                    memcpy(buffer, str, len);
                    buffer += len;
                    max -= len;
                }
            }
            break;

        case 'n':
            *va_arg(ap, int*) = buffer - buffer_start;
            break;

        default:
            break;
        }

    }
    return buffer - buffer_start;
}

size_t bprintf(char *buffer, size_t max, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    size_t r = vbprintf(buffer, max, format, ap);
    va_end(ap);
    return r;
}
