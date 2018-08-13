CDIR=Progressbar
PDIR=pyprogressbar
CFLAGS=-std=c89 -O3 -Wall -Wextra
IPY=/usr/include/python2.7

demo: py
	bash make_demo.sh
py:
	swig -python $(PDIR)/progressbar.i
	gcc -c -fpic $(CFLAGS) $(CDIR)/progressbar.c -I$(IPY) -I. -o $(PDIR)/build/progressbar.o
	gcc -c -fpic $(CFLAGS) $(CDIR)/ansi_ctrl.c -I$(IPY) -I. -o $(PDIR)/build/ansi_ctrl.o
	gcc -c -fpic $(CFLAGS) $(PDIR)/progressbar_wrap.c -I$(IPY) -I. -o $(PDIR)/build/progressbar_wrap.o
	gcc -shared $(CFLAGS) $(PDIR)/build/progressbar.o $(PDIR)/build/ansi_ctrl.o $(PDIR)/build/progressbar_wrap.o -o $(PDIR)/_progressbar.so
	ln -sf ../$(PDIR) Demo/$(PDIR)
clean:
	bash make_clean.sh
