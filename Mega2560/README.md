The ThanielBase directory contains a VPW bus monitoring tool that was originally written by someone named 
Thaniel, who gave the code to PeteS (aka Loud160) when Pete and I (and a few other folks) were working on
tools to read and write the flash memory of General Motors P01 and P59 powertrain control modules (PCMs).

Those projects eventually turned into LS Droid and PCM Hammer.

I relied on the bus monitoring extensively during the development of PCM Hammer.

The ThanielPlusLedShield directory contains basically the same thing but with some added code to visualize
messages on an Adafruit 5x8 RGB LED shield. It's not tremendously useful but it is kind of have some hint
about what's happening on the VPW bus.

Thaniel's original code was able to read the flash memory of a PCM, but this version is missing some key
pieces for that. It worked well enough that I'm tempted to tidy up the code into something a bit more
readable and maintainable, but... too many projects, too little time.

Some context: https://pcmhacking.net/forums/viewtopic.php?f=4&t=1566&start=235
