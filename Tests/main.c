/* ************************************************************************** */
/*                                                                            */
/*   main.c - ft_printf stress test (MANDATORY part only: cspdiuxX%)          */
/*                                                                            */
/*   Every CHECK(...) runs the real printf and your ft_printf with the same   */
/*   arguments and compares: output bytes, return value, return == bytes.     */
/*                                                                            */
/* ************************************************************************** */

#include <ctype.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ft_printf.h"
#include "tester.h"

/* a pointer built from an integer, with the right type for %p */
#define PTR(v) ((void *)(uintptr_t)(v))

/* %x and %X on the same (unsigned int) value */
#define CHECK_HEX(v) \
	do { \
		CHECK("%x", v); \
		CHECK("%X", v); \
	} while (0)

/* NULL %s / NULL %p are undefined by the C standard: glibc prints
** "(null)" and "(nil)". These tests only make sense on Linux/glibc. */
#define CHECK_GLIBC(...) \
	do { \
		note("Linux/glibc-specific behavior"); \
		CHECK(__VA_ARGS__); \
	} while (0)

typedef int	(*t_pf)(const char *, ...);

typedef struct s_point
{
	int		x;
	int		y;
	char	tag;
}	t_point;

typedef struct s_opts
{
	int		verbose;
	int		all;
	char	sel[32];
	int		nsel;
}	t_opts;

typedef struct s_section
{
	char		id;
	const char	*title;
	void		(*run)(void);
}	t_section;

static char	g_long_1k[1025];
static char	g_long_10k[10001];
static char	g_long_100k[100001];
static char	g_big[70001];
static char	g_fmt[20000];
static int	g_global = 42;

/* ************************************************************************** */
/*  A. Basic tests                                                            */
/* ************************************************************************** */

static void	test_basic(void)
{
	int	n;

	n = 42;
	CHECK("Hello, world!");
	CHECK("");
	CHECK("a");
	CHECK(" ");
	CHECK("\n");
	CHECK("\n\n\n");
	CHECK("\t");
	CHECK("No conversion in this format string.\n");
	CHECK("Special: !@#$^&*()_+-=[]{};':,.<>/?|~`");
	CHECK("%c", 'x');
	CHECK("%s", "hello");
	CHECK("%p", (void *)&n);
	CHECK("%d", 42);
	CHECK("%i", 42);
	CHECK("%u", 42u);
	CHECK("%x", 42u);
	CHECK("%X", 42u);
	CHECK("%%");
	CHECK("%c%c%c", 'a', 'b', 'c');
	CHECK("%s%s%s", "foo", "bar", "baz");
	CHECK("%d%d%d", 1, 2, 3);
	CHECK("%i%i%i", 4, 5, 6);
	CHECK("%u%u%u", 1u, 2u, 3u);
	CHECK("%x%x%x", 1u, 2u, 3u);
	CHECK("%X%X%X", 10u, 11u, 12u);
	CHECK("%d %s %c", 42, "text", 'Z');
	CHECK("Value: %d\n", 1234);
	CHECK("%s\n", "newline at the end");
	CHECK("abc%cdef", 'X');
	CHECK("%cabc", 'X');
	CHECK("abc%c", 'X');
	CHECK("  %d  ", 5);
	CHECK("%d is a number and %s is a word", 7, "seven");
}

/* ************************************************************************** */
/*  B. Character tests (%c)                                                   */
/* ************************************************************************** */

static void	test_char_plain(void)
{
	CHECK("%c", 'a');
	CHECK("%c", 'z');
	CHECK("%c", 'A');
	CHECK("%c", 'Z');
	CHECK("%c", '0');
	CHECK("%c", '9');
	CHECK("%c", ' ');
	CHECK("%c", '\n');
	CHECK("%c", '\t');
	CHECK("%c", '\r');
	CHECK("%c", '\\');
	CHECK("%c", '\'');
	CHECK("%c", '"');
	CHECK("%c", '%');
	CHECK("%c", '~');
	CHECK("%c", '!');
	CHECK("%c", '@');
	CHECK("%c", '#');
	CHECK("%c", '$');
	CHECK("%c", '`');
	CHECK("%c", '\a');
	CHECK("%c", '\b');
	CHECK("%c", '\f');
	CHECK("%c", '\v');
	CHECK("%c", 1);
	CHECK("%c", 27);
	CHECK("%c", 127);
	CHECK("%c", 128);
	CHECK("%c", 200);
	CHECK("%c", 255);
	CHECK("%c", (char)-1);
	CHECK("%c", (unsigned char)255);
	CHECK("%c%c", 'a', 'b');
	CHECK("%c %c %c", '1', '2', '3');
	CHECK("[%c]", 'x');
	CHECK("%c\n", 'x');
}

