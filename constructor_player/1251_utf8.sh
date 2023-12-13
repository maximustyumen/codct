#!/bin/sh

find . -name '*.cpp' | while read i; do enconv -L ru -x UTF-8 "$i" ; done
find . -name '*.h' | while read i; do enconv -L ru -x UTF-8 "$i" ; done

