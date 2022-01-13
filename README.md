# Moontif

An obsolete UI toolkit for a modern-ish language. Plays well with [NsCDE](https://github.com/NsCDE/NsCDE) and [CDE](https://sourceforge.net/projects/cdesktopenv/).

Not all widgets are implemented yet. Creating a MenuBar, for example, is still an exercise in frustration. This will be fixed in future releases.

## Build (Linux, Debian)

Requires Lua 5.3

    apt install libbsd-dev libx11-dev libmotif-dev liblua5.3-dev libgc-dev
    make
    make install

## Examples


![A hello world application displaying its own source code](https://i.imgur.com/WXtCknq.png)<br>Hello.lua

## Credits

* based on luamotif, Portions copyright (C) Micro Systems Marc Balmer \<marc@msys.ch\>
