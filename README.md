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

Roberto, Aldus in Raspibo events
