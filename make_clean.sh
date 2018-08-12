#!/bin/bash

PDIR=pyprogressbar

rm -f $PDIR/build/* $PDIR/progressbar_wrap.c $PDIR/_progressbar.so $PDIR/progressbar.py $PDIR/*.pyc > /dev/null 2>&1
rm -rf Demo/build/* Demo/demo > /dev/null 2>&1
rm Demo/$PDIR > /dev/null 2>&1
