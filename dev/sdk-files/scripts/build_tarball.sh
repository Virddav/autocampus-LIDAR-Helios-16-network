#!/bin/bash

cd $(dirname "$0")
cp ../aoc_makefile/Makefile /home/duser/mk6/stack/apps/exampleETSI/
cp ../configs/build/* /home/duser/mk6/stack/apps/exampleETSI/
cp -r ../src/* /home/duser/mk6/stack/apps/exampleETSI/src/
make
mv /home/duser/mk6/stack/apps/exampleETSI/exampleETSI-mk6-Exported.tgz ../
