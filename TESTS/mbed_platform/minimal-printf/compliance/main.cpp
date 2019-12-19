/* mbed Microcontroller Library
 * Copyright (c) 2017 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifdef TARGET_LIKE_MBED
#include "mbed.h"
#endif
#include "mbed_printf.h"

#include "utest/utest.h"
#include "unity/unity.h"
#include "greentea-client/test_env.h"

#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <inttypes.h>

#ifndef ULLONG_MAX
#define ULLONG_MAX UINT64_MAX
#endif

#ifndef LLONG_MAX
#define LLONG_MAX INT64_MAX
#endif

#ifndef LLONG_MIN
#define LLONG_MIN INT64_MIN
#endif

using namespace utest::v1;

#if defined(__NEWLIB_NANO)

#define MAX_STRING_SIZE 100
//Global buffer used by test cases to store string literal
char expected_string[MAX_STRING_SIZE];

typedef enum {
    RADIX_10,//radix base for decimal
    RADIX_16 //radix base for Hexadecimal
} radix_base;

// This function is similar to itoa but it can convert any data type with decimal and hexadecimal numerical value into a string
static void convert_to_string(unsigned long long  value,  char str[],  radix_base radix = RADIX_10, bool sign = false)
{
    char        buffer[MAX_STRING_SIZE];
    char       *dest = buffer + sizeof(buffer);

    // If value is 0
    if (value == 0) {
        memcpy(str, "0", 2);
        return;
    }

    if (sign) {
        if ((long long) value < 0) {
            // Make value postive
            value = -value;
        }
    }
    *--dest = '\0';
    switch (radix) {
        case RADIX_16:
            while (value) {
                * --dest = '0' + (value & 0xF);
                if (*dest > '9') {
                    *dest += 'A' - '9' - 1;
                }
                value >>= 4;
            }
            break;
        case RADIX_10:
            while (value) {
                *--dest = '0' + (value % 10);
                value /= 10;
            }
            break;

        default:
            MBED_ASSERT(0);
            break;
    }

    if (sign) {
        // Add the negative sign for negative numerical value
        *--dest = '-';
    }
    // Copy the converted string from dest into str buffer of converted string length
    memcpy(str, dest, buffer + sizeof(buffer) - dest);
}

//This function used to calculate the string length on format 'prefix + data + suffix'
static int expected_string_length(char *prefix = nullptr, long long value, radix_base base, char *suffix = nullptr, bool sign = false)
{
    char *exp_str = &expected_string[0];
    int prefix_length = 0;
    memset(exp_str, 0, MAX_STRING_SIZE);
    if (prefix) {
        prefix_length = strlen(prefix);
        strncpy(exp_str, prefix, prefix_length);
    }
    convert_to_string(value, &exp_str[prefix_length], base, sign);
    if (suffix) {
        strcat(exp_str, suffix);
    }
    return strlen(exp_str);
}

//This function used to extract the prefix format from the fmt
static void extract_prefix(const char *fmt, char *prefix = nullptr)
{
    char *temp_prefix;
    strncpy(expected_string, fmt, strlen(fmt));
    if (prefix) {
        temp_prefix =  strtok(expected_string, "%");
        strncpy(prefix, temp_prefix, strlen(temp_prefix));
    }
}
#endif

static control_t test_printf_d(const size_t call_count)
{
    int result_baseline;
    int result_minimal;
    int result_file;

    /*************************************************************************/
    /*************************************************************************/
    result_minimal = mbed_printf("hhd: %hhd\r\n", SCHAR_MIN);
    result_file = mbed_fprintf(stderr, "hhd: %hhd\r\n", SCHAR_MIN);
#if !defined(__NEWLIB_NANO)
    // The format specifier %hhd is not supported by Newlib-Nano
    result_baseline = printf("hhd: %hhd\r\n", SCHAR_MIN);
#else
    result_baseline = expected_string_length("hhd: ", SCHAR_MIN, RADIX_10, "\r\n", true);
#endif
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);

    result_minimal = mbed_printf("hhd: %hhd\r\n", SCHAR_MAX);
    result_file = mbed_fprintf(stderr, "hhd: %hhd\r\n", SCHAR_MAX);
#if !defined(__NEWLIB_NANO)
    // The format specifier %hhd is not supported by Newlib-Nano
    result_baseline = printf("hhd: %hhd\r\n", SCHAR_MAX);
#else
    result_baseline = expected_string_length("hhd: ", SCHAR_MAX, RADIX_10, "\r\n");
