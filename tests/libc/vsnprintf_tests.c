///////////////////////////////////////////////////////////////////////////////
// \author (c) Marco Paland (info@paland.com)
//             2017-2019, PALANDesign Hannover, Germany
//
// \license The MIT License (MIT)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// \brief printf unit tests
//
///////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>

#define CUNIT_IMPLEMENTATION
#include <cunit.h>

#define PRINT_BUFFER_SIZE 100

#ifdef sprintf
#undef sprintf
#endif

static char   printf_buffer[PRINT_BUFFER_SIZE];
static size_t printf_idx = 0U;

int sprintf(char* str, const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    int ret = vsnprintf(str, SIZE_MAX, format, ap);
    va_end(ap);
    return ret;
}


CUNIT_TEST(snprintf)
{
  char buffer[PRINT_BUFFER_SIZE];

  snprintf(buffer, 100U, "%d", -1000);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-1000"));

  snprintf(buffer, 3U, "%d", -1000);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-1"));
}

static void vsnprintf_builder_1(char* buffer, ...)
{
  va_list args;
  va_start(args, buffer);
  vsnprintf(buffer, 100U, "%d", args);
  va_end(args);
}

static void vsnprintf_builder_3(char* buffer, ...)
{
  va_list args;
  va_start(args, buffer);
  vsnprintf(buffer, 100U, "%d %d %s", args);
  va_end(args);
}


CUNIT_TEST(vsnprintf)
{
  char buffer[PRINT_BUFFER_SIZE];

  vsnprintf_builder_1(buffer, -1);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-1"));

  vsnprintf_builder_3(buffer, 3, -1000, "test");
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "3 -1000 test"));
}


CUNIT_TEST(space_flag)
{
  char buffer[PRINT_BUFFER_SIZE];

  sprintf(buffer, "% d", 42);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, " 42"));

  sprintf(buffer, "% d", -42);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-42"));

  sprintf(buffer, "% 5d", 42);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "   42"));

  sprintf(buffer, "% 5d", -42);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "  -42"));

  sprintf(buffer, "% 15d", 42);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "             42"));

  sprintf(buffer, "% 15d", -42);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "            -42"));

  sprintf(buffer, "% 15d", -42);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "            -42"));

  sprintf(buffer, "% 15.3f", -42.987);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "        -42.987"));

  sprintf(buffer, "% 15.3f", 42.987);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "         42.987"));

  sprintf(buffer, "% s", "Hello testing");
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "Hello testing"));

  sprintf(buffer, "% d", 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, " 1024"));

  sprintf(buffer, "% d", -1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-1024"));

  sprintf(buffer, "% i", 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, " 1024"));

  sprintf(buffer, "% i", -1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-1024"));

  sprintf(buffer, "% u", 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "1024"));

  sprintf(buffer, "% u", 4294966272U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "4294966272"));

  sprintf(buffer, "% o", 511);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "777"));

  sprintf(buffer, "% o", 4294966785U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "37777777001"));

  sprintf(buffer, "% x", 305441741);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "1234abcd"));

  sprintf(buffer, "% x", 3989525555U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "edcb5433"));

  sprintf(buffer, "% X", 305441741);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "1234ABCD"));

  sprintf(buffer, "% X", 3989525555U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "EDCB5433"));

  sprintf(buffer, "% c", 'x');
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "x"));
}


CUNIT_TEST(plus_flag)
{
  char buffer[PRINT_BUFFER_SIZE];

  sprintf(buffer, "%+d", 42);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "+42"));

  sprintf(buffer, "%+d", -42);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-42"));

  sprintf(buffer, "%+5d", 42);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "  +42"));

  sprintf(buffer, "%+5d", -42);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "  -42"));

  sprintf(buffer, "%+15d", 42);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "            +42"));

  sprintf(buffer, "%+15d", -42);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "            -42"));

  sprintf(buffer, "%+s", "Hello testing");
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "Hello testing"));

  sprintf(buffer, "%+d", 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "+1024"));

  sprintf(buffer, "%+d", -1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-1024"));

  sprintf(buffer, "%+i", 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "+1024"));

  sprintf(buffer, "%+i", -1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-1024"));

  sprintf(buffer, "%+u", 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "1024"));

  sprintf(buffer, "%+u", 4294966272U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "4294966272"));

  sprintf(buffer, "%+o", 511);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "777"));

  sprintf(buffer, "%+o", 4294966785U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "37777777001"));

  sprintf(buffer, "%+x", 305441741);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "1234abcd"));

  sprintf(buffer, "%+x", 3989525555U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "edcb5433"));

  sprintf(buffer, "%+X", 305441741);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "1234ABCD"));

  sprintf(buffer, "%+X", 3989525555U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "EDCB5433"));

  sprintf(buffer, "%+c", 'x');
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "x"));

  sprintf(buffer, "%+.0d", 0);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "+"));
}


