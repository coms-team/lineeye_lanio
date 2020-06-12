#! /bin/bash

libdir=/usr/local/lib
realname=libLANIO.so.1.0.?
soname=libLANIO.so.1
linkername=libLANIO.so
ldcommand=ldconfig

cp library86/$realname $libdir

if [ `which $ldcommand` ]; then
  ldcommand=ldconfig
elif [ -e /sbin/ldconfig ]; then
  ldcommand=/sbin/ldconfig
fi

$ldcommand

LD_SUCCESS=`$ldcommand -p | grep libLANIO | wc -l`
if [ $LD_SUCCESS -eq 0 ]; then
    echo "adding /usr/local/lib to library path.."
    echo $libdir > /etc/ld.so.conf.d/LE-local-lib.conf 
    $ldcommand
fi

ln -sf $libdir/$soname $libdir/$linkername
echo "Installed."
