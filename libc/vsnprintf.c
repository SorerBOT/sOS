#include <stdio.h>
#include <types.h>
#include <stdbool.h>
#include <string.h>

#define VSNPRINTF_DIGITS_ASCII_OFFSET 48
#define VSNPRINTF_CONVERT_DIGIT_TO_CHAR(d) (((d) % 10) + VSNPRINTF_DIGITS_ASCII_OFFSET)
#define VSNPRINTF_CONVERT_CHAR_TO_DIGIT(c) ((c) - VSNPRINTF_DIGITS_ASCII_OFFSET)
#define VSNPRINTF_IS_DIGIT(c) ( '0' <= (c) && (c) <= '9' )
#define VSNPRINTF_NULL_STRINGIFIED "(null)"
#define VSNPRINTF_UNKNOWN_STRINGIFIED "(unknown specifier)"
#define VSNPRINTF_CANONICAL_INT_DEFAULT 0xDEADBEEF

typedef enum
{
    VSNPRINTF_LEN_NONE,
    VSNPRINTF_LEN_SHORT,
    VSNPRINTF_LEN_CHAR,
    VSNPRINTF_LEN_INT,
    VSNPRINTF_LEN_LONG,
    VSNPRINTF_LEN_LONG_LONG
} vsnprintf_modifier_length_t;

typedef enum
{
    VSNPRINTF_TYPE_NON_SPECIFIER_PERCENT_SIGN,
    VSNPRINTF_TYPE_UNKNOWN,
    VSNPRINTF_TYPE_DECIMAL,
    VSNPRINTF_TYPE_HEX_LOWERCASE,
    VSNPRINTF_TYPE_HEX_UPPERCASE,
    VSNPRINTF_TYPE_OCTAL,
    VSNPRINTF_TYPE_CHAR,
    VSNPRINTF_TYPE_STRING,
    VSNPRINTF_TYPE_WRITE_COUNT
} vsnprintf_modifier_conversion_t;

typedef struct
{
    vsnprintf_modifier_length_t len;
    vsnprintf_modifier_conversion_t type;
    size_t min_width;
    bool is_zero_padded;
    bool is_signed;
    bool is_valid_specifier;
} vsnprintf_specifier_t;

static inline void get_canonical_int(vsnprintf_specifier_t* specifier_data, va_list* ap_ptr, uintmax_t* canonical_int, bool* is_negative);
static inline int print_specifier_data(char* restrict dst, size_t size, vsnprintf_specifier_t* specifier_data, va_list* ap_ptr);
static inline const char* get_number_flag_value(const char* str, size_t* flag_value);
static inline const char* get_min_width(const char* str, vsnprintf_specifier_t* specifier_data);
static inline vsnprintf_modifier_length_t get_modifier_length_arch_dependent(size_t size_of_arch_dependent_variable);
static inline void get_modifier_conversion(const char* str, vsnprintf_specifier_t* specifier_data);
static inline const char* get_format_specifier(const char* str, vsnprintf_specifier_t* specifier_data);
static int vsnprintf_print_string(char* restrict str, size_t size, const char* restrict src);
static int vsnprintf_print_int(char* restrict str, size_t size, uintmax_t d, bool is_negative);
static int vsnprintf_print_octal(char* restrict str, size_t size, uintmax_t d);
static int vsnprintf_print_hex(char* restrict str, size_t size, uintmax_t d, bool is_uppercase);

