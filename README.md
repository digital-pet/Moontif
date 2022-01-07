# A Lua Binding for Motif

Copyright (C) Micro Systems Marc Balmer.

You can reach the author at marc@msys.ch


Makefile is for BSD systems

GNUmakefile is for Linux systems

## Build (Linux, Debian)

Requires Lua 5.3

    apt install libbsd-dev libx11-dev libmotif-dev liblua5.3-dev
    make
    make install

## TODO

- Add XmVaCreateMenubar
- Determine other missing extension methods and implement as needed
