# PAWSv2 a Risc-V ( 32bit RV32IMAFC+B or 64bit RV64GC )

* Written in Silice
    * Inspired by ICE-V by [ICE-V](https://github.com/sylefeb/Silice/tree/master/projects/ice-v) by @sylefeb

Refer to RV32 for the 32bit CPU, or RV64 for the 64bit CPU. Development has stopped on the 32bit CPU, but is kept for reference.

Copy the files from the directory "SOFTWARE/PAWS" to an SDCARD and insert into the ULX3S. Flash the bitstream from "ULX3S/BUILD_ulx3s/PAWSv2-32.bit" or "ULX3S/BUILD_ulx3s/PAWSv2-64.bit" to the ULX3S

32bit software has a .PAW extension. 64bit software has a .P64 extension.

<br>
The 32-bit BIOS
<br>

```
fujprog PAWSv2-32.bit
```

![BIOS 32 BIT](Reference/Graphics/BIOS-32.jpg)

<br>
The 64-bit BIOS
<br>

```
fujprog PAWSv2-64.bit
```

![BIOS 64 BIT](Reference/Graphics/BIOS-64.jpg)

This will initialise the BIOS and allow the selection of a .PAW or .P64 file to load and run.


# Example Program Screenshots

## 3D MAZE
Based upon the ZX-81 3D MONSTER MAZE using PacMan Ghosts instead of the dinorsaur, as I don't have graphics for a Tyrannosaurus Rex.
<br>
![3D MAZE](Reference/Graphics/MAZE-1.jpg)

![3D MAZE](Reference/Graphics/MAZE-2.jpg)

## IMGUI
A test running the IMGUI DEMO to check PAWSv2 against other people's code.
<br>
![IMGUI Test](Reference/Graphics/IMGUI.jpg)

## PACMAN
A conversion of https://github.com/floooh/pacman.c to the PAWSv2 graphics, sound and input system.
<br>
![PacMan Welcome Screen](Reference/Graphics/PACMAN-1.jpg)

![PacMan Start Screen](Reference/Graphics/PACMAN-2.jpg)

## RAYTRACE
A test of the floating point unit. Runs first in 256 colours, then in 256 greys.
<br>
![RAYTRACING Colour](Reference/Graphics/RAY-1.jpg)

![RAYTRACING Grey](Reference/Graphics/RAY-2.jpg)
