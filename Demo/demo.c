#if _POSIX_C_SOURCE >= 199309L
 #define USE_CLOCK_GETTIME
 #define USE_NANOSLEEP
 #include <time.h>
#else
 #define _BSD_SOURCE
 #include <sys/time.h>
 #include <unistd.h>
#endif

#include <progressbar.h>

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>


/* Ensures that the scroll region is restored on abnormal exit. */
struct sigaction sa_sigint, sa_sigabrt, sa_sigterm;

#define FREE(x) free(x); x = NULL
#define EXIT(status, ...) { fprintf(stderr, __VA_ARGS__); exit(status); }
#define EXIT_SIGINIT(signum) EXIT(signum, "Error initializing " #signum " handler: %s\n", strerror(errno))

static void exit_signal_handler(int sig);
static int init_sigaction(struct sigaction *sa, int signum, void (*handler)(int));

static int
init_sigaction(struct sigaction *sa,
	       int signum,
	       void (*handler)(int))
{
  sigemptyset(&sa->sa_mask);
  sa->sa_flags = 0;
  sa->sa_handler = handler;
  return sigaction(signum, sa, NULL);
}

static void
exit_signal_handler(int sig)
{
  (void)sig;
  
  signal(sig, SIG_IGN);
  destroy_progressbar();
  signal(sig, SIG_DFL);
  raise(sig);
}

extern double
get_time(void)
{
#ifdef USE_CLOCK_GETTIME
  struct timespec t;
  
  clock_gettime(CLOCK_REALTIME, &t);
  return t.tv_sec + t.tv_nsec / 1e9;
#else
  struct timeval t;

  gettimeofday(&t, NULL);
  return t.tv_sec + t.tv_usec / 1e6;
#endif
}

static void
sleep_ms(unsigned long t_ms)
{
#ifdef USE_NANOSLEEP
  struct timespec t_sleep;
  
  t_sleep.tv_sec = t_ms / 1000L;
  t_sleep.tv_nsec = (t_ms - t_sleep.tv_sec*1000L) * 1000000L;
  nanosleep(&t_sleep, NULL);
#else
  unsigned long t_us;
  unsigned int sec, usec;
  
  t_us = t_ms * 1000L;
  if ((usec = t_us % 1000000L) > 0) { usleep(usec); }
  if ((sec = t_us / 1000000L) > 0) { sleep(sec); }
#endif
}

typedef struct
{
  unsigned row;
  double t_start;
  double t_duration;
  char *str;
} PBInfo;

static void
print_bars(PBInfo const *const *pbi)
{
  PBInfo const *p;

  while ((p = *pbi++)) {
    draw_progressbar(p->row, p->str, 100.0 * (get_time() - p->t_start) / p->t_duration);
  }
}

static PBInfo **
create_pbinfo(unsigned rows,
	      char const *msg[],
	      double const *duration)
{
  PBInfo **pbi;
  unsigned i;
  
  pbi = (PBInfo **) calloc(rows + 1, sizeof(PBInfo *));
  for (i = 0; i < rows; ++i) {
    pbi[i] = (PBInfo *) calloc(1, sizeof(PBInfo));
    pbi[i]->row = i;
    pbi[i]->str = (char *) calloc(strlen(msg[i]) + 1, sizeof(char));
    strcpy(pbi[i]->str, msg[i]);
    pbi[i]->t_duration = duration[i];
  }
  return pbi;
}

static void
destroy_pbinfo(PBInfo **const pbi)
{
  PBInfo **pp, *p;
  
  for (pp = pbi; (p = *pp++);) {
    FREE(p->str);
    FREE(p);
  }
  free(pbi);
}

int
main(int argc, char **argv)
{
  unsigned i, j;
  double t_start;
  ProgressbarConfig cfg;
  PBInfo **pbi, **pp, *p;
  unsigned n_rows = 2;
  char const *progress_str[] = { "Progress 1", "Progress 2" };
  double duration[] = { 10, 5 };
  (void)argc; (void) argv;

  if (init_sigaction(&sa_sigint, SIGINT, exit_signal_handler) == -1) { EXIT_SIGINIT(SIGINT); }
  if (init_sigaction(&sa_sigabrt, SIGABRT, exit_signal_handler) == -1) { EXIT_SIGINIT(SIGABRT); }
  if (init_sigaction(&sa_sigterm, SIGTERM, exit_signal_handler) == -1) { EXIT_SIGINIT(SIGTERM); }

  cfg = get_default_config();
  cfg.rows = 2;
  cfg.type = pbt_frac_color;
  if (create_progressbar(&cfg)) {
    fprintf(stderr, "Error creating progressbar.\n");
    return EXIT_FAILURE;
  }
  pbi = create_pbinfo(n_rows, progress_str, duration);
  t_start = get_time();
  for (pp = pbi; (p = *pp++); p->t_start = t_start);
  
  for (j = 0; get_time() - t_start < duration[0]; ++j) {
    for (i = 0; i < 100; i++) {
      print_bars((PBInfo const **)pbi);
      sleep_ms(10);
    }
    printf("Status update %u.\n", j);
  }
  print_bars((PBInfo const **)pbi);
  sleep_ms(2000);
  
  destroy_pbinfo(pbi);
  destroy_progressbar();
  return EXIT_SUCCESS;
}