#endif
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);

    result_minimal = mbed_printf("hd: %hd\r\n", SHRT_MIN);
    result_file = mbed_fprintf(stderr, "hd: %hd\r\n", SHRT_MIN);
    result_baseline = printf("hd: %hd\r\n", SHRT_MIN);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);

    result_minimal = mbed_printf("hd: %hd\r\n", SHRT_MAX);
    result_file = mbed_fprintf(stderr, "hd: %hd\r\n", SHRT_MAX);
    result_baseline = printf("hd: %hd\r\n", SHRT_MAX);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);

    result_minimal = mbed_printf("d: %d\r\n", INT_MIN);
    result_file = mbed_fprintf(stderr, "d: %d\r\n", INT_MIN);
    result_baseline = printf("d: %d\r\n", INT_MIN);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);

    result_minimal = mbed_printf("d: %d\r\n", INT_MAX);
    result_file = mbed_fprintf(stderr, "d: %d\r\n", INT_MAX);
    result_baseline = printf("d: %d\r\n", INT_MAX);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);

    result_minimal = mbed_printf("ld: %ld\r\n", LONG_MIN);
    result_file = mbed_fprintf(stderr, "ld: %ld\r\n", LONG_MIN);
    result_baseline = printf("ld: %ld\r\n", LONG_MIN);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);

    result_minimal = mbed_printf("ld: %ld\r\n", LONG_MAX);
    result_file = mbed_fprintf(stderr, "ld: %ld\r\n", LONG_MAX);
    result_baseline = printf("ld: %ld\r\n", LONG_MAX);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);

    result_minimal = mbed_printf("lld: %lld\r\n", LLONG_MIN);
    result_file = mbed_fprintf(stderr, "lld: %lld\r\n", LLONG_MIN);
#if !defined(__NEWLIB_NANO)
    // The format specifier %lld is not supported by Newlib-Nano
    result_baseline = printf("lld: %lld\r\n", LLONG_MIN);
#else
    result_baseline = expected_string_length("lld: ", LLONG_MIN, RADIX_10, "\r\n", true);
#endif
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);

    result_minimal = mbed_printf("lld: %lld\r\n", LLONG_MAX);
    result_file = mbed_fprintf(stderr, "lld: %lld\r\n", LLONG_MAX);
#if !defined(__NEWLIB_NANO)
    // The format specifier %lld is not supported by Newlib-Nano
    result_baseline = printf("lld: %lld\r\n", LLONG_MAX);
#else
    result_baseline = expected_string_length("lld: ", LLONG_MAX, RADIX_10, "\r\n");
#endif
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);

#ifdef TARGET_LIKE_MBED
    printf("%%jd not supported by mbed\r\n");
#else
    result_minimal = mbed_printf("jd: %jd\r\n", INT32_MIN);
    result_file = mbed_fprintf(stderr, "jd: %jd\r\n", INT32_MIN);
    result_baseline = printf("jd: %jd\r\n", (intmax_t) INT32_MIN);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);

    result_minimal = mbed_printf("jd: %jd\r\n", INT32_MAX);
    result_file = mbed_fprintf(stderr, "jd: %jd\r\n", INT32_MAX);
    result_baseline = printf("jd: %jd\r\n", (intmax_t) INT32_MAX);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);
#endif

#ifdef TARGET_LIKE_MBED
    printf("%%zd not supported by mbed\r\n");
#else
    result_minimal = mbed_printf("zd: %zd\r\n", INT32_MIN);
    result_file = mbed_fprintf(stderr, "zd: %zd\r\n", INT32_MIN);
    result_baseline = printf("zd: %zd\r\n", (ssize_t) INT32_MIN);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);

    result_minimal = mbed_printf("zd: %zd\r\n", INT32_MAX);
    result_file = mbed_fprintf(stderr, "zd: %zd\r\n", INT32_MAX);
    result_baseline = printf("zd: %zd\r\n", (ssize_t) INT32_MAX);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);
#endif

#ifdef TARGET_LIKE_MBED
    printf("%%td not supported by mbed\r\n");
#else
    result_minimal = mbed_printf("td: %td\r\n", PTRDIFF_MIN);
    result_file = mbed_fprintf(stderr, "td: %td\r\n", PTRDIFF_MIN);
    result_baseline = printf("td: %td\r\n", PTRDIFF_MIN);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);

    result_minimal = mbed_printf("td: %td\r\n", PTRDIFF_MAX);
    result_file = mbed_fprintf(stderr, "td: %td\r\n", PTRDIFF_MAX);
    result_baseline = printf("td: %td\r\n", PTRDIFF_MAX);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);
#endif

    return CaseNext;
}

static control_t test_printf_u(const size_t call_count)
{
    int result_baseline;
    int result_minimal;
    int result_file;

    /*************************************************************************/
    /*************************************************************************/
    result_minimal = mbed_printf("hhu: %hhu\r\n", 0);
    result_file = mbed_fprintf(stderr, "hhu: %hhu\r\n", 0);
#if !defined(__NEWLIB_NANO)
    // The format specifier %hhu is not supported by Newlib-Nano
    result_baseline = printf("hhu: %hhu\r\n", 0);
#else
    result_baseline = expected_string_length("hhu: ", 0, RADIX_10, "\r\n");
#endif
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);

    result_minimal = mbed_printf("hhu: %hhu\r\n", UCHAR_MAX);
    result_file = mbed_fprintf(stderr, "hhu: %hhu\r\n", UCHAR_MAX);
#if !defined(__NEWLIB_NANO)
    // The format specifier %hhu is not supported by Newlib-Nano
    result_baseline = printf("hhu: %hhu\r\n", UCHAR_MAX);
#else
    result_baseline = expected_string_length("hhu: ", UCHAR_MAX, RADIX_10, "\r\n");