static void	test_char_null(void)
{
	CHECK("%c", '\0');
	CHECK("%c", 0);
	CHECK("a%cb", '\0');
	CHECK("%c%c", '\0', '\0');
	CHECK("%c%c%c", '\0', '\0', '\0');
	CHECK("%c%c%c", 'a', '\0', 'b');
	CHECK("%c%c%c", '\0', 'a', '\0');
	CHECK("%c text", '\0');
	CHECK("text %c", '\0');
	CHECK("[%c]", '\0');
	CHECK("%c%c%c%c%c%c%c%c", 0, 0, 0, 0, 0, 0, 0, 0);
	CHECK("%c%s%c", '\0', "mid", '\0');
	CHECK("%s%c%s", "left", '\0', "right");
	CHECK("%c%d%c%d", '\0', 1, '\0', 2);
	CHECK("%d%c%x", 42, '\0', 255u);
	CHECK("%c\n", '\0');
	CHECK("%c%%%c", '\0', '\0');
}

static void	test_char(void)
{
	test_char_plain();
	test_char_null();
}

/* ************************************************************************** */
/*  C. String tests (%s)    (NULL strings are in section L: glibc only)       */
/* ************************************************************************** */

static void	test_string_plain(void)
{
	CHECK("%s", "Hello");
	CHECK("%s", "");
	CHECK("%s", "a");
	CHECK("%s", "ab");
	CHECK("%s", "Hello, World!");
	CHECK("%s", "multiple   spaces   inside");
	CHECK("%s", "   leading spaces");
	CHECK("%s", "trailing spaces   ");
	CHECK("%s", " ");
	CHECK("%s", "\n");
	CHECK("%s", "line1\nline2\nline3\n");
	CHECK("%s", "tab\tseparated\tvalues");
	CHECK("%s", "carriage\rreturn");
	CHECK("%s", "100%");
	CHECK("%s", "%s");
	CHECK("%s", "%d %i %u %x %X %p %c %%");
	CHECK("%s", "%%");
	CHECK("%s", "abc\0def");
	CHECK("%s", "\0");
	CHECK("%s", "caf\xc3\xa9 \xe2\x82\xac");
	CHECK("%s", "\x01\x02\x03\x7f\x80\xfe\xff");
}

static void	test_string_combo(void)
{
	char	stack_buf[] = "stack array string";
	char	*heap;

	heap = malloc(12);
	strcpy(heap, "heap string");
	CHECK("%s", stack_buf);
	CHECK("%s", (const char *)"pointer to literal");
	CHECK("%s", heap);
	CHECK("%s%s", "", "");
	CHECK("%s%s", "a", "");
	CHECK("%s%s", "", "b");
	CHECK("%s%s%s", "one", " ", "two");
	CHECK("[%s]", "bracketed");
	CHECK("[%s]", "");
	CHECK("%s %s %s %s %s", "a", "b", "c", "d", "e");
	CHECK("%s%c%s", "ab", 'X', "cd");
	CHECK("%s", g_long_1k);
	CHECK("%s", g_long_10k);
	CHECK("%s", g_long_100k);
	CHECK("%s|%s", g_long_1k, g_long_1k);
	CHECK("prefix %s suffix", g_long_10k);
	free(heap);
}

static void	test_string(void)
{
	test_string_plain();
	test_string_combo();
}

/* ************************************************************************** */
/*  D. Pointer tests (%p)   (NULL pointer is in section L: glibc only)        */
/* ************************************************************************** */

static void	test_pointer_vars(void)
{
	int		n;
	char	c;
	int		arr[10];
	t_point	pt;
	long	l;
	double	d;

	n = 0;
	c = 'x';
	memset(arr, 0, sizeof(arr));
	memset(&pt, 0, sizeof(pt));
	l = 0;
	d = 0.0;
	CHECK("%p", (void *)&n);
	CHECK("%p", (void *)&c);
	CHECK("%p", (void *)arr);
	CHECK("%p", (void *)&arr[0]);
	CHECK("%p", (void *)&arr[5]);
	CHECK("%p", (void *)&arr[9]);
	CHECK("%p", (void *)&pt);
	CHECK("%p", (void *)&pt.y);
	CHECK("%p", (void *)&pt.tag);
	CHECK("%p", (void *)&l);
	CHECK("%p", (void *)&d);
	CHECK("%p", (void *)&g_global);
	CHECK("%p", (void *)g_long_1k);
	CHECK("%p", (void *)"string literal");
}

static void	test_pointer_values(void)
{
	CHECK("%p", PTR(0x1));
	CHECK("%p", PTR(0xf));
	CHECK("%p", PTR(0x10));
	CHECK("%p", PTR(0xff));
	CHECK("%p", PTR(0x100));
	CHECK("%p", PTR(0xabc));
	CHECK("%p", PTR(0xABCDEF));
	CHECK("%p", PTR(0x7fff));
	CHECK("%p", PTR(0x12345678));
	CHECK("%p", PTR(0x7fffffff));
	CHECK("%p", PTR(0xdeadbeef));
	CHECK("%p", PTR(0xffffffff));
#if UINTPTR_MAX > 0xffffffffUL
	CHECK("%p", PTR(0x100000000ULL));
	CHECK("%p", PTR(0x123456789abcdefULL));
	CHECK("%p", PTR(0x7ffd12345678ULL));
	CHECK("%p", PTR(0x1000000000000000ULL));
	CHECK("%p", PTR(0x7fffffffffffffffULL));
	CHECK("%p", PTR(0x8000000000000000ULL));
	CHECK("%p", PTR(0xfedcba9876543210ULL));
	CHECK("%p", PTR(UINTPTR_MAX));
#endif
}

