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
sleep_ms(long t_ms)
{
#ifdef USE_NANOSLEEP
  struct timespec t_sleep;
  
  t_sleep.tv_sec = t_ms / 1000L;
  t_sleep.tv_nsec = (t_ms - t_sleep.tv_sec*1000L) * 1000000L;
  nanosleep(&t_sleep, NULL);
#else
  long t_us;
  unsigned int sec, usec;
  
  t_us = t_ms * 1000L;
  if ((usec = t_us % 1000000L) > 0) { usleep(usec); }
  if ((sec = t_us / 1000000L) > 0) { sleep(sec); }
#endif
}

int
main()
{
  size_t i, j;
  double t_start = get_time();
  int duration[] = { 10, 5 };
  char const *progress_str[] = { "Progress 1", "Progress 2" };
  size_t n_rows = 2;

  if (init_sigaction(&sa_sigint, SIGINT, exit_signal_handler) == -1) { EXIT_SIGINIT(SIGINT); }
  if (init_sigaction(&sa_sigabrt, SIGABRT, exit_signal_handler) == -1) { EXIT_SIGINIT(SIGABRT); }
  if (init_sigaction(&sa_sigterm, SIGTERM, exit_signal_handler) == -1) { EXIT_SIGINIT(SIGTERM); }

  ProgressbarConfig cfg = get_default_config();
  cfg.rows = 2;
  cfg.type = pbt_frac_color;
  if (create_progressbar(&cfg)) {
    fprintf(stderr, "Error creating progressbar.\n");
    return EXIT_FAILURE;
  }
  for (j = 0; get_time() - t_start < duration[0]; ++j) {
    for (i = 0; i < n_rows; ++i) {
      draw_progressbar(i, progress_str[i], 100.0 * (get_time() - t_start) / duration[i]);
    }
    printf("Hello %lu\n", j);
    sleep_ms(500);
  }
  for (i = 0; i < n_rows; ++i) {
    draw_progressbar(i, progress_str[i], 100.0 * (get_time() - t_start) / duration[i]);
  }
  sleep_ms(2000);
  destroy_progressbar();
  return EXIT_SUCCESS;
}