CUNIT_TEST(zero_flag)
{
  char buffer[PRINT_BUFFER_SIZE];

  sprintf(buffer, "%0d", 42);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "42"));

  sprintf(buffer, "%0ld", 42L);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "42"));

  sprintf(buffer, "%0d", -42);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-42"));

  sprintf(buffer, "%05d", 42);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "00042"));

  sprintf(buffer, "%05d", -42);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-0042"));

  sprintf(buffer, "%015d", 42);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "000000000000042"));

  sprintf(buffer, "%015d", -42);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-00000000000042"));

  sprintf(buffer, "%015.2f", 42.1234);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "000000000042.12"));

  sprintf(buffer, "%015.3f", 42.9876);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "00000000042.988"));

  sprintf(buffer, "%015.5f", -42.9876);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-00000042.98760"));
}


CUNIT_TEST(minus_flag)
{
  char buffer[PRINT_BUFFER_SIZE];

  sprintf(buffer, "%-d", 42);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "42"));

  sprintf(buffer, "%-d", -42);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-42"));

  sprintf(buffer, "%-5d", 42);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "42   "));

  sprintf(buffer, "%-5d", -42);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-42  "));

  sprintf(buffer, "%-15d", 42);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "42             "));

  sprintf(buffer, "%-15d", -42);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-42            "));

  sprintf(buffer, "%-0d", 42);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "42"));

  sprintf(buffer, "%-0d", -42);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-42"));

  sprintf(buffer, "%-05d", 42);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "42   "));

  sprintf(buffer, "%-05d", -42);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-42  "));

  sprintf(buffer, "%-015d", 42);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "42             "));

  sprintf(buffer, "%-015d", -42);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-42            "));

  sprintf(buffer, "%0-d", 42);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "42"));

  sprintf(buffer, "%0-d", -42);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-42"));

  sprintf(buffer, "%0-5d", 42);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "42   "));

  sprintf(buffer, "%0-5d", -42);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-42  "));

  sprintf(buffer, "%0-15d", 42);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "42             "));

  sprintf(buffer, "%0-15d", -42);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-42            "));

  sprintf(buffer, "%0-15.3e", -42.);
#ifndef PRINTF_DISABLE_SUPPORT_EXPONENTIAL
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-4.200e+01     "));
#else
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "e"));
#endif

  sprintf(buffer, "%0-15.3g", -42.);
#ifndef PRINTF_DISABLE_SUPPORT_EXPONENTIAL
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-42.0          "));
#else
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "g"));
#endif
}


CUNIT_TEST(hashtag_flag)
{
  char buffer[PRINT_BUFFER_SIZE];

  sprintf(buffer, "%#.0x", 0);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, ""));
  sprintf(buffer, "%#.1x", 0);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "0"));
  sprintf(buffer, "%#.0llx", (long long)0);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, ""));
  sprintf(buffer, "%#.8x", 0x614e);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "0x0000614e"));
  sprintf(buffer,"%#b", 6);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "0b110"));
}


CUNIT_TEST(specifier)
{
  char buffer[PRINT_BUFFER_SIZE];

  sprintf(buffer, "Hello testing");
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "Hello testing"));

  sprintf(buffer, "%s", "Hello testing");
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "Hello testing"));

  sprintf(buffer, "%d", 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "1024"));

  sprintf(buffer, "%d", -1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-1024"));

  sprintf(buffer, "%i", 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "1024"));

  sprintf(buffer, "%i", -1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-1024"));

  sprintf(buffer, "%u", 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "1024"));

  sprintf(buffer, "%u", 4294966272U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "4294966272"));

  sprintf(buffer, "%o", 511);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "777"));

  sprintf(buffer, "%o", 4294966785U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "37777777001"));

  sprintf(buffer, "%x", 305441741);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "1234abcd"));

  sprintf(buffer, "%x", 3989525555U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "edcb5433"));

  sprintf(buffer, "%X", 305441741);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "1234ABCD"));

  sprintf(buffer, "%X", 3989525555U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "EDCB5433"));

  sprintf(buffer, "%%");
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "%"));
}


CUNIT_TEST(width)
{
  char buffer[PRINT_BUFFER_SIZE];

  sprintf(buffer, "%1s", "Hello testing");
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "Hello testing"));

  sprintf(buffer, "%1d", 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "1024"));

  sprintf(buffer, "%1d", -1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-1024"));

  sprintf(buffer, "%1i", 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "1024"));

  sprintf(buffer, "%1i", -1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-1024"));

  sprintf(buffer, "%1u", 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "1024"));

  sprintf(buffer, "%1u", 4294966272U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "4294966272"));

  sprintf(buffer, "%1o", 511);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "777"));

  sprintf(buffer, "%1o", 4294966785U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "37777777001"));

  sprintf(buffer, "%1x", 305441741);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "1234abcd"));

  sprintf(buffer, "%1x", 3989525555U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "edcb5433"));

  sprintf(buffer, "%1X", 305441741);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "1234ABCD"));

  sprintf(buffer, "%1X", 3989525555U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "EDCB5433"));

  sprintf(buffer, "%1c", 'x');
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "x"));
}