static void	test_pointer_combo(void)
{
	int		n;
	char	c;
	char	*heap;

	n = 1;
	c = 'c';
	heap = malloc(32);
	CHECK("%p", (void *)heap);
	CHECK("%p", (void *)(heap + 5));
	CHECK("%p", (void *)&heap);
	CHECK("%p %p", (void *)&n, (void *)&c);
	CHECK("%p%p%p", (void *)&n, (void *)&c, (void *)heap);
	CHECK("addr=%p;", (void *)&n);
	CHECK("%p %s %d", (void *)&n, "str", n);
	CHECK("%p|%p|%p|%p", PTR(1), PTR(0xff), PTR(0xabcdef), PTR(0xdeadbeef));
	CHECK("%c%p%c", 'a', (void *)&n, 'b');
	CHECK("%p %p %p %p %p %p %p %p", PTR(1), PTR(2), PTR(3), PTR(4),
		PTR(5), PTR(6), PTR(7), PTR(0xabc));
	free(heap);
}

static void	test_pointer(void)
{
	test_pointer_vars();
	test_pointer_values();
	test_pointer_combo();
}

/* ************************************************************************** */
/*  E. Signed integers (%d %i)                                                */
/* ************************************************************************** */

static void	test_signed_d(void)
{
	CHECK("%d", 0);
	CHECK("%d", 1);
	CHECK("%d", -1);
	CHECK("%d", 2);
	CHECK("%d", -2);
	CHECK("%d", 9);
	CHECK("%d", -9);
	CHECK("%d", 10);
	CHECK("%d", -10);
	CHECK("%d", 11);
	CHECK("%d", 19);
	CHECK("%d", 20);
	CHECK("%d", 42);
	CHECK("%d", -42);
	CHECK("%d", 99);
	CHECK("%d", -99);
	CHECK("%d", 100);
	CHECK("%d", -100);
	CHECK("%d", 101);
	CHECK("%d", 110);
	CHECK("%d", 123);
	CHECK("%d", -123);
	CHECK("%d", 999);
	CHECK("%d", 1000);
	CHECK("%d", -1000);
	CHECK("%d", 1001);
	CHECK("%d", 1010);
	CHECK("%d", 1100);
	CHECK("%d", 9999);
	CHECK("%d", 10000);
	CHECK("%d", 12345);
	CHECK("%d", -12345);
	CHECK("%d", 99999);
	CHECK("%d", 100000);
	CHECK("%d", 123456);
	CHECK("%d", 999999);
	CHECK("%d", 1000000);
	CHECK("%d", 1234567);
	CHECK("%d", 10000000);
	CHECK("%d", 12345678);
}

static void	test_signed_d_big(void)
{
	CHECK("%d", 100000000);
	CHECK("%d", 123456789);
	CHECK("%d", -123456789);
	CHECK("%d", 999999999);
	CHECK("%d", 1000000000);
	CHECK("%d", -1000000000);
	CHECK("%d", 1000000001);
	CHECK("%d", 1234567890);
	CHECK("%d", 1999999999);
	CHECK("%d", 2000000000);
	CHECK("%d", -2000000000);
	CHECK("%d", 2147483646);
	CHECK("%d", INT_MAX);
	CHECK("%d", INT_MAX - 1);
	CHECK("%d", INT_MIN);
	CHECK("%d", INT_MIN + 1);
	CHECK("%d", INT_MIN + 2);
	CHECK("%d", 100100100);
	CHECK("%d", -100100100);
	CHECK("%d", 1000000009);
	CHECK("%d", 909090909);
	CHECK("%d", 2020202020);
	CHECK("%d", (short)SHRT_MIN);
	CHECK("%d", SHRT_MAX);
	CHECK("%d", (signed char)-128);
	CHECK("%d", (unsigned char)200);
	CHECK("%d", 'A');
}

static void	test_signed_i_and_combo(void)
{
	CHECK("%i", 0);
	CHECK("%i", 7);
	CHECK("%i", -7);
	CHECK("%i", 42);
	CHECK("%i", -42);
	CHECK("%i", 100);
	CHECK("%i", -100);
	CHECK("%i", 1000000000);
	CHECK("%i", INT_MAX);
	CHECK("%i", INT_MIN);
	CHECK("%d %d", INT_MIN, INT_MAX);
	CHECK("%d%d%d", 0, 0, 0);
	CHECK("%d%d", -1, -1);
	CHECK("%d,%d,%d", 1, -2, 3);
	CHECK("%i %d", INT_MIN, INT_MIN);
	CHECK("%d%%", 100);
	CHECK("%d\n", INT_MIN);
	CHECK("%i%i%i%i", 1, 2, 3, 4);
	CHECK("[%d][%i]", -5, -6);
}

