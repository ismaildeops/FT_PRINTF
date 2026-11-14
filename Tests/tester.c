/* ************************************************************************** */
/*                                                                            */
/*   tester.c - capture, compare and report                                   */
/*                                                                            */
/* ************************************************************************** */

#define _GNU_SOURCE
#include <errno.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "tester.h"

#define GRN "\033[1;32m"
#define RED "\033[1;31m"
#define YEL "\033[1;33m"
#define CYN "\033[1;36m"
#define RST "\033[0m"

int	(*const g_ref)(const char *, ...) = printf;

typedef struct s_fail
{
	int		id;
	char	section;
	int		reason;
	char	expr[320];
	char	note[128];
	t_cap	ref;
	t_cap	act;
}	t_fail;

typedef struct s_state
{
	int			verbose;
	int			detail_limit;
	int			color;
	int			total;
	int			passed;
	int			failed;
	int			skipped;
	int			stored;
	int			sec_start_total;
	int			sec_start_failed;
	int			sec_skipped;
	const char	*sec_skip_why;
	char		sec_id;
	const char	*sec_title;
	char		note[128];
	t_fail		fails[MAX_FAILS];
}	t_state;

static t_state	g_st;

/* ------------------------------------------------------------------------ */
/*  small helpers                                                           */
/* ------------------------------------------------------------------------ */

static const char	*col(const char *code)
{
	if (g_st.color)
		return (code);
	return ("");
}

static void	die(const char *msg)
{
	perror(msg);
	exit(2);
}

void	note(const char *fmt, ...)
{
	va_list	ap;

	va_start(ap, fmt);
	vsnprintf(g_st.note, sizeof(g_st.note), fmt, ap);
	va_end(ap);
}

void	tester_init(int verbose, int detail_all)
{
	memset(&g_st, 0, sizeof(g_st));
	g_st.verbose = verbose;
	g_st.detail_limit = 30;
	if (detail_all)
		g_st.detail_limit = MAX_FAILS;
	g_st.color = (isatty(STDOUT_FILENO) && getenv("NO_COLOR") == NULL);
}

/* ------------------------------------------------------------------------ */
/*  capture: one forked child per call                                      */
/* ------------------------------------------------------------------------ */

static void	child_limits(void)
{
	struct rlimit	rl;

	rl.rlim_cur = MAX_FILE_SIZE;
	rl.rlim_max = MAX_FILE_SIZE;
	setrlimit(RLIMIT_FSIZE, &rl);
	rl.rlim_cur = 0;
	rl.rlim_max = 0;
	setrlimit(RLIMIT_CORE, &rl);
	alarm(TIMEOUT_SEC);
}

/*
** Returns 0 in the child (stdout now points to a temporary file) and
** 1 in the parent. The parent's stdio buffer MUST be flushed before the
** fork, otherwise the child would copy it into the captured file.
*/
int	cap_start(t_cap *c)
{
	int	fds[2];

	memset(c, 0, sizeof(*c));
	fflush(stdout);
	c->tmp = tmpfile();
	if (c->tmp == NULL || pipe(fds) < 0)
		die("tmpfile/pipe");
	c->out_fd = fileno(c->tmp);
	c->rd_fd = fds[0];
	c->wr_fd = fds[1];
	c->pid = fork();
	if (c->pid < 0)
		die("fork");
	if (c->pid > 0)
	{
		close(c->wr_fd);
		return (1);
	}
	close(c->rd_fd);
	dup2(c->out_fd, STDOUT_FILENO);
	child_limits();
	return (0);
}

/* Child only: flush printf's buffer, report the return value, leave. */
void	cap_finish_child(t_cap *c, int ret)
{
	fflush(stdout);
	if (write(c->wr_fd, &ret, sizeof(ret)) < 0)
		_exit(3);
	_exit(0);
}

static void	read_output(t_cap *c)
{
	off_t	size;
	ssize_t	n;
	size_t	got;

	size = lseek(c->out_fd, 0, SEEK_END);
	if (size < 0)
		size = 0;
	c->buf = malloc((size_t)size + 1);
	if (c->buf == NULL)
		die("malloc");
	lseek(c->out_fd, 0, SEEK_SET);
	got = 0;
	while (got < (size_t)size)
	{
		n = read(c->out_fd, c->buf + got, (size_t)size - got);
		if (n <= 0)
			break ;
		got += (size_t)n;
	}
	c->buf[got] = '\0';
	c->len = got;
}

void	cap_collect(t_cap *c)
{
	ssize_t	n;

	while (waitpid(c->pid, &c->status, 0) < 0 && errno == EINTR)
		;
	n = read(c->rd_fd, &c->ret, sizeof(c->ret));
	c->has_ret = (n == (ssize_t)sizeof(c->ret));
	close(c->rd_fd);
	read_output(c);
	fclose(c->tmp);
}

