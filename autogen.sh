#! /bin/sh -x

aclocal -I m4
autoheader
autoconf
libtoolize --automake
automake -a
