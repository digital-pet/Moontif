# A Lua Binding for Motif

based on luamotif,
Copyright (C) Micro Systems Marc Balmer \<marc@msys.ch\>

Additions by and (c) [digital-pet](https://github.com/digital-pet/)

Makefile is for BSD systems

GNUmakefile is for Linux systems

## Build (Linux, Debian)

Requires Lua 5.3

    apt install libbsd-dev libx11-dev libmotif-dev liblua5.3-dev
    make
    make install

## TODO

- Implement missing convenience functions such as XmVaCreateMenubar, either here in native C or in Lua
