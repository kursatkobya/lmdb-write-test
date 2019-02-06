# lmdb-write-test
A IO write test utility for lmdb[http://www.lmdb.tech/doc] database.
The program opens a default database on startup and add random data regarding
giving commands.

# Dependencies
* liblmdb

# Build
mkdir build && cd build
cmake .. & make
./lmdb_write_test

# Commands
* w1k    : write random 1024 bytes string value to a random 16byte key
* w2k    : write random 2048 bytes
* w512k  : write random 512 Kbytes
* w50m   : write random 50 MB
* c      : commit key/value pairs to database
* r      : read the value of the last key
* r [key}: read the value of the last key
* e      : exit program

# Test
* Run the program.
* Attach to the programs pid with strace:
    `sudo strace -p ####`
* Set random values using commands on the program and meanwhile observe strace
  to ensure no data sync happens.
* Commit using `c` command on the program and meanwhile observe strace to
  ensure data sync happens