#endif
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);

    result_minimal = mbed_printf("hu: %hu\r\n", 0);
    result_file = mbed_fprintf(stderr, "hu: %hu\r\n", 0);
    result_baseline = printf("hu: %hu\r\n", 0);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);

    result_minimal = mbed_printf("hu: %hu\r\n", USHRT_MAX);
    result_file = mbed_fprintf(stderr, "hu: %hu\r\n", USHRT_MAX);
    result_baseline = printf("hu: %hu\r\n", USHRT_MAX);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);

    result_minimal = mbed_printf("u: %u\r\n", 0);
    result_file = mbed_fprintf(stderr, "u: %u\r\n", 0);
    result_baseline = printf("u: %u\r\n", 0);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);

    result_minimal = mbed_printf("u: %u\r\n", UINT_MAX);
    result_file = mbed_fprintf(stderr, "u: %u\r\n", UINT_MAX);
    result_baseline = printf("u: %u\r\n", UINT_MAX);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);

    result_minimal = mbed_printf("lu: %lu\r\n", 0UL);
    result_file = mbed_fprintf(stderr, "lu: %lu\r\n", 0UL);
    result_baseline = printf("lu: %lu\r\n", 0UL);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);

    result_minimal = mbed_printf("lu: %lu\r\n", ULONG_MAX);
    result_file = mbed_fprintf(stderr, "lu: %lu\r\n", ULONG_MAX);
    result_baseline = printf("lu: %lu\r\n", ULONG_MAX);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);

    result_minimal = mbed_printf("llu: %llu\r\n", 0ULL);
    result_file = mbed_fprintf(stderr, "llu: %llu\r\n", 0ULL);
#if !defined(__NEWLIB_NANO)
    // The format specifier %llu is not supported by Newlib-Nano
    result_baseline = printf("llu: %llu\r\n", 0ULL);
#else
    result_baseline = expected_string_length("llu: ", 0ULL, RADIX_10, "\r\n");
#endif
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);

    result_minimal = mbed_printf("llu: %llu\r\n", ULLONG_MAX);
    result_file = mbed_fprintf(stderr, "llu: %llu\r\n", ULLONG_MAX);
#if !defined(__NEWLIB_NANO)
    // The format specifier %llu is not supported by Newlib-Nano
    result_baseline = printf("llu: %llu\r\n", ULLONG_MAX);
#else
    result_baseline = expected_string_length("llu: ", ULLONG_MAX, RADIX_10, "\r\n");
#endif
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);

#ifdef TARGET_LIKE_MBED
    printf("%%ju not supported by mbed\r\n");
#else
    result_minimal = mbed_printf("ju: %ju\r\n", (uintmax_t) 0);
    result_file = mbed_fprintf(stderr, "ju: %ju\r\n", (uintmax_t) 0);
    result_baseline = printf("ju: %ju\r\n", (uintmax_t) 0);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);

    result_minimal = mbed_printf("ju: %ju\r\n", UINTMAX_MAX);
    result_file = mbed_fprintf(stderr, "ju: %ju\r\n", UINTMAX_MAX);
    result_baseline = printf("ju: %ju\r\n", UINTMAX_MAX);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);
#endif

#ifdef TARGET_LIKE_MBED
    printf("%%zu not supported by mbed\r\n");
#else
    result_minimal = mbed_printf("zu: %zu\r\n", 0);
    result_file = mbed_fprintf(stderr, "zu: %zu\r\n", 0);
    result_baseline = printf("zu: %zu\r\n", 0);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);

    result_minimal = mbed_printf("zu: %zu\r\n", SIZE_MAX);
    result_file = mbed_fprintf(stderr, "zu: %zu\r\n", SIZE_MAX);
    result_baseline = printf("zu: %zu\r\n", SIZE_MAX);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);
#endif

#ifdef TARGET_LIKE_MBED
    printf("%%tu not supported by mbed\r\n");
#else
    result_minimal = mbed_printf("tu: %tu\r\n", 0);
    result_file = mbed_fprintf(stderr, "tu: %tu\r\n", 0);
    result_baseline = printf("tu: %tu\r\n", 0);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);

    result_minimal = mbed_printf("tu: %tu\r\n", UINTPTR_MAX);
    result_file = mbed_fprintf(stderr, "tu: %tu\r\n", UINTPTR_MAX);
    result_baseline = printf("tu: %tu\r\n", UINTPTR_MAX);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);
#endif

    return CaseNext;
}

static control_t test_printf_x(const size_t call_count)
{
    int result_baseline;
    int result_minimal;
    int result_file;

    /*************************************************************************/
    /*************************************************************************/
    result_minimal = mbed_printf("hhX: %hhX\r\n", 0);
    result_file = mbed_fprintf(stderr, "hhX: %hhX\r\n", 0);
#if !defined(__NEWLIB_NANO)
    // The format specifier %hhX is not supported by Newlib-Nano
    result_baseline = printf("hhX: %hhX\r\n", 0);
#else
    result_baseline = expected_string_length("hhX: ", 0, RADIX_16, "\r\n");
#endif
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);

    result_minimal = mbed_printf("hhX: %hhX\r\n", UCHAR_MAX);
    result_file = mbed_fprintf(stderr, "hhX: %hhX\r\n", UCHAR_MAX);
#if !defined(__NEWLIB_NANO)
    // The format specifier %hhX is not supported by Newlib-Nano
    result_baseline = printf("hhX: %hhX\r\n", UCHAR_MAX);
#else
    result_baseline = expected_string_length("hhX: ", UCHAR_MAX, RADIX_16, "\r\n");