static void	cap_free(t_cap *c)
{
	free(c->buf);
	c->buf = NULL;
}

static int	cap_ok(const t_cap *c)
{
	if (!c->has_ret)
		return (0);
	return (WIFEXITED(c->status) && WEXITSTATUS(c->status) == 0);
}

/* ------------------------------------------------------------------------ */
/*  printing helpers                                                        */
/* ------------------------------------------------------------------------ */

static void	print_char(unsigned char c)
{
	if (c == '\n')
		printf("\\n");
	else if (c == '\t')
		printf("\\t");
	else if (c == '\r')
		printf("\\r");
	else if (c == '\\')
		printf("\\\\");
	else if (c == '"')
		printf("\\\"");
	else if (c == '\0')
		printf("\\0");
	else if (c >= 32 && c < 127)
		putchar(c);
	else
		printf("\\x%02x", c);
}

static void	print_bytes(const char *buf, size_t len)
{
	size_t	i;

	putchar('"');
	i = 0;
	while (i < len && i < MAX_SHOW)
	{
		print_char((unsigned char)buf[i]);
		i++;
	}
	putchar('"');
	if (len > MAX_SHOW)
		printf("... (+%zu more bytes)", len - MAX_SHOW);
}

static void	put_reason(int *first, const char *text)
{
	if (!*first)
		printf(", ");
	printf("%s", text);
	*first = 0;
}

static void	print_reasons(int r)
{
	int	first;

	first = 1;
	if (r & R_REF)
		put_reason(&first, "REFERENCE printf FAILED (harness problem)");
	if (r & R_CRASH)
		put_reason(&first, "ft_printf CRASHED / TIMED OUT");
	if (r & R_OUT)
		put_reason(&first, "OUTPUT BYTES DIFFER");
	if (r & R_RET)
		put_reason(&first, "RETURN VALUE DIFFERS FROM printf");
	if (r & R_COUNT)
		put_reason(&first, "RETURN VALUE != BYTES REALLY WRITTEN");
}

static void	print_status(const t_cap *c)
{
	int	sig;

	if (WIFSIGNALED(c->status))
	{
		sig = WTERMSIG(c->status);
		if (sig == SIGALRM)
			printf("TIMEOUT (more than %d s: infinite loop?)", TIMEOUT_SEC);
		else
			printf("CRASH: killed by signal %d (%s)", sig, strsignal(sig));
	}
	else if (WEXITSTATUS(c->status) != 0)
		printf("exited with code %d", WEXITSTATUS(c->status));
	else
		printf("no return value reported");
}

static void	print_side(const char *label, const t_cap *c)
{
	printf("         %s : ", label);
	print_bytes(c->buf, c->len);
	printf(" (%zu bytes)  return = ", c->len);
	if (c->has_ret)
		printf("%d\n", c->ret);
	else
		printf("n/a\n");
	if (!cap_ok(c))
	{
		printf("         status   : ");
		print_status(c);
		printf("\n");
	}
}

static void	print_diff(const t_cap *a, const t_cap *b)
{
	size_t	i;
	size_t	min;

	min = a->len;
	if (b->len < min)
		min = b->len;
	i = 0;
	while (i < min && a->buf[i] == b->buf[i])
		i++;
	if (i == min && a->len == b->len)
		return ;
	printf("         first difference at byte offset %zu\n", i);
}

static void	print_fail(const t_fail *f)
{
	printf("%s[ FAIL ]%s #%d [%c] %s\n", col(RED), col(RST), f->id,
		f->section, f->expr);
	if (f->note[0])
		printf("         note     : %s\n", f->note);
	printf("         reason   : ");
	print_reasons(f->reason);
	printf("\n");
	print_side("expected", &f->ref);
	print_side("actual  ", &f->act);
	if (cap_ok(&f->ref) && cap_ok(&f->act))
		print_diff(&f->ref, &f->act);
}

/* ------------------------------------------------------------------------ */
/*  judging                                                                 */
/* ------------------------------------------------------------------------ */

static int	get_reason(const t_cap *ref, const t_cap *act)
{
	int	r;

	if (!cap_ok(ref))
		return (R_REF);
	if (!cap_ok(act))
		return (R_CRASH);
	r = 0;
	if (ref->len != act->len || memcmp(ref->buf, act->buf, ref->len) != 0)
		r |= R_OUT;
	if (ref->ret != act->ret)
		r |= R_RET;
	if (act->ret < 0 || (size_t)act->ret != act->len)
		r |= R_COUNT;
	return (r);
}

