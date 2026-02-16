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
#define VSNPRINTF_UPPERCASE_ADDITION ('a' - 'A')

#define VSNPRINTF_PREFIX_HEX_LOWERCASE "0x"
#define VSNPRINTF_PREFIX_HEX_UPPERCASE "0X"
#define VSNPRINTF_PREFIX_OCTAL "0"
#define VSNPRINTF_PREFIX_BINARY "0b"

#define VSNPRINTF_IS_ALTERNATE_FORM(flags) (((flags) & VSNPRINTF_FLAG_ALTERNATE_FORM) != 0)
#define VSNPRINTF_IS_ZERO_PAD(flags) (((flags) & VSNPRINTF_FLAG_ZERO_PAD) != 0)
#define VSNPRINTF_IS_PAD_RIGHT(flags) (((flags) & VSNPRINTF_FLAG_PAD_RIGHT) != 0)
#define VSNPRINTF_IS_SPACE(flags) (((flags) & VSNPRINTF_FLAG_SPACE) != 0)
#define VSNPRINTF_IS_PLUS(flags) (((flags) & VSNPRINTF_FLAG_PLUS) != 0)
#define VSNPRINTF_IS_DOT(flags) (((flags) & VSNPRINTF_FLAG_DOT) != 0)

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
    VSNPRINTF_TYPE_INVALID,
    VSNPRINTF_TYPE_NON_SPECIFIER_PERCENT_SIGN,
    VSNPRINTF_TYPE_UNKNOWN,
    VSNPRINTF_TYPE_DECIMAL,
    VSNPRINTF_TYPE_HEX_LOWERCASE,
    VSNPRINTF_TYPE_HEX_UPPERCASE,
    VSNPRINTF_TYPE_OCTAL,
    VSNPRINTF_TYPE_BINARY,
    VSNPRINTF_TYPE_CHAR,
    VSNPRINTF_TYPE_STRING,
    VSNPRINTF_TYPE_WRITE_COUNT
} vsnprintf_modifier_conversion_t;

typedef enum
{
    VSNPRINTF_FLAG_ALTERNATE_FORM   = 1,
    VSNPRINTF_FLAG_ZERO_PAD         = 2,
    VSNPRINTF_FLAG_PAD_RIGHT        = 4,
    VSNPRINTF_FLAG_SPACE            = 8,
    VSNPRINTF_FLAG_PLUS             = 16,
    VSNPRINTF_FLAG_DOT              = 32,
} vsnprintf_flags_t;

typedef struct
{
    size_t min_width;
    size_t dot_flag_param;
    const char* prefix;
    vsnprintf_modifier_length_t len;
    vsnprintf_modifier_conversion_t type;
    vsnprintf_flags_t flags;
    char unknown_specifier_char;
    bool is_signed;
    bool is_valid_specifier;
} vsnprintf_specifier_t;

static inline uint16_t get_highest_contained_power(uintmax_t container, uintmax_t exponentiated_number);
static inline void get_canonical_int(const vsnprintf_specifier_t* specifier_data, va_list* ap_ptr, uintmax_t* canonical_int, bool* is_negative);
static inline int print_specifier_data(char* restrict dst, size_t size, const vsnprintf_specifier_t* specifier_data, va_list* ap_ptr);
static inline const char* get_flags_except_dot(const char* str, vsnprintf_specifier_t* specifier_data);
static inline const char* get_dot_flag(const char* str, vsnprintf_specifier_t* specifier_data, va_list* ap_ptr);
static inline const char* get_flag_number_param(const char* str, size_t* param_ptr, va_list* ap_ptr);
static inline vsnprintf_modifier_length_t get_modifier_length_arch_dependent(size_t size_of_arch_dependent_variable);
static inline void get_modifier_conversion(const char* str, vsnprintf_specifier_t* specifier_data);
static inline const char* get_format_specifier(const char* str, vsnprintf_specifier_t* specifier_data, va_list* ap_ptr);
static inline void print_char(char* restrict str, size_t size, size_t* chars_generated, char c);
static inline void print_prefix(char* restrict str, size_t size, size_t* chars_generated, const char* prefix);
static inline void print_padding(char* restrict str, size_t size, size_t* chars_generated, size_t total_width, size_t min_width, bool is_zero_pad);
static inline bool get_is_type_decimal(vsnprintf_modifier_conversion_t type);
static int vsnprintf_print_string(char* restrict str, size_t size, const char* restrict src, bool is_negative, bool should_print_prefix, const vsnprintf_specifier_t* specifier_data);
static int vsnprintf_print_base_up_to_16(char* restrict str, size_t size, uintmax_t base, uintmax_t d, bool is_uppercase, bool is_negative, bool should_print_prefix, const vsnprintf_specifier_t* specifier_data);

