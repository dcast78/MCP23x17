MCP23x17
======

MCP23x17 code repository for Raspberry Pi.

---------------------------

This is a collection of scripts used with MCP23x17 chips/circuits.

The files related to piface also work with Piface board.

To compile source file (example):

gcc piface.c -o piface 

to run program:

./piface /dev/spi0.0

For an italian tutorial:

http://raspibo.v2.cs.unibo.it/wiki/index.php/GPIO_aggiuntivi_MCP23x17

The code is based on:

https://gist.github.com/ChickenProp/3183960 

and Micrchip datasheet

http://ww1.microchip.com/downloads/en/devicedoc/21952b.pdf

and Piface board

http://www.element14.com/community/docs/DOC-52857/l/piface-digital-for-raspberry-pi

Relized with my friends:

Roberto, Alduss in Raspibo events

====================================

The MIT License (MIT)

Copyright (c) <year> <copyright holders>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
