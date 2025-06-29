# mytar - Tar Archive Utility

`mytar` is a command-line tool for managing `.tar` archives, supporting basic listing and extraction of files.

## Features

  * `-t`: List archive contents.
  * `-x`: Extract files from the archive.
  * `-v`: Verbose output during extraction (`-x` only).
  * `-f <archive_file>`: Mandatory option to specify the archive.

## Compilation

Compile using the provided `Makefile`:

```bash
make
```

## Usage

```bash
./mytar [options] -f <archive_file> [files_to_process...]
```

### Examples:

  * **List all:** `./mytar -t -f myarchive.tar`
  * **Extract all:** `./mytar -x -f myarchive.tar`
  * **Extract specific:** `./mytar -x -f myarchive.tar file1.txt`
  * **Extract verbose:** `./mytar -x -v -f myarchive.tar`
