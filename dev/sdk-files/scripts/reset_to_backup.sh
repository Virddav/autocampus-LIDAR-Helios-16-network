#!/bin/bash

cd $(dirname "$0")
rm -rf /home/duser/mk6/stack/apps/exampleETSI/src/*
cp -r ../backup/src/* /home/duser/mk6/stack/apps/exampleETSI/src/
cp -r ../backup/Makefile /home/duser/mk6/stack/apps/exampleETSI/
make