static inline void get_canonical_int(vsnprintf_specifier_t* specifier_data, va_list* ap_ptr, uintmax_t* canonical_int, bool* is_negative)
{
    char c;
    short s;
    int i;
    long l;
    long long ll;

    if (specifier_data->is_signed)
    {
        switch (specifier_data->len)
        {
            case VSNPRINTF_LEN_CHAR:
                c = (char) va_arg(*ap_ptr, int);
                if (c < 0)
                {
                    *is_negative = true;
                    *canonical_int = (uintmax_t) -c;
                }
                else
                {
                    *is_negative = false;
                    *canonical_int = (uintmax_t) c;
                }
                return;
                break;

            case VSNPRINTF_LEN_SHORT:
                s = (short) va_arg(*ap_ptr, int);
                if (s < 0)
                {
                    *is_negative = true;
                    *canonical_int = (uintmax_t) -s;
                }
                else
                {
                    *is_negative = false;
                    *canonical_int = s;
                }
                return;
                break;

            case VSNPRINTF_LEN_INT:
                i = va_arg(*ap_ptr, int);
                if (i < 0)
                {
                    *is_negative = true;
                    *canonical_int = (uintmax_t) -i;
                }
                else
                {
                    *is_negative = false;
                    *canonical_int = (uintmax_t) i;
                }
                return;
                break;

            case VSNPRINTF_LEN_LONG:
                l = va_arg(*ap_ptr, long);
                if (l < 0)
                {
                    *is_negative = true;
                    *canonical_int = (uintmax_t) -l;
                }
                else
                {
                    *is_negative = false;
                    *canonical_int = (uintmax_t) l;
                }
                return;
                break;

            case VSNPRINTF_LEN_LONG_LONG:
                ll = va_arg(*ap_ptr, long long);
                if (ll < 0)
                {
                    *is_negative = true;
                    *canonical_int = (uintmax_t) -ll;
                }
                else
                {
                    *is_negative = false;
                    *canonical_int = (uintmax_t) ll;
                }
            case VSNPRINTF_LEN_NONE:
            default:
                *is_negative = false;
                *canonical_int = (uintmax_t) VSNPRINTF_CANONICAL_INT_DEFAULT;
            return;
            break;
        }
    }
    else
    {
        *is_negative = false;
        switch (specifier_data->len)
        {
            case VSNPRINTF_LEN_CHAR:
                *canonical_int = (uintmax_t) (unsigned char) va_arg(*ap_ptr, unsigned int);
                return;
                break;

            case VSNPRINTF_LEN_SHORT:
                *canonical_int = (uintmax_t) (unsigned short) va_arg(*ap_ptr, unsigned int);
                return;
                break;

            case VSNPRINTF_LEN_INT:
                *canonical_int = (uintmax_t) va_arg(*ap_ptr, unsigned int);
                return;
                break;

            case VSNPRINTF_LEN_LONG:
                *canonical_int = (uintmax_t) va_arg(*ap_ptr, unsigned long);
                return;
                break;

            case VSNPRINTF_LEN_LONG_LONG:
                *canonical_int = (uintmax_t) va_arg(*ap_ptr, unsigned long long);
                return;
                break;
            case VSNPRINTF_LEN_NONE:
            default:
                *canonical_int = (uintmax_t) VSNPRINTF_CANONICAL_INT_DEFAULT;
            return;
            break;
        }
    }
}

static inline int print_specifier_data(char* restrict dst, size_t size, vsnprintf_specifier_t* specifier_data, va_list* ap_ptr)
{
    if (specifier_data->type == VSNPRINTF_TYPE_UNKNOWN || specifier_data->len == VSNPRINTF_LEN_NONE)
    {
        return 0;
    }

    vsnprintf_modifier_conversion_t type = specifier_data->type;
    uintmax_t canonical_int;
    bool is_negative;
    if (type == VSNPRINTF_TYPE_DECIMAL || type == VSNPRINTF_TYPE_OCTAL
        || type == VSNPRINTF_TYPE_HEX_LOWERCASE || type == VSNPRINTF_TYPE_HEX_UPPERCASE)
    {
        get_canonical_int(specifier_data, ap_ptr, &canonical_int, &is_negative);
    }

    char* string_arg = NULL;
    if (type == VSNPRINTF_TYPE_STRING)
    {
        string_arg = va_arg(*ap_ptr, char*);
    }

    char char_buf[2] = {0};
    if (type == VSNPRINTF_TYPE_CHAR)
    {
        char_buf[0] = (char) va_arg(*ap_ptr, int);
    }

    char non_specifier_percent_sign_buf[2] = {0};
    if (type == VSNPRINTF_TYPE_NON_SPECIFIER_PERCENT_SIGN)
    {
        non_specifier_percent_sign_buf[0] = '%';
    }

    switch (type)
    {
        case VSNPRINTF_TYPE_DECIMAL:
            return vsnprintf_print_int(dst, size, canonical_int, is_negative);
            break;
        case VSNPRINTF_TYPE_OCTAL:
            return vsnprintf_print_octal(dst, size, canonical_int);
            break;
        case VSNPRINTF_TYPE_HEX_UPPERCASE:
            return vsnprintf_print_hex(dst, size, canonical_int, true);
            break;
        case VSNPRINTF_TYPE_HEX_LOWERCASE:
            return vsnprintf_print_hex(dst, size, canonical_int, false);
            break;
        case VSNPRINTF_TYPE_STRING:
            return vsnprintf_print_string(dst, size, string_arg);
            break;
        case VSNPRINTF_TYPE_CHAR:
            return vsnprintf_print_string(dst, size, char_buf);
            break;
        case VSNPRINTF_TYPE_NON_SPECIFIER_PERCENT_SIGN:
            return vsnprintf_print_string(dst, size, non_specifier_percent_sign_buf);
            break;
        case VSNPRINTF_TYPE_WRITE_COUNT:
        case VSNPRINTF_TYPE_UNKNOWN:
        default:
            return 0;
            break;
    }
}