CUNIT_TEST(width_20)
{
  char buffer[PRINT_BUFFER_SIZE];

  sprintf(buffer, "%20s", "Hello");
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "               Hello"));

  sprintf(buffer, "%20d", 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "                1024"));

  sprintf(buffer, "%20d", -1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "               -1024"));

  sprintf(buffer, "%20i", 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "                1024"));

  sprintf(buffer, "%20i", -1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "               -1024"));

  sprintf(buffer, "%20u", 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "                1024"));

  sprintf(buffer, "%20u", 4294966272U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "          4294966272"));

  sprintf(buffer, "%20o", 511);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "                 777"));

  sprintf(buffer, "%20o", 4294966785U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "         37777777001"));

  sprintf(buffer, "%20x", 305441741);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "            1234abcd"));

  sprintf(buffer, "%20x", 3989525555U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "            edcb5433"));

  sprintf(buffer, "%20X", 305441741);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "            1234ABCD"));

  sprintf(buffer, "%20X", 3989525555U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "            EDCB5433"));

  sprintf(buffer, "%20c", 'x');
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "                   x"));
}

CUNIT_TEST(width_star_20)
{
  char buffer[PRINT_BUFFER_SIZE];

  sprintf(buffer, "%*s", 20, "Hello");
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "               Hello"));

  sprintf(buffer, "%*d", 20, 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "                1024"));

  sprintf(buffer, "%*d", 20, -1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "               -1024"));

  sprintf(buffer, "%*i", 20, 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "                1024"));

  sprintf(buffer, "%*i", 20, -1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "               -1024"));

  sprintf(buffer, "%*u", 20, 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "                1024"));

  sprintf(buffer, "%*u", 20, 4294966272U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "          4294966272"));

  sprintf(buffer, "%*o", 20, 511);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "                 777"));

  sprintf(buffer, "%*o", 20, 4294966785U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "         37777777001"));

  sprintf(buffer, "%*x", 20, 305441741);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "            1234abcd"));

  sprintf(buffer, "%*x", 20, 3989525555U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "            edcb5433"));

  sprintf(buffer, "%*X", 20, 305441741);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "            1234ABCD"));

  sprintf(buffer, "%*X", 20, 3989525555U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "            EDCB5433"));

  sprintf(buffer, "%*c", 20,'x');
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "                   x"));
}


CUNIT_TEST(width_minus_twenty)
{
  char buffer[PRINT_BUFFER_SIZE];

  sprintf(buffer, "%-20s", "Hello");
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "Hello               "));

  sprintf(buffer, "%-20d", 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "1024                "));

  sprintf(buffer, "%-20d", -1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-1024               "));

  sprintf(buffer, "%-20i", 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "1024                "));

  sprintf(buffer, "%-20i", -1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-1024               "));

  sprintf(buffer, "%-20u", 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "1024                "));

  sprintf(buffer, "%-20.4f", 1024.1234);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "1024.1234           "));

  sprintf(buffer, "%-20u", 4294966272U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "4294966272          "));

  sprintf(buffer, "%-20o", 511);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "777                 "));

  sprintf(buffer, "%-20o", 4294966785U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "37777777001         "));

  sprintf(buffer, "%-20x", 305441741);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "1234abcd            "));

  sprintf(buffer, "%-20x", 3989525555U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "edcb5433            "));

  sprintf(buffer, "%-20X", 305441741);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "1234ABCD            "));

  sprintf(buffer, "%-20X", 3989525555U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "EDCB5433            "));

  sprintf(buffer, "%-20c", 'x');
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "x                   "));

  sprintf(buffer, "|%5d| |%-2d| |%5d|", 9, 9, 9);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "|    9| |9 | |    9|"));

  sprintf(buffer, "|%5d| |%-2d| |%5d|", 10, 10, 10);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "|   10| |10| |   10|"));

  sprintf(buffer, "|%5d| |%-12d| |%5d|", 9, 9, 9);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "|    9| |9           | |    9|"));

  sprintf(buffer, "|%5d| |%-12d| |%5d|", 10, 10, 10);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "|   10| |10          | |   10|"));
}


CUNIT_TEST(width_zero_minus_twenty)
{
  char buffer[PRINT_BUFFER_SIZE];

  sprintf(buffer, "%0-20s", "Hello");
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "Hello               "));

  sprintf(buffer, "%0-20d", 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "1024                "));

  sprintf(buffer, "%0-20d", -1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-1024               "));

  sprintf(buffer, "%0-20i", 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "1024                "));

  sprintf(buffer, "%0-20i", -1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-1024               "));

  sprintf(buffer, "%0-20u", 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "1024                "));

  sprintf(buffer, "%0-20u", 4294966272U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "4294966272          "));

  sprintf(buffer, "%0-20o", 511);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "777                 "));

  sprintf(buffer, "%0-20o", 4294966785U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "37777777001         "));

  sprintf(buffer, "%0-20x", 305441741);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "1234abcd            "));

  sprintf(buffer, "%0-20x", 3989525555U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "edcb5433            "));

  sprintf(buffer, "%0-20X", 305441741);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "1234ABCD            "));

  sprintf(buffer, "%0-20X", 3989525555U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "EDCB5433            "));

  sprintf(buffer, "%0-20c", 'x');
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "x                   "));
}


