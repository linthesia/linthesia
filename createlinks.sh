#!/bin/sh

if [ "$#" -ne 2 ]; 
then
    pushd $(dirname "$0")
else
    pushd $2
    cd $1
fi

for d in graphics-*/ ; do
    pushd "$d"
    ln -s ../graphics/* .
    popd
done

popd

