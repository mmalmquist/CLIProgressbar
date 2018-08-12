#ifndef PROGRESSBAR_H_
#define PROGRESSBAR_H_

#include <unistd.h>

typedef enum {
  pbt_no_color,
  pbt_one_color,
  pbt_frac_color
} ProgressbarType;

typedef struct {
  size_t rows;
  char left_edge;
  char right_edge;
  char fill;
  char empty;
  ProgressbarType type;
} ProgressbarConfig;

extern ProgressbarConfig
get_default_config(void);

extern int
create_progressbar(ProgressbarConfig const *pc);
extern void
destroy_progressbar(void);

extern void
draw_progressbar(size_t row,
		 char const *msg,
		 double percent);

#endif /* PROGRESSBAR_H_ */