CUNIT_TEST(padding_twenty)
{
  char buffer[PRINT_BUFFER_SIZE];

  sprintf(buffer, "%020d", 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "00000000000000001024"));

  sprintf(buffer, "%020d", -1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-0000000000000001024"));

  sprintf(buffer, "%020i", 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "00000000000000001024"));

  sprintf(buffer, "%020i", -1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-0000000000000001024"));

  sprintf(buffer, "%020u", 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "00000000000000001024"));

  sprintf(buffer, "%020u", 4294966272U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "00000000004294966272"));

  sprintf(buffer, "%020o", 511);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "00000000000000000777"));

  sprintf(buffer, "%020o", 4294966785U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "00000000037777777001"));

  sprintf(buffer, "%020x", 305441741);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "0000000000001234abcd"));

  sprintf(buffer, "%020x", 3989525555U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "000000000000edcb5433"));

  sprintf(buffer, "%020X", 305441741);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "0000000000001234ABCD"));

  sprintf(buffer, "%020X", 3989525555U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "000000000000EDCB5433"));
}


CUNIT_TEST(padding_dot_twenty)
{
  char buffer[PRINT_BUFFER_SIZE];

  sprintf(buffer, "%.20d", 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "00000000000000001024"));

  sprintf(buffer, "%.20d", -1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-00000000000000001024"));

  sprintf(buffer, "%.20i", 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "00000000000000001024"));

  sprintf(buffer, "%.20i", -1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-00000000000000001024"));

  sprintf(buffer, "%.20u", 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "00000000000000001024"));

  sprintf(buffer, "%.20u", 4294966272U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "00000000004294966272"));

  sprintf(buffer, "%.20o", 511);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "00000000000000000777"));

  sprintf(buffer, "%.20o", 4294966785U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "00000000037777777001"));

  sprintf(buffer, "%.20x", 305441741);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "0000000000001234abcd"));

  sprintf(buffer, "%.20x", 3989525555U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "000000000000edcb5433"));

  sprintf(buffer, "%.20X", 305441741);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "0000000000001234ABCD"));

  sprintf(buffer, "%.20X", 3989525555U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "000000000000EDCB5433"));
}


CUNIT_TEST(padding_hashtag_020)
{
  char buffer[PRINT_BUFFER_SIZE];

  sprintf(buffer, "%#020d", 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "00000000000000001024"));

  sprintf(buffer, "%#020d", -1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-0000000000000001024"));

  sprintf(buffer, "%#020i", 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "00000000000000001024"));

  sprintf(buffer, "%#020i", -1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-0000000000000001024"));

  sprintf(buffer, "%#020u", 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "00000000000000001024"));

  sprintf(buffer, "%#020u", 4294966272U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "00000000004294966272"));

  sprintf(buffer, "%#020o", 511);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "00000000000000000777"));

  sprintf(buffer, "%#020o", 4294966785U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "00000000037777777001"));

  sprintf(buffer, "%#020x", 305441741);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "0x00000000001234abcd"));

  sprintf(buffer, "%#020x", 3989525555U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "0x0000000000edcb5433"));

  sprintf(buffer, "%#020X", 305441741);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "0X00000000001234ABCD"));

  sprintf(buffer, "%#020X", 3989525555U);

  CUNIT_ASSERT_TRUE(!strcmp(buffer, "0X0000000000EDCB5433"));
}


CUNIT_TEST(padding_hashtag_twenty)
{
  char buffer[PRINT_BUFFER_SIZE];

  sprintf(buffer, "%#20d", 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "                1024"));

  sprintf(buffer, "%#20d", -1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "               -1024"));

  sprintf(buffer, "%#20i", 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "                1024"));

  sprintf(buffer, "%#20i", -1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "               -1024"));

  sprintf(buffer, "%#20u", 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "                1024"));

  sprintf(buffer, "%#20u", 4294966272U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "          4294966272"));

  sprintf(buffer, "%#20o", 511);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "                0777"));

  sprintf(buffer, "%#20o", 4294966785U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "        037777777001"));

  sprintf(buffer, "%#20x", 305441741);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "          0x1234abcd"));

  sprintf(buffer, "%#20x", 3989525555U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "          0xedcb5433"));

  sprintf(buffer, "%#20X", 305441741);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "          0X1234ABCD"));

  sprintf(buffer, "%#20X", 3989525555U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "          0XEDCB5433"));
}


CUNIT_TEST(padding_20_dot_5)
{
  char buffer[PRINT_BUFFER_SIZE];

  sprintf(buffer, "%20.5d", 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "               01024"));

  sprintf(buffer, "%20.5d", -1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "              -01024"));

  sprintf(buffer, "%20.5i", 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "               01024"));

  sprintf(buffer, "%20.5i", -1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "              -01024"));

  sprintf(buffer, "%20.5u", 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "               01024"));

  sprintf(buffer, "%20.5u", 4294966272U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "          4294966272"));

  sprintf(buffer, "%20.5o", 511);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "               00777"));

  sprintf(buffer, "%20.5o", 4294966785U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "         37777777001"));

  sprintf(buffer, "%20.5x", 305441741);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "            1234abcd"));

  sprintf(buffer, "%20.10x", 3989525555U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "          00edcb5433"));

  sprintf(buffer, "%20.5X", 305441741);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "            1234ABCD"));

  sprintf(buffer, "%20.10X", 3989525555U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "          00EDCB5433"));
}