static inline uint16_t get_highest_contained_power(uintmax_t container, uintmax_t exponentiated_number)
{
    size_t current_power = 0;

    for (; container / exponentiated_number >= 1; ++current_power, container /= exponentiated_number);

    return current_power;
}

static inline void get_canonical_int(const vsnprintf_specifier_t* specifier_data, va_list* ap_ptr, uintmax_t* canonical_int, bool* is_negative)
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
                return;
                break;
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

static inline int print_specifier_data(char* restrict dst, size_t size, const vsnprintf_specifier_t* specifier_data, va_list* ap_ptr)
{
    if (specifier_data->type == VSNPRINTF_TYPE_INVALID)
    {
        return 0;
    }

    vsnprintf_modifier_conversion_t type = specifier_data->type;
    uintmax_t canonical_int;
    bool should_print_prefix = VSNPRINTF_IS_ALTERNATE_FORM(specifier_data->flags);
    bool is_negative;
    if (get_is_type_decimal(specifier_data->type))
    {
        get_canonical_int(specifier_data, ap_ptr, &canonical_int, &is_negative);
        if (canonical_int == 0)
        {
            should_print_prefix = false;
        }
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

    char unknown_char_buf[2] = {0};
    if (type == VSNPRINTF_TYPE_UNKNOWN)
    {
        unknown_char_buf[0] = specifier_data->unknown_specifier_char;
    }

    switch (type)
    {
        case VSNPRINTF_TYPE_DECIMAL:
            return vsnprintf_print_base_up_to_16(dst, size, 10, canonical_int, false, is_negative, should_print_prefix, specifier_data);
            break;
        case VSNPRINTF_TYPE_OCTAL:
            return vsnprintf_print_base_up_to_16(dst, size, 8, canonical_int, false, false, should_print_prefix, specifier_data);
            break;
        case VSNPRINTF_TYPE_BINARY:
            return vsnprintf_print_base_up_to_16(dst, size, 2, canonical_int, false, false, should_print_prefix, specifier_data);
            break;
        case VSNPRINTF_TYPE_HEX_UPPERCASE:
            return vsnprintf_print_base_up_to_16(dst, size, 16, canonical_int, true, false, should_print_prefix, specifier_data);
            break;
        case VSNPRINTF_TYPE_HEX_LOWERCASE:
            return vsnprintf_print_base_up_to_16(dst, size, 16, canonical_int, false, false, should_print_prefix, specifier_data);
            break;
        case VSNPRINTF_TYPE_STRING:
            return vsnprintf_print_string(dst, size, string_arg, false, should_print_prefix, specifier_data);
            break;
        case VSNPRINTF_TYPE_CHAR:
            return vsnprintf_print_string(dst, size, char_buf, false, should_print_prefix, specifier_data);
            break;
        case VSNPRINTF_TYPE_NON_SPECIFIER_PERCENT_SIGN:
            return vsnprintf_print_string(dst, size, non_specifier_percent_sign_buf, false, should_print_prefix, specifier_data);
            break;
        case VSNPRINTF_TYPE_WRITE_COUNT:
        case VSNPRINTF_TYPE_UNKNOWN:
            return vsnprintf_print_string(dst, size, unknown_char_buf, false, should_print_prefix, specifier_data);
            break;
        default:
            return 0;
            break;
    }
}

static inline const char* get_flags_except_dot(const char* str, vsnprintf_specifier_t* specifier_data)
{
    for (; ; ++str)
    {
        switch ( *str )
        {
            case '#':
                specifier_data->flags |= VSNPRINTF_FLAG_ALTERNATE_FORM;
                continue;
            case '0':
                specifier_data->flags |= VSNPRINTF_FLAG_ZERO_PAD;
                continue;
            case '-':
                specifier_data->flags |= VSNPRINTF_FLAG_PAD_RIGHT;
                continue;
            case ' ':
                specifier_data->flags |= VSNPRINTF_FLAG_SPACE;
                continue;
            case '+':
                specifier_data->flags |= VSNPRINTF_FLAG_PLUS;
                continue;
            default:
                return str;
        }
    }
}

static inline const char* get_dot_flag(const char* str, vsnprintf_specifier_t* specifier_data, va_list* ap_ptr)
{
    if ( *str == '.' )
    {
        specifier_data->flags |= VSNPRINTF_FLAG_DOT;
        return get_flag_number_param(++str, &specifier_data->dot_flag_param, ap_ptr);
    }

    return str;
}

static inline const char* get_flag_number_param(const char* str, size_t* param_ptr, va_list* ap_ptr)
{
    if ( *str == '*' )
    {
        int32_t signed_value = va_arg(*ap_ptr, int);
        if (signed_value < 0)
        {
            // I also need to apply right-pad
            int64_t lsigned_value = signed_value;
            *param_ptr = (size_t) (-lsigned_value);
        }
        else
        {
            *param_ptr = (size_t) signed_value;
        }

        return str + 1;
    }


    if ( !VSNPRINTF_IS_DIGIT(*str) )
    {
        *param_ptr = 0;
        return str;
    }

    size_t value = 0;
    for ( ; VSNPRINTF_IS_DIGIT(*str); ++str )
    {
        value *= 10;
        value += VSNPRINTF_CONVERT_CHAR_TO_DIGIT(*str);
    }
    *param_ptr = value;

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
            specifier_data->flags |= VSNPRINTF_FLAG_ZERO_PAD;
            specifier_data->min_width = (specifier_data->len == VSNPRINTF_LEN_INT)
                ? 8
                : 16;
            //specifier_data->flags |= VSNPRINTF_FLAG_ALTERNATE_FORM;
            //specifier_data->prefix = VSNPRINTF_PREFIX_HEX_UPPERCASE; // It is implementation dependent. Keeping as an option for now
            return;
        case 'X':
            specifier_data->type = VSNPRINTF_TYPE_HEX_UPPERCASE;
            specifier_data->is_signed = false;
            if (VSNPRINTF_IS_ALTERNATE_FORM(specifier_data->flags))
            {
                specifier_data->prefix = VSNPRINTF_PREFIX_HEX_UPPERCASE;  // <---- only in alternative form (#)
            }
            return;
            break;
        case 'x':
            specifier_data->type = VSNPRINTF_TYPE_HEX_LOWERCASE;
            specifier_data->is_signed = false;
            if (VSNPRINTF_IS_ALTERNATE_FORM(specifier_data->flags))
            {
                specifier_data->prefix = VSNPRINTF_PREFIX_HEX_LOWERCASE; // <-------- only in alternative form
            }
            return;
            break;
        case 'o':
            specifier_data->type = VSNPRINTF_TYPE_OCTAL;
            specifier_data->is_signed = false;
            if (VSNPRINTF_IS_ALTERNATE_FORM(specifier_data->flags))
            {
                specifier_data->prefix = VSNPRINTF_PREFIX_OCTAL; // <-------- only in alternative form
            }
            return;
            break;
        case 'b':
            specifier_data->type = VSNPRINTF_TYPE_BINARY;
            specifier_data->is_signed = false;
            if (VSNPRINTF_IS_ALTERNATE_FORM(specifier_data->flags))
            {
                specifier_data->prefix = VSNPRINTF_PREFIX_BINARY; // <-------- only in alternative form
            }
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
            specifier_data->type = VSNPRINTF_TYPE_INVALID;
            return;
        default:
            specifier_data->type = VSNPRINTF_TYPE_UNKNOWN;
            specifier_data->unknown_specifier_char = *str;
            return;
            break;
    }
}


