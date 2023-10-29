#!/bin/sh

pushd $2
cd $1

for d in graphics-*/ ; do
    pushd "$d"
    ln -s ../graphics/* .
    popd
done

popd