static void	test_signed(void)
{
	test_signed_d();
	test_signed_d_big();
	test_signed_i_and_combo();
}

/* ************************************************************************** */
/*  F. Unsigned integers (%u)                                                 */
/* ************************************************************************** */

static void	test_unsigned(void)
{
	CHECK("%u", 0u);
	CHECK("%u", 1u);
	CHECK("%u", 9u);
	CHECK("%u", 10u);
	CHECK("%u", 42u);
	CHECK("%u", 99u);
	CHECK("%u", 100u);
	CHECK("%u", 255u);
	CHECK("%u", 256u);
	CHECK("%u", 65535u);
	CHECK("%u", 65536u);
	CHECK("%u", 1000000u);
	CHECK("%u", 1000000000u);
	CHECK("%u", 2147483647u);
	CHECK("%u", 2147483648u);
	CHECK("%u", 2147483649u);
	CHECK("%u", 3000000000u);
	CHECK("%u", 4000000000u);
	CHECK("%u", 4294967294u);
	CHECK("%u", UINT_MAX);
	CHECK("%u", UINT_MAX - 1);
	CHECK("%u", UINT_MAX - 9);
	CHECK("%u", UINT_MAX - 100);
	CHECK("%u", (unsigned int)-1);
	CHECK("%u", (unsigned int)-2);
	CHECK("%u", (unsigned int)-42);
	CHECK("%u", (unsigned int)-1000);
	CHECK("%u", (unsigned int)INT_MIN);
	CHECK("%u", (unsigned int)(INT_MIN + 1));
	CHECK("%u", (unsigned int)INT_MAX);
	CHECK("%u %u", 0u, UINT_MAX);
	CHECK("%u%u%u", 0u, 0u, 0u);
	CHECK("%u,%u", UINT_MAX, UINT_MAX);
	CHECK("[%u]", 123u);
	CHECK("%u\n", UINT_MAX);
}

/* ************************************************************************** */
/*  G. Hexadecimal (%x %X)                                                    */
/* ************************************************************************** */

static void	test_hex_small(void)
{
	CHECK_HEX(0u);
	CHECK_HEX(1u);
	CHECK_HEX(2u);
	CHECK_HEX(9u);
	CHECK_HEX(10u);
	CHECK_HEX(11u);
	CHECK_HEX(12u);
	CHECK_HEX(13u);
	CHECK_HEX(14u);
	CHECK_HEX(15u);
	CHECK_HEX(16u);
	CHECK_HEX(17u);
	CHECK_HEX(31u);
	CHECK_HEX(32u);
	CHECK_HEX(100u);
	CHECK_HEX(127u);
	CHECK_HEX(128u);
	CHECK_HEX(255u);
	CHECK_HEX(256u);
	CHECK_HEX(4095u);
	CHECK_HEX(4096u);
	CHECK_HEX(65535u);
	CHECK_HEX(65536u);
}

static void	test_hex_digits(void)
{
	CHECK_HEX(0x1u);
	CHECK_HEX(0x99u);
	CHECK_HEX(0x9999u);
	CHECK_HEX(0x1234567u);
	CHECK_HEX(0x12345678u);
	CHECK_HEX(0x10203040u);
	CHECK_HEX(0xau);
	CHECK_HEX(0xabu);
	CHECK_HEX(0xabcu);
	CHECK_HEX(0xabcdu);
	CHECK_HEX(0xabcdefu);
	CHECK_HEX(0xdeadbeefu);
	CHECK_HEX(0xcafebabeu);
	CHECK_HEX(0xfeedfaceu);
	CHECK_HEX(0x1a2b3c4du);
	CHECK_HEX(0x9abcdef0u);
	CHECK_HEX(0x00ff00ffu);
	CHECK_HEX(0xf0f0f0f0u);
	CHECK_HEX(0x0f0f0f0fu);
	CHECK_HEX(0x10000000u);
	CHECK_HEX(0x01000000u);
	CHECK_HEX(0x00100000u);
	CHECK_HEX(0x00001000u);
	CHECK_HEX(0x00000100u);
	CHECK_HEX(0x00000010u);
	CHECK_HEX(0x0fffffffu);
	CHECK_HEX(0x7fffffffu);
	CHECK_HEX(0x80000000u);
	CHECK_HEX(0xfffffff0u);
	CHECK_HEX(0xfffffffeu);
	CHECK_HEX(0xffffffffu);
}

