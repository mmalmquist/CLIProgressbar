#!/usr/bin/env python2

import time
from pyprogressbar.progressbar import (create_progressbar,
                                       destroy_progressbar,
                                       draw_progressbar,
                                       get_default_config,
                                       pbt_no_color,
                                       pbt_one_color,
                                       pbt_frac_color)
import threading


class thread_handler:
    def __init__(self):
        self._f_percent_done = list()
        self._f_message = list()
        self._running = False
        self._thread = None

    def add_row(self, f_percent_done_, f_message_):
        self._f_percent_done.append(f_percent_done_)
        self._f_message.append(f_message_)

    def start(self):
        self._thread = threading.Thread(target=self._run)
        self._running = True
        self._thread.start()

    def stop(self):
        self._running = False
        self._thread.join()
        self._thread = None

    def _run(self):
        while self._running:
            for i, (msg, prc) in enumerate(zip(self._f_message,
                                               self._f_percent_done)):
                draw_progressbar(i, msg(), prc())
                if not self._running:
                    break
                time.sleep(0.01)


class progressbar:
    def __init__(self, cfg_, thread_handler_):
        self._cfg = cfg_
        self._thread_handler = thread_handler_

    def __enter__(self):
        create_progressbar(self._cfg)
        self._thread_handler.start()
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self._thread_handler.stop()
        destroy_progressbar()
        return False


if __name__ == '__main__':
    t_start = time.time()
    duration = [10, 5]
    progress_str = ["Progress 1", "Progress 2"]
    n_rows = 2

    p = get_default_config()
    p.rows = n_rows
    p.type = pbt_frac_color

    handler = thread_handler()
    for i in range(n_rows):
        handler.add_row(lambda i=i: 100.0*(time.time()-t_start)/duration[i],
                        lambda i=i: progress_str[i])

    with progressbar(p, handler):
        j = 0
        while time.time() - t_start < duration[0]:
            for i in range(100):
                time.sleep(0.01)
            print("Status update %d." % j)
            j += 1
        time.sleep(2.0)