static inline const char* get_number_flag_value(const char* str, size_t* flag_value)
{
    size_t value = 0;
    for ( ; VSNPRINTF_IS_DIGIT(*str); ++str )
    {
        value *= 10;
        value += VSNPRINTF_CONVERT_CHAR_TO_DIGIT(*str);
    }

    *flag_value = value;
    return str;
}

static inline const char* get_min_width(const char* str, vsnprintf_specifier_t* specifier_data)
{
    if ( !VSNPRINTF_IS_DIGIT(*str) )
    {
        specifier_data->min_width = 0;
        return str;
    }

    if ( *str == '0' )
    {
        specifier_data->is_zero_padded = true;
        ++str;
    }

    str = get_number_flag_value(str, &specifier_data->min_width);

    return str;
}

static inline vsnprintf_modifier_length_t get_modifier_length_arch_dependent(size_t size)
{
    if ( size == sizeof(char) )
    {
        return VSNPRINTF_LEN_CHAR;
    }

    else if ( size == sizeof(short) )
    {
        return VSNPRINTF_LEN_SHORT;
    }

    else if ( size == sizeof(int) )
    {
        return VSNPRINTF_LEN_INT;
    }

    else if ( size == sizeof(long) )
    {
        return VSNPRINTF_LEN_LONG;
    }

    else if ( size == sizeof(long long) )
    {
        return VSNPRINTF_LEN_LONG;
    }

    else return VSNPRINTF_LEN_NONE;
}

static inline void get_modifier_conversion(const char* str, vsnprintf_specifier_t* specifier_data)
{
    switch ( *str )
    {
        case 'd':
        case 'i':
            specifier_data->type = VSNPRINTF_TYPE_DECIMAL;
            specifier_data->is_signed = true;
            return;
            break;
        case 'u':
            specifier_data->type = VSNPRINTF_TYPE_DECIMAL;
            specifier_data->is_signed = false;
            return;
            break;
        case 'p':
            specifier_data->type = VSNPRINTF_TYPE_HEX_UPPERCASE;
            specifier_data->is_signed = false;
            return;
        case 'X':
            specifier_data->type = VSNPRINTF_TYPE_HEX_UPPERCASE;
            specifier_data->is_signed = false;
            return;
            break;
        case 'x':
            specifier_data->type = VSNPRINTF_TYPE_HEX_LOWERCASE;
            specifier_data->is_signed = false;
            return;
            break;
        case 'o':
            specifier_data->type = VSNPRINTF_TYPE_OCTAL;
            specifier_data->is_signed = false;
            return;
            break;
        case 'n':
            specifier_data->type = VSNPRINTF_TYPE_WRITE_COUNT;
            return;
            break;
        case '%':
            specifier_data->type = VSNPRINTF_TYPE_NON_SPECIFIER_PERCENT_SIGN;
            return;
            break;
        case '\0':
        default:
            specifier_data->type = VSNPRINTF_TYPE_UNKNOWN;
            return;
            break;
    }
}


/*
 * Call this as soon as %-sign is encountered in the format string.
 */