static void	test_hex_types(void)
{
	CHECK("%x", UINT_MAX);
	CHECK("%X", UINT_MAX);
	CHECK("%x", UINT_MAX - 1);
	CHECK("%x", (unsigned int)-1);
	CHECK("%X", (unsigned int)-1);
	CHECK("%x", (unsigned int)-42);
	CHECK("%X", (unsigned int)-42);
	CHECK("%x", (unsigned int)INT_MIN);
	CHECK("%X", (unsigned int)INT_MIN);
	CHECK("%x", (unsigned int)INT_MAX);
	CHECK("%X", (unsigned int)INT_MAX);
	CHECK("%x %X", 0xabcdefu, 0xabcdefu);
	CHECK("%x%X%x%X", 10u, 11u, 12u, 13u);
	CHECK("0x%x", 255u);
	CHECK("0x%X", 255u);
	CHECK("%x-%x-%x", 1u, 0xabu, 0xabcu);
	CHECK("%x %x %x", 0u, 0u, 0u);
	CHECK("%X%X%X", 0u, 0u, 0u);
	CHECK("[%x][%X]", 0xbeefu, 0xbeefu);
	CHECK("%x\n", 0xdeadu);
}

static void	test_hex(void)
{
	test_hex_small();
	test_hex_digits();
	test_hex_types();
}

/* ************************************************************************** */
/*  H. Percent (%%)                                                           */
/* ************************************************************************** */

static void	test_percent(void)
{
	CHECK("%%");
	CHECK("%%%%");
	CHECK("%%%%%%");
	CHECK("%%%%%%%%");
	CHECK("%%%%%%%%%%%%%%%%");
	CHECK("%%a");
	CHECK("a%%");
	CHECK("a%%b");
	CHECK("%%a%%b%%");
	CHECK("100%%");
	CHECK("%% %% %%");
	CHECK("%%c");
	CHECK("%%s");
	CHECK("%%d");
	CHECK("%%i");
	CHECK("%%u");
	CHECK("%%x");
	CHECK("%%X");
	CHECK("%%p");
	CHECK("%%%%d");
	CHECK("%%\n");
	CHECK("%%%c", 'x');
	CHECK("%c%%", 'x');
	CHECK("%c%%%c", 'a', 'b');
	CHECK("%%%d%%", 42);
	CHECK("%%%s%%", "str");
	CHECK("%%%x%%%X%%", 255u, 255u);
	CHECK("%%%u%%%i%%", 7u, -7);
	CHECK("%d%%%d%%%d", 1, 2, 3);
	CHECK("%%%p%%", PTR(0xabc));
	CHECK("%s%%", "50");
}

/* ************************************************************************** */
/*  I. Mixed tests                                                            */
/* ************************************************************************** */

static void	test_mixed_all(void)
{
	int	n;

	n = 7;
	CHECK("%c|%s|%p|%d|%i|%u|%x|%X|%%", 'A', "str", (void *)&n, -42, 42, 42u,
		255u, 255u);
	CHECK("%X|%x|%u|%i|%d|%p|%s|%c", 255u, 255u, 42u, 42, -42, (void *)&n,
		"str", 'A');
	CHECK("%c%s%p%d%i%u%x%X%%", 'A', "str", (void *)&n, -42, 42, 42u, 255u,
		255u);
	CHECK("Char: %c, String: %s, Pointer: %p, Decimal: %d, Integer: %i, "
		"Unsigned: %u, Hex: %x, HEX: %X, Percent: %%\n", 'z', "hello",
		PTR(0xabc), INT_MIN, INT_MAX, UINT_MAX, UINT_MAX, UINT_MAX);
	CHECK("[%c][%s][%p][%d][%i][%u][%x][%X][%%]\n", 'a', "b", PTR(0xc), 4, 5,
		6u, 7u, 8u);
	CHECK("%d %i %u %x %X", INT_MIN, INT_MAX, UINT_MAX, UINT_MAX, UINT_MAX);
	CHECK("%d %i %u %x %X", INT_MAX, INT_MIN, 0u, 0u, 0u);
	CHECK("%u %x %X %d", (unsigned int)-1, (unsigned int)-1,
		(unsigned int)-1, -1);
	CHECK("%%d = %d, %%s = %s, %%c = %c", 42, "str", 'x');
	CHECK("%d%s%c%u%x%X%i", 1, "two", '3', 4u, 5u, 6u, 7);
	CHECK("%c%%%c%%%c", 'a', 'b', 'c');
	CHECK("%x%s%X%s%u", 0xabu, "-", 0xabu, "-", 0xabu);
}

