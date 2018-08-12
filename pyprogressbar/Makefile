FNAME = progressbar

compile:
	swig -python $(FNAME).i
	gcc -c -fpic -std=gnu99 -O3 -Wall -Wextra $(FNAME).c $(FNAME)_wrap.c -I/usr/include/python2.7
	gcc -shared -std=gnu99 -O3 -Wall -Wextra $(FNAME).o $(FNAME)_wrap.o -o _$(FNAME).so
clean:
	rm $(FNAME).o $(FNAME)_wrap.o $(FNAME)_wrap.c _$(FNAME).so $(FNAME).py *.pyc