CUNIT_TEST(padding_neg_numbers)
{
  char buffer[PRINT_BUFFER_SIZE];

  // space padding
  sprintf(buffer, "% 1d", -5);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-5"));

  sprintf(buffer, "% 2d", -5);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-5"));

  sprintf(buffer, "% 3d", -5);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, " -5"));

  sprintf(buffer, "% 4d", -5);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "  -5"));

  // zero padding
  sprintf(buffer, "%01d", -5);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-5"));


  sprintf(buffer, "%02d", -5);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-5"));

  sprintf(buffer, "%03d", -5);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-05"));

  sprintf(buffer, "%04d", -5);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-005"));
}


CUNIT_TEST(float_padding_neg_numbers)
{
  char buffer[PRINT_BUFFER_SIZE];

  // space padding
  sprintf(buffer, "% 3.1f", -5.);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-5.0"));

  sprintf(buffer, "% 4.1f", -5.);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-5.0"));

  sprintf(buffer, "% 5.1f", -5.);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, " -5.0"));

#ifndef PRINTF_DISABLE_SUPPORT_EXPONENTIAL
  sprintf(buffer, "% 6.1g", -5.);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "    -5"));

  sprintf(buffer, "% 6.1e", -5.);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-5.0e+00"));

  sprintf(buffer, "% 10.1e", -5.);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "  -5.0e+00"));
#endif

  // zero padding
  sprintf(buffer, "%03.1f", -5.);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-5.0"));

  sprintf(buffer, "%04.1f", -5.);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-5.0"));

  sprintf(buffer, "%05.1f", -5.);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-05.0"));

  // zero padding no decimal point
  sprintf(buffer, "%01.0f", -5.);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-5"));

  sprintf(buffer, "%02.0f", -5.);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-5"));

  sprintf(buffer, "%03.0f", -5.);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-05"));

#ifndef PRINTF_DISABLE_SUPPORT_EXPONENTIAL
  sprintf(buffer, "%010.1e", -5.);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-005.0e+00"));

  sprintf(buffer, "%07.0E", -5.);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-05E+00"));

  sprintf(buffer, "%03.0g", -5.);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-05"));
#endif
}

CUNIT_TEST(length)
{
  char buffer[PRINT_BUFFER_SIZE];

  sprintf(buffer, "%.0s", "Hello testing");
  CUNIT_ASSERT_TRUE(!strcmp(buffer, ""));

  sprintf(buffer, "%20.0s", "Hello testing");
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "                    "));

  sprintf(buffer, "%.s", "Hello testing");
  CUNIT_ASSERT_TRUE(!strcmp(buffer, ""));

  sprintf(buffer, "%20.s", "Hello testing");
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "                    "));

  sprintf(buffer, "%20.0d", 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "                1024"));

  sprintf(buffer, "%20.0d", -1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "               -1024"));

  sprintf(buffer, "%20.d", 0);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "                    "));

  sprintf(buffer, "%20.0i", 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "                1024"));

  sprintf(buffer, "%20.i", -1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "               -1024"));

  sprintf(buffer, "%20.i", 0);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "                    "));

  sprintf(buffer, "%20.u", 1024);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "                1024"));

  sprintf(buffer, "%20.0u", 4294966272U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "          4294966272"));

  sprintf(buffer, "%20.u", 0U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "                    "));

  sprintf(buffer, "%20.o", 511);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "                 777"));

  sprintf(buffer, "%20.0o", 4294966785U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "         37777777001"));

  sprintf(buffer, "%20.o", 0U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "                    "));

  sprintf(buffer, "%20.x", 305441741);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "            1234abcd"));

  sprintf(buffer, "%50.x", 305441741);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "                                          1234abcd"));

  sprintf(buffer, "%50.x%10.u", 305441741, 12345);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "                                          1234abcd     12345"));

  sprintf(buffer, "%20.0x", 3989525555U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "            edcb5433"));

  sprintf(buffer, "%20.x", 0U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "                    "));

  sprintf(buffer, "%20.X", 305441741);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "            1234ABCD"));

  sprintf(buffer, "%20.0X", 3989525555U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "            EDCB5433"));

  sprintf(buffer, "%20.X", 0U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "                    "));

  sprintf(buffer, "%02.0u", 0U);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "  "));

  sprintf(buffer, "%02.0d", 0);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "  "));
}