static void	test_mixed_order(void)
{
	int	n;

	n = 3;
	CHECK("%s %s %s %s", "", "a", "", "b");
	CHECK("%c%c%c%s%s%s%d%d%d", 'a', 'b', 'c', "d", "e", "f", 1, 2, 3);
	CHECK("%s%d%s%d%s%d%s%d", "a", 1, "b", 2, "c", 3, "d", 4);
	CHECK("%u%d%u%d%u%d", 1u, -1, 2u, -2, 3u, -3);
	CHECK("%x%X%d%i%u", 0xau, 0xbu, 12, 13, 14u);
	CHECK("%p%s%p%s", (void *)&n, " and ", (void *)&n, "!");
	CHECK("%c%c%c%c%c%c", 'p', 'r', 'i', 'n', 't', 'f');
	CHECK("%s%c%s%c%s", "a", '\0', "b", '\0', "c");
	CHECK("%c%d%c%s%c%x%c", '\0', 1, '\0', "mid", '\0', 255u, '\0');
	CHECK("%d %d %d %d %d %d %d %d %d %d %d %d", 1, 2, 3, 4, 5, 6, 7, 8, 9,
		10, 11, 12);
	CHECK("%c %s %d %i %u %x %X %c %s %d %i %u %x %X %%", 'a', "b", 1, 2, 3u,
		4u, 5u, 'c', "d", 6, 7, 8u, 9u, 10u);
	CHECK("%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c", 0, 'a', 1, 'b', 2, 'c',
		3, 'd', 4, 'e', 5, 'f', 6, 'g', 7, 'h', 8, 'i', 9, 'j');
	CHECK("%s%s%s%s%s%s%s%s%s%s", "0", "1", "2", "3", "4", "5", "6", "7",
		"8", "9");
}

static void	test_mixed(void)
{
	test_mixed_all();
	test_mixed_order();
}

/* ************************************************************************** */
/*  J. Return value focus (output right but count wrong is a classic bug)     */
/* ************************************************************************** */

static void	test_return_values(void)
{
	int	n;

	n = 1;
	CHECK("");
	CHECK("%s", "");
	CHECK("%c", '\0');
	CHECK("%c%c%c%c%c", '\0', '\0', '\0', '\0', '\0');
	CHECK("a%cb%cc", '\0', '\0');
	CHECK("%%%%%%");
	CHECK("%d", 0);
	CHECK("%d", -1);
	CHECK("%d", INT_MIN);
	CHECK("%d", INT_MAX);
	CHECK("%i", INT_MIN);
	CHECK("%u", 0u);
	CHECK("%u", UINT_MAX);
	CHECK("%x", 0u);
	CHECK("%x", UINT_MAX);
	CHECK("%X", 0xabcdu);
	CHECK("%p", PTR(1));
	CHECK("%p", (void *)&n);
	CHECK("%s", g_long_10k);
	CHECK("%s", g_long_100k);
	CHECK("%d%d%d", 100, 200, 300);
	CHECK("%s%c%s", "ab", '\0', "cd");
	CHECK("%c%s%d%i%u%x%X%%", 'a', "bc", -1, 22, 333u, 0xabcu, 0xdefu);
	CHECK("no conversions, newline\n");
	CHECK("%s\n%s\n%s\n", "l1", "l2", "l3");
	CHECK("%d\n", INT_MIN);
	CHECK("%c\n", '\0');
}

/* ************************************************************************** */
/*  K. Generated sweeps (hundreds of cheap tests, they find hidden bugs)      */
/* ************************************************************************** */

static unsigned int	next_rand(unsigned int *state)
{
	unsigned int	x;

	x = *state;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	*state = x;
	return (x);
}

static void	sweep_chars(void)
{
	int	i;

	i = 0;
	while (i < 256)
	{
		note("byte value %d", i);
		CHECK("%c", i);
		i++;
	}
}

static void	sweep_pow10(void)
{
	long long	p;
	long long	d;
	int			v;

	p = 1;
	while (p <= 1000000000LL)
	{
		d = -1;
		while (d <= 1)
		{
			v = (int)(p + d);
			note("value = %d", v);
			CHECK("%d", v);
			note("value = %d", -v);
			CHECK("%i", -v);
			d++;
		}
		p *= 10;
	}
}

static void	check_unsigned(unsigned int v)
{
	note("value = %u", v);
	CHECK("%u", v);
	note("value = %u", v);
	CHECK("%x", v);
	note("value = %u", v);
	CHECK("%X", v);
}

static void	sweep_pow2(void)
{
	unsigned int	k;
	unsigned int	p;

	k = 0;
	while (k < 32)
	{
		p = 1u << k;
		check_unsigned(p - 1);
		check_unsigned(p);
		check_unsigned(p + 1);
		k++;
	}
}

static void	sweep_hex_bytes(void)
{
	unsigned int	v;

	v = 0;
	while (v < 256)
	{
		note("value = %u", v);
		CHECK("%x", v);
		note("value = %u", v);
		CHECK("%X", v);
		v++;
	}
}

static void	sweep_random_numbers(void)
{
	unsigned int	seed;
	unsigned int	u;
	unsigned int	v;
	int				i;

	seed = 2463534242u;
	i = 0;
	while (i < 200)
	{
		u = next_rand(&seed);
		v = u >> (i % 32);
		note("random #%d, u = %u, v = %u", i, u, v);
		CHECK("%d|%i|%u|%x|%X", (int)v, (int)u, v, u, v);
		i++;
	}
}