#endif
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);

    result_minimal = mbed_printf("hX: %hX\r\n", 0);
    result_file = mbed_fprintf(stderr, "hX: %hX\r\n", 0);
    result_baseline = printf("hX: %hX\r\n", 0);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);

    result_minimal = mbed_printf("hX: %hX\r\n", USHRT_MAX);
    result_file = mbed_fprintf(stderr, "hX: %hX\r\n", USHRT_MAX);
    result_baseline = printf("hX: %hX\r\n", USHRT_MAX);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);

    result_minimal = mbed_printf("X: %X\r\n", 0);
    result_file = mbed_fprintf(stderr, "X: %X\r\n", 0);
    result_baseline = printf("X: %X\r\n", 0);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);

    result_minimal = mbed_printf("X: %X\r\n", UINT_MAX);
    result_file = mbed_fprintf(stderr, "X: %X\r\n", UINT_MAX);
    result_baseline = printf("X: %X\r\n", UINT_MAX);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);

    result_minimal = mbed_printf("lX: %lX\r\n", 0UL);
    result_file = mbed_fprintf(stderr, "lX: %lX\r\n", 0UL);
    result_baseline = printf("lX: %lX\r\n", 0UL);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);

    result_minimal = mbed_printf("lX: %lX\r\n", ULONG_MAX);
    result_file = mbed_fprintf(stderr, "lX: %lX\r\n", ULONG_MAX);
    result_baseline = printf("lX: %lX\r\n", ULONG_MAX);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);

    result_minimal = mbed_printf("llX: %llX\r\n", 0ULL);
    result_file = mbed_fprintf(stderr, "llX: %llX\r\n", 0ULL);
#if !defined(__NEWLIB_NANO)
    // The format specifier %llX is not supported by Newlib-Nano
    result_baseline = printf("llX: %llX\r\n", 0ULL);
#else
    result_baseline = expected_string_length("llX: ", 0ULL, RADIX_16, "\r\n");
#endif
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);

    result_minimal = mbed_printf("llX: %llX\r\n", ULLONG_MAX);
    result_file = mbed_fprintf(stderr, "llX: %llX\r\n", ULLONG_MAX);
#if !defined(__NEWLIB_NANO)
    // The format specifier %llX is not supported by Newlib-Nano
    result_baseline = printf("llX: %llX\r\n", ULLONG_MAX);
#else
    result_baseline = expected_string_length("llX: ", ULLONG_MAX, RADIX_16, "\r\n");
#endif
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);

#ifdef TARGET_LIKE_MBED
    printf("%%jX not supported by mbed\r\n");
#else
    result_minimal = mbed_printf("jX: %jX\r\n", (uintmax_t) 0);
    result_file = mbed_fprintf(stderr, "jX: %jX\r\n", (uintmax_t) 0);
    result_baseline = printf("jX: %jX\r\n", (uintmax_t) 0);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);

    result_minimal = mbed_printf("jX: %jX\r\n", UINTMAX_MAX);
    result_file = mbed_fprintf(stderr, "jX: %jX\r\n", UINTMAX_MAX);
    result_baseline = printf("jX: %jX\r\n", UINTMAX_MAX);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);
#endif

#ifdef TARGET_LIKE_MBED
    printf("%%zX not supported by mbed\r\n");
#else
    result_minimal = mbed_printf("zX: %zX\r\n", 0);
    result_file = mbed_fprintf(stderr, "zX: %zX\r\n", 0);
    result_baseline = printf("zX: %zX\r\n", 0);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);

    result_minimal = mbed_printf("zX: %zX\r\n", SIZE_MAX);
    result_file = mbed_fprintf(stderr, "zX: %zX\r\n", SIZE_MAX);
    result_baseline = printf("zX: %zX\r\n", SIZE_MAX);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);
#endif

#ifdef TARGET_LIKE_MBED
    printf("%%tX not supported by mbed\r\n");
#else
    result_minimal = mbed_printf("tX: %tX\r\n", 0);
    result_file = mbed_fprintf(stderr, "tX: %tX\r\n", 0);
    result_baseline = printf("tX: %tX\r\n", 0);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);

    result_minimal = mbed_printf("tX: %tX\r\n", UINTPTR_MAX);
    result_file = mbed_fprintf(stderr, "tX: %tX\r\n", UINTPTR_MAX);
    result_baseline = printf("tX: %tX\r\n", UINTPTR_MAX);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);
#endif

    result_minimal = mbed_printf("x: %x\r\n", 11259375);
    result_baseline = printf("x: %x\r\n", 11259375);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    return CaseNext;
}

static control_t test_printf_percent(const size_t call_count)
{
    int result_baseline;
    int result_minimal;
    int result_file;

    result_minimal = mbed_printf("%% \r\n");
    result_file = mbed_fprintf(stderr, "%% \r\n");
    result_baseline = printf("%% \r\n");
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_file);

    return CaseNext;
}

/******************************************************************************/
/*                                                                            */
/* SNPRINTF                                                                   */
/*                                                                            */
/******************************************************************************/

static control_t test_snprintf_d(const size_t call_count)
{
    char buffer_baseline[100];
    char buffer_minimal[100];
    int result_baseline;
    int result_minimal;

    /*************************************************************************/
    /*************************************************************************/
    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "hhd: %hhd\r\n", SCHAR_MIN);