/*
 * Call this as soon as %-sign is encountered in the format string.
 * RETURN VALUE: returns the last address that is a part of the specifier
 * this function is only called by vsnprintf, and it makes sense because
 * of how the vsnprintf print-loop works.
 */
static inline const char* get_format_specifier(const char* str, vsnprintf_specifier_t* specifier_data, va_list* ap_ptr)
{
    memset(specifier_data, 0, sizeof(*specifier_data));
    specifier_data->is_valid_specifier = true;

    if ( *(++str) == '\0' )
    {
        specifier_data->is_valid_specifier = false;
        return str - 1;
    }

    str = get_flags_except_dot(str, specifier_data);
    str = get_flag_number_param(str, &specifier_data->min_width, ap_ptr);
    str = get_dot_flag(str, specifier_data, ap_ptr);

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
                || *str == 'n' || *str == 'b' )
    {
        specifier_data->len = VSNPRINTF_LEN_INT;
    }


    if ( *str == 'c' )
    {
        specifier_data->len = VSNPRINTF_LEN_CHAR;
        specifier_data->type = VSNPRINTF_TYPE_CHAR;
        return str;
    }

    if ( *str == 's' )
    {
        specifier_data->len = get_modifier_length_arch_dependent(sizeof(void*));
        specifier_data->type = VSNPRINTF_TYPE_STRING;
        return str;
    }

    if ( *str == 'p' )
    {
        specifier_data->len = get_modifier_length_arch_dependent(sizeof(void*));
    }

    get_modifier_conversion(str, specifier_data);

    return str;
}

