#!/bin/bash

cd $(dirname "$0")
cp ../aoc_makefile/Makefile /home/duser/mk6/stack/apps/exampleETSI/
cp -r ../src/* /home/duser/mk6/stack/apps/exampleETSI/src/
cp ../DSRC_RAW/* /home/duser/mk6/stack/apps/exampleETSI/
make
mv /home/duser/mk6/stack/apps/exampleETSI/exampleETSI-mk6-Exported.tgz ../