#if !defined(__NEWLIB_NANO)
    // The format specifier %hhd is not supported by Newlib-Nano
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "hhd: %hhd\r\n", SCHAR_MIN);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
#else
    result_baseline = expected_string_length("hhd: ", SCHAR_MIN, RADIX_10, "\r\n", true);
    TEST_ASSERT_EQUAL_STRING(expected_string, buffer_minimal);
#endif
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "hhd: %hhd\r\n", SCHAR_MAX);
#if !defined(__NEWLIB_NANO)
    // The format specifier %hhd is not supported by Newlib-Nano
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "hhd: %hhd\r\n", SCHAR_MAX);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
#else
    result_baseline = expected_string_length("hhd: ", SCHAR_MAX, RADIX_10, "\r\n");
    TEST_ASSERT_EQUAL_STRING(expected_string, buffer_minimal);
#endif
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "hd: %hd\r\n", SHRT_MIN);
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "hd: %hd\r\n", SHRT_MIN);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "hd: %hd\r\n", SHRT_MAX);
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "hd: %hd\r\n", SHRT_MAX);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "d: %d\r\n", INT_MIN);
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "d: %d\r\n", INT_MIN);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "d: %d\r\n", INT_MAX);
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "d: %d\r\n", INT_MAX);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "ld: %ld\r\n", LONG_MIN);
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "ld: %ld\r\n", LONG_MIN);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "ld: %ld\r\n", LONG_MAX);
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "ld: %ld\r\n", LONG_MAX);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "lld: %lld\r\n", LLONG_MIN);
#if !defined(__NEWLIB_NANO)
    // The format specifier %lld is not supported by Newlib-Nano
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "lld: %lld\r\n", LLONG_MIN);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
#else
    result_baseline = expected_string_length("lld: ", LLONG_MIN, RADIX_10, "\r\n", true);
    TEST_ASSERT_EQUAL_STRING(expected_string, buffer_minimal);
#endif

    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "lld: %lld\r\n", LLONG_MAX);
#if !defined(__NEWLIB_NANO)
    // The format specifier %lld is not supported by Newlib-Nano
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "lld: %lld\r\n", LLONG_MAX);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
#else
    result_baseline = expected_string_length("lld: ", LLONG_MAX, RADIX_10, "\r\n");
    TEST_ASSERT_EQUAL_STRING(expected_string, buffer_minimal);
#endif
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

#ifdef TARGET_LIKE_MBED
    printf("%%jd not supported by mbed\r\n");
#else
    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "jd: %jd\r\n", (intmax_t) INT32_MIN);
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "jd: %jd\r\n", (intmax_t) INT32_MIN);
    TEST_ASSERT_EQUAL_STRING("jd: -2147483648\r\n", buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "jd: %jd\r\n", (intmax_t) INT32_MAX);
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "jd: %jd\r\n", (intmax_t) INT32_MAX);
    TEST_ASSERT_EQUAL_STRING("jd: 2147483647\r\n", buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
#endif

#ifdef TARGET_LIKE_MBED
    printf("%%zd not supported by mbed\r\n");
#else
    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "zd: %zd\r\n", (ssize_t) INT32_MIN);
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "zd: %zd\r\n", (ssize_t) INT32_MIN);
    TEST_ASSERT_EQUAL_STRING("zd: -2147483648\r\n", buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "zd: %zd\r\n", (ssize_t) INT32_MAX);
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "zd: %zd\r\n", (ssize_t) INT32_MAX);
    TEST_ASSERT_EQUAL_STRING("zd: 2147483647\r\n", buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
#endif

#ifdef TARGET_LIKE_MBED
    printf("%%td not supported by mbed\r\n");
#else
    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "td: %td\r\n", PTRDIFF_MIN);
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "td: %td\r\n", PTRDIFF_MIN);
    TEST_ASSERT_EQUAL_STRING("td: -2147483648\r\n", buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "td: %td\r\n", PTRDIFF_MAX);
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "td: %td\r\n", PTRDIFF_MAX);
    TEST_ASSERT_EQUAL_STRING("td: 2147483647\r\n", buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
#endif

    return CaseNext;
}

static control_t test_snprintf_u(const size_t call_count)
{
    char buffer_baseline[100];
    char buffer_minimal[100];
    int result_baseline;
    int result_minimal;

    /*************************************************************************/
    /*************************************************************************/
    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "hhu: %hhu\r\n", 0);
#if !defined(__NEWLIB_NANO)
    // The format specifier %hhu is not supported by Newlib-Nano
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "hhu: %hhu\r\n", 0);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
#else
    result_baseline = expected_string_length("hhu: ", 0, RADIX_10, "\r\n");
    TEST_ASSERT_EQUAL_STRING(expected_string, buffer_minimal);
#endif
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "hhu: %hhu\r\n", UCHAR_MAX);
#if !defined(__NEWLIB_NANO)
    // The format specifier %hhu is not supported by Newlib-Nano
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "hhu: %hhu\r\n", UCHAR_MAX);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
#else
    result_baseline = expected_string_length("hhu: ", UCHAR_MAX, RADIX_10, "\r\n");
    TEST_ASSERT_EQUAL_STRING(expected_string, buffer_minimal);
