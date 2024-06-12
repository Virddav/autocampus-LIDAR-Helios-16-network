# Backup

This directory contains the original Makefile and src files for `exampleETSI` in SDK release 19.3.

You can restore `exampleETSI` by doing:

```bash
# Replace current Makefile with backup
$ cp Makefile /home/duser/mk6/stack/apps/exampleETSI/
# Remove all files within src/
$ rm -rf /home/duser/mk6/stack/apps/exampleETSI/src/*
# Copy backup files to src/
$ cp -r src/* /home/duser/mk6/stack/apps/exampleETSI/src/
```

Now you can run `make` in `/home/duser/mk6/stack/apps/exampleETSI/` to build the original tarball.