// I don't support floats ATM
//CUNIT_TEST(float)
//{
//  char buffer[PRINT_BUFFER_SIZE];
//
//  // test special-case floats using math.h macros
//  sprintf(buffer, "%8f", NAN);
//  CUNIT_ASSERT_TRUE(!strcmp(buffer, "     nan"));
//
//  sprintf(buffer, "%8f", INFINITY);
//  CUNIT_ASSERT_TRUE(!strcmp(buffer, "     inf"));
//
//  sprintf(buffer, "%-8f", -INFINITY);
//  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-inf    "));
//
//#ifndef PRINTF_DISABLE_SUPPORT_EXPONENTIAL
//  sprintf(buffer, "%+8e", INFINITY);
//  CUNIT_ASSERT_TRUE(!strcmp(buffer, "    +inf"));
//#endif
//
//  sprintf(buffer, "%.4f", 3.1415354);
//  CUNIT_ASSERT_TRUE(!strcmp(buffer, "3.1415"));
//
//  sprintf(buffer, "%.3f", 30343.1415354);
//  CUNIT_ASSERT_TRUE(!strcmp(buffer, "30343.142"));
//
//  sprintf(buffer, "%.0f", 34.1415354);
//  CUNIT_ASSERT_TRUE(!strcmp(buffer, "34"));
//
//  sprintf(buffer, "%.0f", 1.3);
//  CUNIT_ASSERT_TRUE(!strcmp(buffer, "1"));
//
//  sprintf(buffer, "%.0f", 1.55);
//  CUNIT_ASSERT_TRUE(!strcmp(buffer, "2"));
//
//  sprintf(buffer, "%.1f", 1.64);
//  CUNIT_ASSERT_TRUE(!strcmp(buffer, "1.6"));
//
//  sprintf(buffer, "%.2f", 42.8952);
//  CUNIT_ASSERT_TRUE(!strcmp(buffer, "42.90"));
//
//  sprintf(buffer, "%.9f", 42.8952);
//  CUNIT_ASSERT_TRUE(!strcmp(buffer, "42.895200000"));
//
//  sprintf(buffer, "%.10f", 42.895223);
//  CUNIT_ASSERT_TRUE(!strcmp(buffer, "42.8952230000"));
//
//  // this testcase checks, that the precision is truncated to 9 digits.
//  // a perfect working float should return the whole number
//  sprintf(buffer, "%.12f", 42.89522312345678);
//  CUNIT_ASSERT_TRUE(!strcmp(buffer, "42.895223123000"));
//
//  // this testcase checks, that the precision is truncated AND rounded to 9 digits.
//  // a perfect working float should return the whole number
//  sprintf(buffer, "%.12f", 42.89522387654321);
//  CUNIT_ASSERT_TRUE(!strcmp(buffer, "42.895223877000"));
//
//  sprintf(buffer, "%6.2f", 42.8952);
//  CUNIT_ASSERT_TRUE(!strcmp(buffer, " 42.90"));
//
//  sprintf(buffer, "%+6.2f", 42.8952);
//  CUNIT_ASSERT_TRUE(!strcmp(buffer, "+42.90"));
//
//  sprintf(buffer, "%+5.1f", 42.9252);
//  CUNIT_ASSERT_TRUE(!strcmp(buffer, "+42.9"));
//
//  sprintf(buffer, "%f", 42.5);
//  CUNIT_ASSERT_TRUE(!strcmp(buffer, "42.500000"));
//
//  sprintf(buffer, "%.1f", 42.5);
//  CUNIT_ASSERT_TRUE(!strcmp(buffer, "42.5"));
//
//  sprintf(buffer, "%f", 42167.0);
//  CUNIT_ASSERT_TRUE(!strcmp(buffer, "42167.000000"));
//
//  sprintf(buffer, "%.9f", -12345.987654321);
//  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-12345.987654321"));
//
//  sprintf(buffer, "%.1f", 3.999);
//  CUNIT_ASSERT_TRUE(!strcmp(buffer, "4.0"));
//
//  sprintf(buffer, "%.0f", 3.5);
//  CUNIT_ASSERT_TRUE(!strcmp(buffer, "4"));
//
//  sprintf(buffer, "%.0f", 4.5);
//  CUNIT_ASSERT_TRUE(!strcmp(buffer, "4"));
//
//  sprintf(buffer, "%.0f", 3.49);
//  CUNIT_ASSERT_TRUE(!strcmp(buffer, "3"));
//
//  sprintf(buffer, "%.1f", 3.49);
//  CUNIT_ASSERT_TRUE(!strcmp(buffer, "3.5"));
//
//  sprintf(buffer, "a%-5.1f", 0.5);
//  CUNIT_ASSERT_TRUE(!strcmp(buffer, "a0.5  "));
//
//  sprintf(buffer, "a%-5.1fend", 0.5);
//  CUNIT_ASSERT_TRUE(!strcmp(buffer, "a0.5  end"));
//
//#ifndef PRINTF_DISABLE_SUPPORT_EXPONENTIAL
//  sprintf(buffer, "%G", 12345.678);
//  CUNIT_ASSERT_TRUE(!strcmp(buffer, "12345.7"));
//
//  sprintf(buffer, "%.7G", 12345.678);
//  CUNIT_ASSERT_TRUE(!strcmp(buffer, "12345.68"));
//
//  sprintf(buffer, "%.5G", 123456789.);
//  CUNIT_ASSERT_TRUE(!strcmp(buffer, "1.2346E+08"));
//
//  sprintf(buffer, "%.6G", 12345.);
//  CUNIT_ASSERT_TRUE(!strcmp(buffer, "12345.0"));
//
//  sprintf(buffer, "%+12.4g", 123456789.);
//  CUNIT_ASSERT_TRUE(!strcmp(buffer, "  +1.235e+08"));
//
//  sprintf(buffer, "%.2G", 0.001234);
//  CUNIT_ASSERT_TRUE(!strcmp(buffer, "0.0012"));
//
//  sprintf(buffer, "%+10.4G", 0.001234);
//  CUNIT_ASSERT_TRUE(!strcmp(buffer, " +0.001234"));
//
//  sprintf(buffer, "%+012.4g", 0.00001234);
//  CUNIT_ASSERT_TRUE(!strcmp(buffer, "+001.234e-05"));
//
//  sprintf(buffer, "%.3g", -1.2345e-308);
//  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-1.23e-308"));
//
//  sprintf(buffer, "%+.3E", 1.23e+308);
//  CUNIT_ASSERT_TRUE(!strcmp(buffer, "+1.230E+308"));
//#endif
//
//  // out of range for float: should switch to exp notation if supported, else empty
//  sprintf(buffer, "%.1f", 1E20);
//#ifndef PRINTF_DISABLE_SUPPORT_EXPONENTIAL
//  CUNIT_ASSERT_TRUE(!strcmp(buffer, "1.0e+20"));
//#else
//  CUNIT_ASSERT_TRUE(!strcmp(buffer, ""));
//#endif
//
//  // brute force float
//  bool fail = false;
//  std::stringstream str;
//  str.precision(5);
//  for (float i = -100000; i < 100000; i += 1) {
//    sprintf(buffer, "%.5f", i / 10000);
//    str.str("");
//    str << std::fixed << i / 10000;
//    fail = fail || !!strcmp(buffer, str.str().c_str());
//  }
//  CUNIT_ASSERT_TRUE(!fail);
//
//
//#ifndef PRINTF_DISABLE_SUPPORT_EXPONENTIAL
//  // brute force exp
//  str.setf(std::ios::scientific, std::ios::floatfield);
//  for (float i = -1e20; i < 1e20; i += 1e15) {
//    sprintf(buffer, "%.5f", i);
//    str.str("");
//    str << i;
//    fail = fail || !!strcmp(buffer, str.str().c_str());
//  }
//  CUNIT_ASSERT_TRUE(!fail);
//#endif
//}