#endif
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "hu: %hu\r\n", 0);
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "hu: %hu\r\n", 0);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "hu: %hu\r\n", USHRT_MAX);
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "hu: %hu\r\n", USHRT_MAX);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "u: %u\r\n", 0);
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "u: %u\r\n", 0);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "u: %u\r\n", UINT_MAX);
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "u: %u\r\n", UINT_MAX);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "lu: %lu\r\n", 0UL);
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "lu: %lu\r\n", 0UL);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "lu: %lu\r\n", ULONG_MAX);
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "lu: %lu\r\n", ULONG_MAX);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "llu: %llu\r\n", 0ULL);
#if !defined(__NEWLIB_NANO)
    // The format specifier %llu is not supported by Newlib-Nano
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "llu: %llu\r\n", 0ULL);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
#else
    result_baseline = expected_string_length("llu: ", 0ULL, RADIX_10, "\r\n");
    TEST_ASSERT_EQUAL_STRING(expected_string, buffer_minimal);
#endif
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "llu: %llu\r\n", ULLONG_MAX);
#if !defined(__NEWLIB_NANO)
    // The format specifier %llu is not supported by Newlib-Nano
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "llu: %llu\r\n", ULLONG_MAX);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
#else
    result_baseline = expected_string_length("llu: ", ULLONG_MAX, RADIX_10, "\r\n");
    TEST_ASSERT_EQUAL_STRING(expected_string, buffer_minimal);
#endif
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

#ifdef TARGET_LIKE_MBED
    printf("%%ju not supported by mbed\r\n");
#else
    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "ju: %ju\r\n", (uintmax_t) 0);
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "ju: %ju\r\n", (uintmax_t) 0);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "ju: %ju\r\n", UINTMAX_MAX);
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "ju: %ju\r\n", UINTMAX_MAX);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
#endif

#ifdef TARGET_LIKE_MBED
    printf("%%zu not supported by mbed\r\n");
#else
    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "zu: %zu\r\n", 0);
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "zu: %zu\r\n", 0);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "zu: %zu\r\n", SIZE_MAX);
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "zu: %zu\r\n", SIZE_MAX);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
#endif

#ifdef TARGET_LIKE_MBED
    printf("%%tu not supported by mbed\r\n");
#else
    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "tu: %tu\r\n", 0);
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "tu: %tu\r\n", 0);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "tu: %tu\r\n", UINTPTR_MAX);
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "tu: %tu\r\n", UINTPTR_MAX);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
#endif

    return CaseNext;
}

static control_t test_snprintf_x(const size_t call_count)
{
    char buffer_baseline[100];
    char buffer_minimal[100];
    int result_baseline;
    int result_minimal;

    /*************************************************************************/
    /*************************************************************************/
    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "hhX: %hhX\r\n", 0);
#if !defined(__NEWLIB_NANO)
    // The format specifier %hhX is not supported by Newlib-Nano
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "hhX: %hhX\r\n", 0);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
#else
    result_baseline = expected_string_length("hhX: ", 0, RADIX_16, "\r\n");
    TEST_ASSERT_EQUAL_STRING(expected_string, buffer_minimal);
#endif
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "hhX: %hhX\r\n", UCHAR_MAX);
#if !defined(__NEWLIB_NANO)
    // The format specifier %hhX is not supported by Newlib-Nano
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "hhX: %hhX\r\n", UCHAR_MAX);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
#else
    result_baseline = expected_string_length("hhX: ", UCHAR_MAX, RADIX_16, "\r\n");
    TEST_ASSERT_EQUAL_STRING(expected_string, buffer_minimal);
#endif
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "hX: %hX\r\n", 0);
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "hX: %hX\r\n", 0);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "hX: %hX\r\n", USHRT_MAX);
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "hX: %hX\r\n", USHRT_MAX);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "X: %X\r\n", 0);
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "X: %X\r\n", 0);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "X: %X\r\n", UINT_MAX);
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "X: %X\r\n", UINT_MAX);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "lX: %lX\r\n", 0UL);
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "lX: %lX\r\n", 0UL);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "lX: %lX\r\n", ULONG_MAX);
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "lX: %lX\r\n", ULONG_MAX);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "llX: %llX\r\n", 0ULL);
#if !defined(__NEWLIB_NANO)
    // The format specifier %llX is not supported by Newlib-Nano
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "llX: %llX\r\n", 0ULL);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
#else
    result_baseline = expected_string_length("llX: ", 0ULL, RADIX_16, "\r\n");
    TEST_ASSERT_EQUAL_STRING(expected_string, buffer_minimal);
#endif
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "llX: %llX\r\n", ULLONG_MAX);
#if !defined(__NEWLIB_NANO)
    // The format specifier %llX is not supported by Newlib-Nano
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "llX: %llX\r\n", ULLONG_MAX);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
#else
    result_baseline = expected_string_length("llX: ", ULLONG_MAX, RADIX_16, "\r\n");
    TEST_ASSERT_EQUAL_STRING(expected_string, buffer_minimal);
#endif
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

#ifdef TARGET_LIKE_MBED
    printf("%%jX not supported by mbed\r\n");
#else
    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "jX: %jX\r\n", (uintmax_t) 0);
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "jX: %jX\r\n", (uintmax_t) 0);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "jX: %jX\r\n", UINTMAX_MAX);
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "jX: %jX\r\n", UINTMAX_MAX);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
#endif

#ifdef TARGET_LIKE_MBED
    printf("%%xX not supported by mbed\r\n");
