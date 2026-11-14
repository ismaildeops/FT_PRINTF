/* ************************************************************************** */
/*                                                                            */
/*   tester.h - tiny harness used by main.c to compare printf / ft_printf     */
/*                                                                            */
/* ************************************************************************** */

#ifndef TESTER_H
# define TESTER_H

# include <stddef.h>
# include <stdio.h>
# include <sys/types.h>
#include "ft_printf.h"
# define TIMEOUT_SEC 2
# define MAX_FILE_SIZE 33554432
# define MAX_FAILS 1024
# define MAX_SHOW 120

/* Why a test failed (bit flags, a test can have several reasons) */
# define R_OUT 1
# define R_RET 2
# define R_COUNT 4
# define R_CRASH 8
# define R_REF 16

/*
** Everything we learn about ONE call (printf or ft_printf):
** the bytes written on fd 1, the return value and how the process ended.
*/
typedef struct s_cap
{
	char	*buf;
	size_t	len;
	int		ret;
	int		has_ret;
	int		status;
	int		out_fd;
	int		rd_fd;
	int		wr_fd;
	pid_t	pid;
	FILE	*tmp;
}	t_cap;

/* The real printf, called through a pointer so the compiler cannot
** "optimize" it (printf("a\n") -> puts) or warn about the test formats. */
extern int	(*const g_ref)(const char *, ...);

int		cap_start(t_cap *c);
void	cap_finish_child(t_cap *c, int ret);
void	cap_collect(t_cap *c);

void	tester_init(int verbose, int detail_all);
void	section_begin(char id, const char *title);
void	section_end(void);
void	tester_skip(const char *why);
void	note(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
void	judge(const char *expr, t_cap *ref, t_cap *act);
int		tester_report(void);

/*
** RUN: execute "call" in a child process whose stdout is a temporary file.
**   - the child runs the call, flushes stdio, sends the return value through
**     a pipe and leaves with _exit()
**   - the parent waits, then reads back the bytes and the return value
** A crash or an infinite loop in ft_printf only kills the child.
*/
# define RUN(c, call) \
	do { \
		if (cap_start(c) == 0) \
			cap_finish_child(c, call); \
		cap_collect(c); \
	} while (0)

/*
** CHECK("format", args...): run printf and ft_printf with the SAME
** arguments, then compare bytes, return values and "return == bytes".
*/
# define CHECK(...) \
	do { \
		t_cap	ref_; \
		t_cap	act_; \
		RUN(&ref_, g_ref(__VA_ARGS__)); \
		RUN(&act_, ft_printf(__VA_ARGS__)); \
		judge("ft_printf(" #__VA_ARGS__ ")", &ref_, &act_); \
	} while (0)

/*
** CHECK_SEQ(fn): fn(pf) calls pf() several times in ONE process and
** returns the sum of the return values (detects state kept between calls).
*/
# define CHECK_SEQ(seq) \
	do { \
		t_cap	ref_; \
		t_cap	act_; \
		RUN(&ref_, seq(g_ref)); \
		RUN(&act_, seq(ft_printf)); \
		judge("sequence " #seq "(pf)", &ref_, &act_); \
	} while (0)

#endif