static void	sweep_random_strings(void)
{
	unsigned int	seed;
	char			buf[256];
	size_t			len;
	size_t			j;
	int				i;

	seed = 88172645u;
	i = 0;
	while (i < 60)
	{
		len = next_rand(&seed) % 200;
		j = 0;
		while (j < len)
		{
			buf[j] = (char)(32 + next_rand(&seed) % 95);
			j++;
		}
		buf[len] = '\0';
		note("random string #%d, length %zu", i, len);
		CHECK("[%s]%c", buf, 'x');
		i++;
	}
}

static void	sweep_string_lengths(void)
{
	static const size_t	lens[] = {1, 2, 15, 16, 17, 31, 32, 33, 63, 64, 65,
		127, 128, 129, 255, 256, 257, 511, 512, 513, 1023, 1024, 1025, 2047,
		2048, 2049, 4095, 4096, 4097, 8191, 8192, 8193, 65535, 65536, 65537};
	size_t				i;
	char				saved;

	i = 0;
	while (i < sizeof(lens) / sizeof(lens[0]))
	{
		saved = g_big[lens[i]];
		g_big[lens[i]] = '\0';
		note("%%s argument is %zu bytes long", lens[i]);
		CHECK("%s", g_big);
		note("%%s argument is %zu bytes long", lens[i]);
		CHECK("%c%s%c", 'A', g_big, 'Z');
		g_big[lens[i]] = saved;
		i++;
	}
}

static void	build_fmt(size_t before, const char *conv, size_t after)
{
	memset(g_fmt, 'f', before);
	strcpy(g_fmt + before, conv);
	memset(g_fmt + before + strlen(conv), 'f', after);
	g_fmt[before + strlen(conv) + after] = '\0';
}

static void	sweep_long_format(void)
{
	static const size_t	lens[] = {100, 255, 256, 1023, 1024, 4094, 4095, 4096,
		4097, 8191, 8192, 8193};
	size_t				i;

	i = 0;
	while (i < sizeof(lens) / sizeof(lens[0]))
	{
		build_fmt(lens[i], "%d", 0);
		note("long format: %zu literal bytes, then %%d", lens[i]);
		CHECK(g_fmt, 42);
		build_fmt(0, "%s", lens[i]);
		note("long format: %%s, then %zu literal bytes", lens[i]);
		CHECK(g_fmt, "mid");
		build_fmt(lens[i], "%x", lens[i]);
		note("long format: %zu bytes, %%x, %zu bytes", lens[i], lens[i]);
		CHECK(g_fmt, 0xabcdefu);
		i++;
	}
}

static void	test_sweeps(void)
{
	sweep_chars();
	sweep_pow10();
	sweep_pow2();
	sweep_hex_bytes();
	sweep_random_numbers();
	sweep_random_strings();
	sweep_string_lengths();
	sweep_long_format();
}

/* ************************************************************************** */
/*  L. Linux / glibc specific: NULL string and NULL pointer                   */
/*     glibc prints "(null)" for %s and "(nil)" for %p. macOS prints "0x0"    */
/*     for %p, musl prints "0x0": the C standard leaves this undefined.       */
/* ************************************************************************** */

static void	test_glibc_null(void)
{
#ifdef __GLIBC__
	CHECK_GLIBC("%s", (char *)NULL);
	CHECK_GLIBC("[%s]", (char *)NULL);
	CHECK_GLIBC("%s%s", (char *)NULL, (char *)NULL);
	CHECK_GLIBC("%s %s", (char *)NULL, "x");
	CHECK_GLIBC("x%sx", (char *)NULL);
	CHECK_GLIBC("%s\n", (char *)NULL);
	CHECK_GLIBC("%p", (void *)NULL);
	CHECK_GLIBC("[%p]", (void *)NULL);
	CHECK_GLIBC("%p %p", (void *)NULL, (void *)NULL);
	CHECK_GLIBC("%p%s", (void *)NULL, (char *)NULL);
	CHECK_GLIBC("%c%p%c", 'a', (void *)NULL, 'b');
	CHECK_GLIBC("%s|%p|%d", (char *)NULL, (void *)NULL, 0);
	CHECK_GLIBC("%d %s %p %x", 1, (char *)NULL, (void *)NULL, 255u);
	CHECK_GLIBC("%p", PTR(0));
#else
	tester_skip("not glibc: NULL %s / %p output is implementation-defined");
#endif
}

/* ************************************************************************** */
/*  M. Several calls in ONE process (static / global state bugs)              */
/*     pf is printf or ft_printf. The sum of the return values must equal     */
/*     the number of bytes written, so a "cumulative" counter is caught.      */
/* ************************************************************************** */

