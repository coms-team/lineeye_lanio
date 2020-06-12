#! /bin/sh

libdir=/usr/local/lib
libdir=/usr/local/lib
realname=libLANIO.so.1.0.?
soname=libLANIO.so.1
linkername=libLANIO.so
ldcommand=ldconfig

rm -f $libdir/$realname
rm -f $libdir/$soname
rm -f $libdir/$linkername

if [ `which $ldcommand` ]; then
  ldcommand=ldconfig
elif [ -e /sbin/ldconfig ]; then
  ldcommand=/sbin/ldconfig
fi

$ldcommand

echo "Uninstalled."