CUNIT_TEST(types)
{
  char buffer[PRINT_BUFFER_SIZE];

  sprintf(buffer, "%i", 0);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "0"));

  sprintf(buffer, "%i", 1234);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "1234"));

  sprintf(buffer, "%i", 32767);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "32767"));

  sprintf(buffer, "%i", -32767);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-32767"));

  sprintf(buffer, "%li", 30L);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "30"));

  sprintf(buffer, "%li", -2147483647L);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-2147483647"));

  sprintf(buffer, "%li", 2147483647L);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "2147483647"));

  sprintf(buffer, "%lli", 30LL);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "30"));

  sprintf(buffer, "%lli", -9223372036854775807LL);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "-9223372036854775807"));

  sprintf(buffer, "%lli", 9223372036854775807LL);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "9223372036854775807"));

  sprintf(buffer, "%lu", 100000L);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "100000"));

  sprintf(buffer, "%lu", 0xFFFFFFFFL);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "4294967295"));

  sprintf(buffer, "%llu", 281474976710656LLU);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "281474976710656"));

  sprintf(buffer, "%llu", 18446744073709551615LLU);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "18446744073709551615"));

  sprintf(buffer, "%zu", 2147483647UL);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "2147483647"));

  sprintf(buffer, "%zd", 2147483647UL);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "2147483647"));

  if (sizeof(size_t) == sizeof(long)) {
    sprintf(buffer, "%zi", -2147483647L);
    CUNIT_ASSERT_TRUE(!strcmp(buffer, "-2147483647"));
  }
  else {
    sprintf(buffer, "%zi", -2147483647LL);
    CUNIT_ASSERT_TRUE(!strcmp(buffer, "-2147483647"));
  }

  sprintf(buffer, "%b", 60000);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "1110101001100000"));

  sprintf(buffer, "%lb", 12345678L);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "101111000110000101001110"));

  sprintf(buffer, "%o", 60000);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "165140"));

  sprintf(buffer, "%lo", 12345678L);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "57060516"));

  sprintf(buffer, "%lx", 0x12345678L);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "12345678"));

  sprintf(buffer, "%llx", 0x1234567891234567LLU);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "1234567891234567"));

  sprintf(buffer, "%lx", 0xabcdefabL);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "abcdefab"));

  sprintf(buffer, "%lX", 0xabcdefabL);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "ABCDEFAB"));

  sprintf(buffer, "%c", 'v');
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "v"));

  sprintf(buffer, "%cv", 'w');
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "wv"));

  sprintf(buffer, "%s", "A Test");
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "A Test"));

  sprintf(buffer, "%hhu", 0xFFFFUL);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "255"));

  sprintf(buffer, "%hu", 0x123456UL);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "13398"));

  sprintf(buffer, "%s%hhi %hu", "Test", 10000, 0xFFFFFFFF);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "Test16 65535"));

  sprintf(buffer, "%tx", &buffer[10] - &buffer[0]);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "a"));

// TBD
  if (sizeof(intmax_t) == sizeof(long)) {
    sprintf(buffer, "%ji", -2147483647L);
    CUNIT_ASSERT_TRUE(!strcmp(buffer, "-2147483647"));
  }
  else {
    sprintf(buffer, "%ji", -2147483647LL);
    CUNIT_ASSERT_TRUE(!strcmp(buffer, "-2147483647"));
  }
}