static void	store_fail(const char *expr, int reason, t_cap *ref, t_cap *act)
{
	t_fail	*f;

	if (g_st.stored >= MAX_FAILS)
	{
		cap_free(ref);
		cap_free(act);
		return ;
	}
	f = &g_st.fails[g_st.stored];
	g_st.stored++;
	f->id = g_st.total;
	f->section = g_st.sec_id;
	f->reason = reason;
	snprintf(f->expr, sizeof(f->expr), "%s", expr);
	snprintf(f->note, sizeof(f->note), "%s", g_st.note);
	f->ref = *ref;
	f->act = *act;
}

static void	print_verdict(int ok, const char *expr)
{
	if (ok)
		printf("  %s[ PASS ]%s #%d %s", col(GRN), col(RST), g_st.total, expr);
	else
		printf("  %s[ FAIL ]%s #%d %s", col(RED), col(RST), g_st.total, expr);
	if (g_st.note[0])
		printf("   (%s)", g_st.note);
	printf("\n");
}

void	judge(const char *expr, t_cap *ref, t_cap *act)
{
	int	reason;

	g_st.total++;
	reason = get_reason(ref, act);
	if (g_st.verbose)
		print_verdict(reason == 0, expr);
	if (reason == 0)
	{
		g_st.passed++;
		cap_free(ref);
		cap_free(act);
	}
	else
	{
		g_st.failed++;
		store_fail(expr, reason, ref, act);
	}
	g_st.note[0] = '\0';
}

/* ------------------------------------------------------------------------ */
/*  sections and final report                                               */
/* ------------------------------------------------------------------------ */

void	section_begin(char id, const char *title)
{
	g_st.sec_id = id;
	g_st.sec_title = title;
	g_st.sec_start_total = g_st.total;
	g_st.sec_start_failed = g_st.failed;
	g_st.sec_skipped = 0;
	if (g_st.verbose)
		printf("\n%s=== %c. %s ===%s\n", col(CYN), id, title, col(RST));
}

void	tester_skip(const char *why)
{
	g_st.sec_skipped = 1;
	g_st.sec_skip_why = why;
	g_st.skipped++;
}

void	section_end(void)
{
	int	run;
	int	bad;

	run = g_st.total - g_st.sec_start_total;
	bad = g_st.failed - g_st.sec_start_failed;
	if (g_st.sec_skipped)
		printf("%s[ SKIP ]%s %c. %-42s %s\n", col(YEL), col(RST),
			g_st.sec_id, g_st.sec_title, g_st.sec_skip_why);
	else if (bad == 0)
		printf("%s[ PASS ]%s %c. %-42s %d/%d\n", col(GRN), col(RST),
			g_st.sec_id, g_st.sec_title, run, run);
	else
		printf("%s[ FAIL ]%s %c. %-42s %d/%d  (%d failed)\n", col(RED),
			col(RST), g_st.sec_id, g_st.sec_title, run - bad, run, bad);
	fflush(stdout);
}

static void	print_totals(void)
{
	const char	*failcol;

	failcol = col(GRN);
	if (g_st.failed > 0)
		failcol = col(RED);
	printf("\n================================================================\n");
	printf(" FT_PRINTF TEST REPORT  (mandatory part)\n");
	printf("================================================================\n");
	printf(" Total tests : %d\n", g_st.total);
	printf(" Passed      : %s%d%s\n", col(GRN), g_st.passed, col(RST));
	printf(" Failed      : %s%d%s\n", failcol, g_st.failed, col(RST));
	if (g_st.skipped > 0)
		printf(" Skipped     : %d section(s)\n", g_st.skipped);
}

int	tester_report(void)
{
	int	i;

	print_totals();
	if (g_st.failed == 0)
	{
		printf("\n %sALL TESTS PASSED%s\n\n", col(GRN), col(RST));
		return (0);
	}
	printf("\n FAILED TESTS (bytes shown C-style, \\0 = a NUL byte)\n");
	printf("----------------------------------------------------------------\n");
	i = 0;
	while (i < g_st.stored)
	{
		if (i < g_st.detail_limit)
			print_fail(&g_st.fails[i]);
		else
			printf("%s[ FAIL ]%s #%d [%c] %s\n", col(RED), col(RST),
				g_st.fails[i].id, g_st.fails[i].section, g_st.fails[i].expr);
		i++;
	}
	if (g_st.stored > g_st.detail_limit)
		printf("\n (only the first %d failures are detailed, use -a for all)\n",
			g_st.detail_limit);
	if (g_st.failed > g_st.stored)
		printf(" (%d more failures not stored)\n", g_st.failed - g_st.stored);
	printf("\n %sSOME TESTS FAILED%s\n\n", col(RED), col(RST));
	return (1);
}