static inline const char* get_format_specifier(const char* str, vsnprintf_specifier_t* specifier_data)
{
    memset(specifier_data, 0, sizeof(*specifier_data));
    specifier_data->is_valid_specifier = true;

    if ( *(++str) == '\0' )
    {
        specifier_data->is_valid_specifier = false;
        return str;
    }

    str = get_min_width(str, specifier_data);
    

    if ( *str == 'h' )
    {
        if ( *(++str) == 'h' )
        {
            specifier_data->len = VSNPRINTF_LEN_CHAR;
            ++str;
        }
        else
        {
            specifier_data->len = VSNPRINTF_LEN_SHORT;
        }
    }

    else if ( *str == 'l' )
    {
        if ( *(++str) == 'l' )
        {
            specifier_data->len = VSNPRINTF_LEN_LONG_LONG;
            ++str;
        }
        else
        {
            specifier_data->len = VSNPRINTF_LEN_LONG;
        }
    }

    else if ( *str == 'j' )
    {
        specifier_data->len = get_modifier_length_arch_dependent(sizeof(intmax_t));
        ++str;
    }

    else if ( *str == 't' )
    {
        specifier_data->len = get_modifier_length_arch_dependent(sizeof(ptrdiff_t));
        ++str;
    }

    else if ( *str == 'z' )
    {
        specifier_data->len = get_modifier_length_arch_dependent(sizeof(size_t));
        ++str;
    }
    else if ( *str == 'd' || *str == 'x' || *str == 'X'
                || *str == 'u' || *str == 'o' || *str == 'i'
                || *str == 'n' )
    {
        specifier_data->len = VSNPRINTF_LEN_INT;
    }


    if ( *str == 'c' )
    {
        specifier_data->len = VSNPRINTF_LEN_CHAR;
        specifier_data->type = VSNPRINTF_TYPE_CHAR;
        return ++str;
    }

    if ( *str == 's' )
    {
        specifier_data->len = get_modifier_length_arch_dependent(sizeof(void*));
        specifier_data->type = VSNPRINTF_TYPE_STRING;
        return ++str;
    }

    if ( *str == 'p' )
    {
        specifier_data->len = get_modifier_length_arch_dependent(sizeof(void*));
    }

    get_modifier_conversion(str, specifier_data);

    return ++str;
}

static int vsnprintf_print_string(char* restrict str, size_t size, const char* restrict src)
{
    size_t chars_generated = 0;
    for (; src[chars_generated] != '\0'; ++chars_generated)
    {
        if (chars_generated < size && str != NULL)
        {
            str[chars_generated] = src[chars_generated];
        }
    }

    return chars_generated;
}

static int vsnprintf_print_int(char* restrict str, size_t size, uintmax_t d, bool is_negative)
{
    char digits[21]; // int max is at most 20 digits and null terminator is another
    digits[20] = '\0';

    size_t current_idx = 19;
    do
    {
        int8_t current_digit = d % 10;
        if (current_digit < 0)
        {
            current_digit *= -1;
        }
        digits[current_idx] = VSNPRINTF_CONVERT_DIGIT_TO_CHAR(current_digit);
        --current_idx;
        d /= 10;
    } while (d != 0);

    if (is_negative)
    {
        digits[current_idx] = '-';
        --current_idx;
    }

    ++current_idx; // we decremented it one too many times
    char* final_string = ((char*)digits) + current_idx;
    size_t chars_generated = vsnprintf_print_string(str, size, final_string);

    return chars_generated;
}

static int vsnprintf_print_octal(char* restrict str, size_t size, uintmax_t d)
{
    return vsnprintf_print_int(str, size, d, false);
}
static int vsnprintf_print_hex(char* restrict str, size_t size, uintmax_t d, bool is_uppercase)
{
    return vsnprintf_print_int(str, size, d, false);
}

int vsnprintf(char* restrict str, size_t size, const char* restrict format, va_list ap)
{
    size_t chars_generated = 0;

    char* s;
    char c;
    intmax_t d;

    for (; *format != '\0'; ++format)
    {
        char current = *format;
        uint8_t long_count = 0;

        size_t remaining_size = (chars_generated + 1 < size)
            ? size - 1 - chars_generated
            : 0;

        char* address = remaining_size > 0
            ? str + chars_generated
            : NULL;

        if (current == '%')
        {
            vsnprintf_specifier_t specifier_data;
            format = get_format_specifier(format, &specifier_data);
            switch (current)
            {
                case 's':
                    s = va_arg(ap, char*);
                    if (s == NULL)
                    {
                        s = VSNPRINTF_NULL_STRINGIFIED;
                    }
                    chars_generated += vsnprintf_print_string(address, remaining_size, s);
                    break;
                case 'c':
                    c = (char)va_arg(ap, int);
                    if (address != NULL)
                    {
                        *address = c;
                    }
                    ++chars_generated;
                    break;
                case 'd':
                    if (long_count == 0)
                    {
                        d = va_arg(ap, int);
                    }
                    else if (long_count == 1)
                    {
                        d = va_arg(ap, long);
                    }
                    else if (long_count == 2)
                    {
                        d = va_arg(ap, long long);
                    }

                    chars_generated += vsnprintf_print_int(address, remaining_size, d);

                    break;
            }
        }
        else
        {
            if (address != NULL)
            {
                str[chars_generated] = current;
            }
            ++chars_generated;
        }
    }


    if (size > 0)
    {
        if (chars_generated + 1 < size)
        {
            str[chars_generated] = '\0';
        }
        else
        {
            str[size - 1] = '\0';
        }
    }

    return chars_generated;
}