static void print_char(char* restrict str, size_t size, size_t* chars_generated, char c)
{
    if (*chars_generated < size && str != NULL)
    {
        str[*chars_generated] = c;
        ++(*chars_generated);
    }
}

// TODO: make this use print_char. CBA ATM
static inline void print_prefix(char* restrict str, size_t size, size_t* chars_generated, const char* prefix)
{
    if (prefix == NULL)
    {
        return;
    }

    for (size_t i = 0; prefix[i] != '\0'; ++(*chars_generated), ++i)
    {
        if (*chars_generated < size && str != NULL)
        {
            str[*chars_generated] = prefix[i];
        }
    }
}

static inline void print_padding(char* restrict str, size_t size, size_t* chars_generated, size_t total_width, size_t min_width, bool is_zero_pad)
{
    if (total_width < min_width)
    {
        size_t padding_width = min_width - total_width;
        char padding_char = is_zero_pad ? '0' : ' ';

        for (size_t i = 0; i < padding_width; ++i, ++(*chars_generated))
        {
            if (*chars_generated < size && str != NULL)
            {
                str[*chars_generated] = padding_char;
            }
        }
    }
}

static inline bool get_is_type_decimal(vsnprintf_modifier_conversion_t type)
{
    return (type == VSNPRINTF_TYPE_DECIMAL || type == VSNPRINTF_TYPE_OCTAL
        || type == VSNPRINTF_TYPE_HEX_LOWERCASE || type == VSNPRINTF_TYPE_HEX_UPPERCASE
        || type == VSNPRINTF_TYPE_BINARY);
}

static int vsnprintf_print_string(char* restrict str, size_t size, const char* restrict src, bool is_negative, bool should_print_prefix, const vsnprintf_specifier_t* specifier_data)
{
    size_t chars_generated = 0;

    bool is_space_or_plus = false;
    char space_or_plus_char;

    bool is_plus_positive_signed = VSNPRINTF_IS_PLUS(specifier_data->flags);
    bool is_positive_signed = specifier_data->is_signed && !is_negative;
    is_space_or_plus = is_plus_positive_signed && is_positive_signed;
    if (is_space_or_plus)
    {
        space_or_plus_char = '+';
    }
    else
    {
        bool is_space_positive_signed = VSNPRINTF_IS_SPACE(specifier_data->flags);
        bool is_positive_signed = specifier_data->is_signed && !is_negative;
        is_space_or_plus = is_space_positive_signed && is_positive_signed;
        if (is_space_or_plus)
        {
            space_or_plus_char = ' ';
        }
    }

    size_t width = strlen(src);

    bool is_dot_flag = VSNPRINTF_IS_DOT(specifier_data->flags);
    size_t dot_flag_param = specifier_data->dot_flag_param;
    size_t precision_max_string_len = width;
    size_t precision_min_number_digits = 0;
    bool should_print_data = true;

    if (is_dot_flag)
    {
        if ( get_is_type_decimal(specifier_data->type) )
        {
            precision_min_number_digits = dot_flag_param;
            if ( precision_min_number_digits == 0 && width == 1 && src[0] == '0' )
            {
                should_print_data = false;
            }
        }
        else if ( specifier_data->type == VSNPRINTF_TYPE_STRING )
        {
            precision_max_string_len = dot_flag_param;
        }
    }
    size_t precision_padding_width = (width < precision_min_number_digits)
        ? precision_min_number_digits - width
        : 0;

    size_t min_width = specifier_data->min_width;
    bool is_pad_right = VSNPRINTF_IS_PAD_RIGHT(specifier_data->flags);
    bool is_zero_pad = !is_pad_right && VSNPRINTF_IS_ZERO_PAD(specifier_data->flags);

    const char* prefix = specifier_data->prefix;
    size_t prefix_width = (prefix == NULL) ? 0 : strlen(prefix);
    size_t space_width = (is_space_or_plus) ? 1 : 0;
    size_t sign_width = (is_negative) ? 1 : 0;
    size_t total_width = (width + prefix_width
                            + space_width + sign_width
                            + precision_padding_width);


    if (is_zero_pad)
    {
        if (is_negative)
        {
            print_char(str, size, &chars_generated, '-');
        }
        if (is_space_or_plus)
        {
            print_char(str, size, &chars_generated, space_or_plus_char);
        }

        if (should_print_prefix)
        {
            print_prefix(str, size, &chars_generated, prefix);
        }
        print_padding(str, size, &chars_generated, total_width, min_width, is_zero_pad);
    }
    else
    {
        if ( !is_pad_right )
        {
            print_padding(str, size, &chars_generated, total_width, min_width, is_zero_pad);
        }
        if (is_negative)
        {
            print_char(str, size, &chars_generated, '-');
        }
        if (is_space_or_plus)
        {
            print_char(str, size, &chars_generated, space_or_plus_char);
        }
        if (should_print_prefix)
        {
            print_prefix(str, size, &chars_generated, prefix);
        }
    }

    if (should_print_data)
    {

        for (size_t i = 0; width + i < precision_min_number_digits; ++i)
        {
            print_char(str, size, &chars_generated, '0');
        }

        for (size_t i = 0; src[i] != '\0' && i < precision_max_string_len; ++chars_generated, ++i)
        {
            if (chars_generated < size && str != NULL)
            {
                str[chars_generated] = src[i];
            }
        }
    }

    if ( is_pad_right )
    {
        print_padding(str, size, &chars_generated, total_width, min_width, is_zero_pad);
    }

    return chars_generated;
}