#else
    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "zX: %zX\r\n", 0);
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "zX: %zX\r\n", 0);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "zX: %zX\r\n", SIZE_MAX);
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "zX: %zX\r\n", SIZE_MAX);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
#endif

#ifdef TARGET_LIKE_MBED
    printf("%%tX not supported by mbed\r\n");
#else
    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "tX: %tX\r\n", 0);
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "tX: %tX\r\n", 0);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "tX: %tX\r\n", UINTPTR_MAX);
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "tX: %tX\r\n", UINTPTR_MAX);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
#endif

    return CaseNext;
}

static control_t test_snprintf_percent(const size_t call_count)
{
    char buffer_baseline[100];
    char buffer_minimal[100];
    int result_baseline;
    int result_minimal;

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "%% \r\n");
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "%% \r\n");
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    return CaseNext;
}

static control_t test_snprintf_unsupported_specifier(const size_t call_count)
{
    char buffer_minimal[100];

    TEST_ASSERT_NOT_EQUAL(
        0,
        mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "%a \r\n", 5)
    );
    TEST_ASSERT_EQUAL_STRING("%a \r\n", buffer_minimal);

    return CaseNext;
}

#if MBED_CONF_PLATFORM_MINIMAL_PRINTF_ENABLE_FLOATING_POINT
static control_t test_printf_f(const size_t call_count)
{
    int result_baseline;
    int result_minimal;

    /*************************************************************************/
    /*************************************************************************/

    double pi = 3.14159265359;


    result_minimal = mbed_printf("f: %f\r\n", 3.0089);
    result_baseline = printf("f: %f\r\n", 3.0089);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_printf("f: %f\r\n", 7.0);
    result_baseline = printf("f: %f\r\n", 7.0);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_printf("f: %f\r\n", -1 * pi);
    result_baseline = printf("f: %f\r\n", -1 * pi);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_printf("f: %f\r\n", 0.0);
    result_baseline = printf("f: %f\r\n", 0.0);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_printf("f: %f\r\n", pi);
    result_baseline = printf("f: %f\r\n", pi);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    return CaseNext;
}

static control_t test_snprintf_f(const size_t call_count)
{
    char buffer_baseline[100];
    char buffer_minimal[100];
    int result_baseline;
    int result_minimal;

    /*************************************************************************/
    /*************************************************************************/

    double pi = 3.14159265359;

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "f: %f\r\n", 3.0089);
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "f: %f\r\n", 3.0089);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "f: %f\r\n", 7.0);
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "f: %f\r\n", 7.0);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "f: %f\r\n", -1 * pi);
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "f: %f\r\n", -1 * pi);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "f: %f\r\n", 0.0);
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "f: %f\r\n", 0.0);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    result_minimal = mbed_snprintf(buffer_minimal, sizeof(buffer_minimal), "f: %f\r\n", pi);
    result_baseline = snprintf(buffer_baseline, sizeof(buffer_baseline), "f: %f\r\n", pi);
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    return CaseNext;
}
#endif


/* Generic buffer overflow test function.
 * Template parameters:
    * 'T' is the type being tested
    * 'buf_size' is the buffer size used in tests
    * 'radix_base' is used when Newlib-nano non-supported printf formats to calculate expected_string_length
  * Function parameters:
    * 'fmt' is the format to use for sprintf
    * 'data' is the data that will be printed
    * 'sign' boolenan true for negative number,false for positive number
*/
#if !defined(__NEWLIB_NANO)
template<typename T, size_t buf_size>
#else
template<typename T, size_t buf_size, radix_base base = RADIX_10>
#endif
static control_t test_snprintf_buffer_overflow_generic(const char *fmt, T data, bool sign = false)
{
    char buffer_baseline[buf_size];
    char buffer_minimal[buf_size];
    int result_baseline;
    int result_minimal;
#if defined(__NEWLIB_NANO)
    char prefix[buf_size];
    memset(prefix, 0, buf_size);
    extract_prefix(fmt, &prefix[0]);
    result_baseline = expected_string_length(prefix, data, base, nullptr, sign);
#endif
    memset(buffer_baseline, 0, buf_size);
#if !defined(__MICROLIB)
    // Microlib snprintf always returns zero if the size
    // to copy from the buffer is zero.
    // See reported microlib bug SDCOMP-54710

    /* empty buffer test */
    result_minimal = mbed_snprintf(buffer_minimal, 0, fmt, data);
#if !defined(__NEWLIB_NANO)
    result_baseline = snprintf(buffer_baseline, 0, fmt, data);
#endif
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);
#endif

    /* buffer isn't large enough, output needs to be truncated */
    result_minimal = mbed_snprintf(buffer_minimal, buf_size - 2, fmt, data);
#if !defined(__NEWLIB_NANO)
    result_baseline = snprintf(buffer_baseline, buf_size - 2, fmt, data);
#else
    strncpy(buffer_baseline, expected_string, buf_size - 3);
#endif
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    /* buffer is one byte shorter than needed, string terminator must
       be written and output must be truncated */
    result_minimal = mbed_snprintf(buffer_minimal, buf_size - 1, fmt, data);
#if !defined(__NEWLIB_NANO)
    result_baseline = snprintf(buffer_baseline, buf_size - 1, fmt, data);
#else
    strncpy(buffer_baseline, expected_string, buf_size - 2);