static int	seq_basic(t_pf pf)
{
	int	sum;

	sum = pf("%d\n", 12345);
	sum += pf("%s\n", "x");
	sum += pf("%c", 'a');
	sum += pf("%u\n", 0u);
	sum += pf("%x\n", 255u);
	sum += pf("%%\n");
	return (sum);
}

static int	seq_null_chars(t_pf pf)
{
	int	sum;
	int	i;

	sum = 0;
	i = 0;
	while (i < 10)
	{
		sum += pf("%c", '\0');
		i++;
	}
	sum += pf("%c%c", '\0', 'a');
	return (sum);
}

static int	seq_big_then_small(t_pf pf)
{
	int	sum;

	sum = pf("%s", g_long_10k);
	sum += pf("%s", "x");
	sum += pf("%s\n", "");
	sum += pf("%d", 7);
	return (sum);
}

static int	seq_many(t_pf pf)
{
	int	sum;
	int	i;

	sum = 0;
	i = 0;
	while (i < 100)
	{
		sum += pf("%d,", i);
		sum += pf("%x ", (unsigned int)i * 977u);
		i++;
	}
	return (sum);
}

static int	seq_extremes(t_pf pf)
{
	int	sum;

	sum = pf("%d", INT_MIN);
	sum += pf("%d", 5);
	sum += pf("%u", UINT_MAX);
	sum += pf("%x", 0u);
	sum += pf("%X", UINT_MAX);
	sum += pf("%p", PTR(0x1));
	sum += pf("%s", "");
	return (sum);
}

static void	test_sequences(void)
{
	CHECK_SEQ(seq_basic);
	CHECK_SEQ(seq_null_chars);
	CHECK_SEQ(seq_big_then_small);
	CHECK_SEQ(seq_many);
	CHECK_SEQ(seq_extremes);
}

/* ************************************************************************** */
/*  runner                                                                    */
/* ************************************************************************** */

static void	fill_pattern(char *dst, size_t len)
{
	static const char	pat[] = "The quick brown fox 0123456789 %s %d %% jumps\n";
	size_t				i;

	i = 0;
	while (i < len)
	{
		dst[i] = pat[i % (sizeof(pat) - 1)];
		i++;
	}
	dst[len] = '\0';
}

static const t_section	g_sections[] = {
{'A', "Basic tests", test_basic},
{'B', "Character tests (%c)", test_char},
{'C', "String tests (%s)", test_string},
{'D', "Pointer tests (%p)", test_pointer},
{'E', "Signed integer tests (%d %i)", test_signed},
{'F', "Unsigned integer tests (%u)", test_unsigned},
{'G', "Hexadecimal tests (%x %X)", test_hex},
{'H', "Percent tests (%%)", test_percent},
{'I', "Mixed tests", test_mixed},
{'J', "Return value focus", test_return_values},
{'K', "Generated sweeps / stress", test_sweeps},
{'L', "NULL %s / NULL %p (Linux/glibc ONLY)", test_glibc_null},
{'M', "Several calls in one process", test_sequences},
{0, NULL, NULL}
};

static int	is_selected(const t_opts *o, char id)
{
	int	i;

	if (o->nsel == 0)
		return (1);
	i = 0;
	while (i < o->nsel)
	{
		if (o->sel[i] == id)
			return (1);
		i++;
	}
	return (0);
}

static void	usage(const char *prog)
{
	printf("usage: %s [-v] [-a] [SECTION...]\n", prog);
	printf("  -v   print one line per test (PASS and FAIL)\n");
	printf("  -a   detail ALL failures in the final report\n");
	printf("  A-M  run only these sections, e.g.  %s -v B K\n", prog);
	exit(0);
}

static void	parse_args(int ac, char **av, t_opts *o)
{
	int	i;

	memset(o, 0, sizeof(*o));
	i = 1;
	while (i < ac)
	{
		if (strcmp(av[i], "-v") == 0)
			o->verbose = 1;
		else if (strcmp(av[i], "-a") == 0)
			o->all = 1;
		else if (strcmp(av[i], "-h") == 0 || o->nsel >= 31)
			usage(av[0]);
		else
		{
			o->sel[o->nsel] = (char)toupper((unsigned char)av[i][0]);
			o->nsel++;
		}
		i++;
	}
}

int	main(int ac, char **av)
{
	t_opts	o;
	int		i;

	parse_args(ac, av, &o);
	fill_pattern(g_long_1k, sizeof(g_long_1k) - 1);
	fill_pattern(g_long_10k, sizeof(g_long_10k) - 1);
	fill_pattern(g_long_100k, sizeof(g_long_100k) - 1);
	fill_pattern(g_big, sizeof(g_big) - 1);
	tester_init(o.verbose, o.all);
	i = 0;
	while (g_sections[i].run != NULL)
	{
		if (is_selected(&o, g_sections[i].id))
		{
			section_begin(g_sections[i].id, g_sections[i].title);
			g_sections[i].run();
			section_end();
		}
		i++;
	}
	return (tester_report());
}