CUNIT_TEST(pointer)
{
  char buffer[PRINT_BUFFER_SIZE];

  sprintf(buffer, "%p", (void*)0x1234U);
  if (sizeof(void*) == 4U) {
    CUNIT_ASSERT_TRUE(!strcmp(buffer, "00001234"));
  }
  else {
    CUNIT_ASSERT_TRUE(!strcmp(buffer, "0000000000001234"));
  }

  sprintf(buffer, "%p", (void*)0x12345678U);
  if (sizeof(void*) == 4U) {
    CUNIT_ASSERT_TRUE(!strcmp(buffer, "12345678"));
  }
  else {
    CUNIT_ASSERT_TRUE(!strcmp(buffer, "0000000012345678"));
  }

  sprintf(buffer, "%p-%p", (void*)0x12345678U, (void*)0x7EDCBA98U);
  if (sizeof(void*) == 4U) {
    CUNIT_ASSERT_TRUE(!strcmp(buffer, "12345678-7EDCBA98"));
  }
  else {
    CUNIT_ASSERT_TRUE(!strcmp(buffer, "0000000012345678-000000007EDCBA98"));
  }

  if (sizeof(uintptr_t) == sizeof(uint64_t)) {
    sprintf(buffer, "%p", (void*)(uintptr_t)0xFFFFFFFFU);
    CUNIT_ASSERT_TRUE(!strcmp(buffer, "00000000FFFFFFFF"));
  }
  else {
    sprintf(buffer, "%p", (void*)(uintptr_t)0xFFFFFFFFU);
    CUNIT_ASSERT_TRUE(!strcmp(buffer, "FFFFFFFF"));
  }
}


CUNIT_TEST(unknown_flag)
{
  char buffer[PRINT_BUFFER_SIZE];

  sprintf(buffer, "%kmarco", 42, 37);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "kmarco"));
}


CUNIT_TEST(string_length)
{
  char buffer[PRINT_BUFFER_SIZE];

  sprintf(buffer, "%.4s", "This is a test");
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "This"));

  sprintf(buffer, "%.4s", "test");
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "test"));

  sprintf(buffer, "%.7s", "123");
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "123"));

  sprintf(buffer, "%.7s", "");
  CUNIT_ASSERT_TRUE(!strcmp(buffer, ""));

  sprintf(buffer, "%.4s%.2s", "123456", "abcdef");
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "1234ab"));

  sprintf(buffer, "%.4.2s", "123456");
  CUNIT_ASSERT_TRUE(!strcmp(buffer, ".2s"));

  sprintf(buffer, "%.*s", 3, "123456");
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "123"));
}


CUNIT_TEST(buffer_length)
{
  char buffer[PRINT_BUFFER_SIZE];
  int ret;

  ret = snprintf(NULL, 10, "%s", "Test");
  CUNIT_ASSERT_TRUE(ret == 4);
  ret = snprintf(NULL, 0, "%s", "Test");
  CUNIT_ASSERT_TRUE(ret == 4);

  buffer[0] = (char)0xA5;
  ret = snprintf(buffer, 0, "%s", "Test");
  CUNIT_ASSERT_TRUE(buffer[0] == (char)0xA5);
  CUNIT_ASSERT_TRUE(ret == 4);

  buffer[0] = (char)0xCC;
  snprintf(buffer, 1, "%s", "Test");
  CUNIT_ASSERT_TRUE(buffer[0] == '\0');

  snprintf(buffer, 2, "%s", "Hello");
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "H"));
}


CUNIT_TEST(ret_value)
{
  char buffer[PRINT_BUFFER_SIZE] ;
  int ret;

  ret = snprintf(buffer, 6, "0%s", "1234");
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "01234"));
  CUNIT_ASSERT_TRUE(ret == 5);

  ret = snprintf(buffer, 6, "0%s", "12345");
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "01234"));
  CUNIT_ASSERT_TRUE(ret == 6);  // '5' is truncated

  ret = snprintf(buffer, 6, "0%s", "1234567");
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "01234"));
  CUNIT_ASSERT_TRUE(ret == 8);  // '567' are truncated

  ret = snprintf(buffer, 10, "hello, world");
  CUNIT_ASSERT_TRUE(ret == 12);

  ret = snprintf(buffer, 3, "%d", 10000);
  CUNIT_ASSERT_TRUE(ret == 5);
  CUNIT_ASSERT_TRUE(strlen(buffer) == 2U);
  CUNIT_ASSERT_TRUE(buffer[0] == '1');
  CUNIT_ASSERT_TRUE(buffer[1] == '0');
  CUNIT_ASSERT_TRUE(buffer[2] == '\0');
}


CUNIT_TEST(misc)
{
  char buffer[PRINT_BUFFER_SIZE];

  sprintf(buffer, "%u%u%ctest%d %s", 5, 3000, 'a', -20, "bit");
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "53000atest-20 bit"));

  sprintf(buffer, "%.*f", 2, 0.33333333);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "0.33"));

  sprintf(buffer, "%.*d", -1, 1);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "1"));

  sprintf(buffer, "%.3s", "foobar");
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "foo"));

  sprintf(buffer, "% .0d", 0);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, " "));

  sprintf(buffer, "%10.5d", 4);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "     00004"));

  sprintf(buffer, "%*sx", -3, "hi");
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "hi x"));

#ifndef PRINTF_DISABLE_SUPPORT_EXPONENTIAL
  sprintf(buffer, "%.*g", 2, 0.33333333);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "0.33"));

  sprintf(buffer, "%.*e", 2, 0.33333333);
  CUNIT_ASSERT_TRUE(!strcmp(buffer, "3.33e-01"));
#endif
}