#endif
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    /* buffer is just long enough */
    result_minimal = mbed_snprintf(buffer_minimal, buf_size, fmt, data);
#if !defined(__NEWLIB_NANO)
    result_baseline = snprintf(buffer_baseline, buf_size, fmt, data);
#else
    strncpy(buffer_baseline, expected_string, buf_size);
#endif
    TEST_ASSERT_EQUAL_STRING(buffer_baseline, buffer_minimal);
    TEST_ASSERT_EQUAL_INT(result_baseline, result_minimal);

    return CaseNext;
}

/* Based on the generic buffer overflow function above, create tests for
   each relevant data type. In each case, the buffer for printing will only
   be large enough to fit the printed data. */
static control_t test_snprintf_buffer_overflow_d(const size_t call_count)
{
    return test_snprintf_buffer_overflow_generic<int, sizeof("d: -1024")>("d: %d", -1024);
}

static control_t test_snprintf_buffer_overflow_ld(const size_t call_count)
{
    return test_snprintf_buffer_overflow_generic<long, sizeof("ld: -1048576")>("ld: %ld", -1048576L);
}

static control_t test_snprintf_buffer_overflow_lld(const size_t call_count)
{
    return test_snprintf_buffer_overflow_generic<long long, sizeof("lld: -1099511627776")>("lld: %lld", -1099511627776LL);
}

static control_t test_snprintf_buffer_overflow_u(const size_t call_count)
{
    return test_snprintf_buffer_overflow_generic<unsigned int, sizeof("u: 1024")>("u: %u", 1024);
}

static control_t test_snprintf_buffer_overflow_lu(const size_t call_count)
{
    return test_snprintf_buffer_overflow_generic<unsigned long, sizeof("lu: 1048576")>("lu: %lu", 1048576UL);
}

static control_t test_snprintf_buffer_overflow_llu(const size_t call_count)
{
    return test_snprintf_buffer_overflow_generic<unsigned long long, sizeof("llu: 1099511627776")>("llu: %llu", 1099511627776ULL);
}

static control_t test_snprintf_buffer_overflow_x(const size_t call_count)
{
#if !defined(__NEWLIB_NANO)
    return test_snprintf_buffer_overflow_generic<unsigned int, sizeof("x: 0x400")>("x: 0x%x", 0x400);
#else
    return test_snprintf_buffer_overflow_generic<unsigned int, sizeof("x: 0x400"), RADIX_16>("x: 0x%x", 0x400);
#endif
}

static control_t test_snprintf_buffer_overflow_lx(const size_t call_count)
{
#if !defined(__NEWLIB_NANO)
    return test_snprintf_buffer_overflow_generic<unsigned long, sizeof("lx: 0x100000")>("lx: 0x%lx", 0x100000UL);
#else
    return test_snprintf_buffer_overflow_generic<unsigned long, sizeof("lx: 0x100000"), RADIX_16>("lx: 0x%lx", 0x100000UL);
#endif
}

static control_t test_snprintf_buffer_overflow_llx(const size_t call_count)
{
#if !defined(__NEWLIB_NANO)
    return test_snprintf_buffer_overflow_generic<unsigned long long, sizeof("llx: 0x10000000000")>("llx: 0x%llx", 0x10000000000ULL);
#else
    return test_snprintf_buffer_overflow_generic<unsigned long long, sizeof("llx: 0x10000000000"), RADIX_16>("llx: 0x%llx", 0x10000000000ULL);
#endif
}

utest::v1::status_t greentea_setup(const size_t number_of_cases)
{
    GREENTEA_SETUP(30 * 60, "default_auto");
    return greentea_test_setup_handler(number_of_cases);
}

Case cases[] = {
    Case("printf %d", test_printf_d),
    Case("snprintf %d", test_snprintf_d),
    Case("printf %u", test_printf_u),
    Case("snprintf %u", test_snprintf_u),
    Case("printf %x", test_printf_x),
    Case("snprintf %x", test_snprintf_x),
    Case("printf %%", test_printf_percent),
    Case("snprintf %%", test_snprintf_percent),
    Case("snprintf unsupported specifier", test_snprintf_unsupported_specifier),
#if MBED_CONF_PLATFORM_MINIMAL_PRINTF_ENABLE_FLOATING_POINT
    Case("printf %f", test_printf_f),
    Case("snprintf %f", test_snprintf_f),
#endif
    Case("snprintf buffer overflow %d", test_snprintf_buffer_overflow_d),
    Case("snprintf buffer overflow %ld", test_snprintf_buffer_overflow_ld),
    Case("snprintf buffer overflow %lld", test_snprintf_buffer_overflow_lld),
    Case("snprintf buffer overflow %u", test_snprintf_buffer_overflow_u),
    Case("snprintf buffer overflow %lu", test_snprintf_buffer_overflow_lu),
    Case("snprintf buffer overflow %llu", test_snprintf_buffer_overflow_llu),
    Case("snprintf buffer overflow %x", test_snprintf_buffer_overflow_x),
    Case("snprintf buffer overflow %lx", test_snprintf_buffer_overflow_lx),
    Case("snprintf buffer overflow %llx", test_snprintf_buffer_overflow_llx),
};

Specification specification(greentea_setup, cases, greentea_test_teardown_handler);

int main()
{
    return !Harness::run(specification);
}
