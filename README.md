# A Lua Binding for Motif

based on luamotif,
Copyright (C) Micro Systems Marc Balmer \<marc@msys.ch\>

Modifications by and (c) [digital-pet](https://github.com/digital-pet/) under GPLv3

Makefile is for BSD systems

GNUmakefile is for Linux systems

## Build (Linux, Debian)

Requires Lua 5.3

    apt install libbsd-dev libx11-dev libmotif-dev liblua5.3-dev libgc-dev
    make
    make install

## TODO

- Wrap XmMainWindowSetAreas

## Examples


![A hello world application displaying its own source code](https://i.imgur.com/Pt2GRbr.png)<br>Hello.lua