// works for any base between 2 and 16 inclusive.
static int vsnprintf_print_base_up_to_16(char* restrict str, size_t size, uintmax_t base, uintmax_t d, bool is_uppercase, bool is_negative, bool should_print_prefix, const vsnprintf_specifier_t* specifier_data)
{
    char buf[67]; // even in base 2, the largest number would be 64-bits, then we have a null terminator and a - (minus) sign

    uint8_t ascii_addition = is_uppercase ? 0 : VSNPRINTF_UPPERCASE_ADDITION;
    size_t chars_generated = 0;

    uint16_t highest_power = get_highest_contained_power(d, base);
    uintmax_t base_to_highest_power = 1;
    for (size_t i = 0; i < highest_power; ++i)
    {
        base_to_highest_power *= base;
    }


    for (; base_to_highest_power > 0; ++chars_generated, base_to_highest_power /= base)
    {
        uint16_t quotient = d / base_to_highest_power;
        if (quotient <= 9)
        {
            buf[chars_generated] = VSNPRINTF_CONVERT_DIGIT_TO_CHAR(quotient);
        }
        else
        {
            switch (quotient)
            {
                case 10:
                    buf[chars_generated] = 'A' + ascii_addition;
                    break;
                case 11:
                    buf[chars_generated] = 'B' + ascii_addition;
                    break;
                case 12:
                    buf[chars_generated] = 'C' + ascii_addition;
                    break;
                case 13:
                    buf[chars_generated] = 'D' + ascii_addition;
                    break;
                case 14:
                    buf[chars_generated] = 'E' + ascii_addition;
                    break;
                case 15:
                    buf[chars_generated] = 'F' + ascii_addition;
                    break;
            }
        }

        d = d % base_to_highest_power;
    }

    buf[chars_generated] = '\0';
    return vsnprintf_print_string(str, size, buf, is_negative, should_print_prefix, specifier_data);
}

int vsnprintf(char* restrict str, size_t size, const char* restrict format, va_list ap)
{
    if (str == NULL)
    {
        size = 0;
    }

    size_t chars_generated = 0;

    va_list ap_copy;
    va_copy(ap_copy, ap);

    for (; *format != '\0'; ++format)
    {
        char current = *format;

        size_t remaining_size = (chars_generated + 1 < size)
            ? size - 1 - chars_generated
            : 0;

        char* address = remaining_size > 0
            ? str + chars_generated
            : NULL;

        if (current == '%')
        {
            vsnprintf_specifier_t specifier_data;
            format = get_format_specifier(format, &specifier_data, &ap_copy); // format now points to the last char in the specifier (we increment it again in the for loop)
            chars_generated += print_specifier_data(address, remaining_size, &specifier_data, &ap_copy);
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
