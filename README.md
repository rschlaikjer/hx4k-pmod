# HX4k PMOD Breakout

This repository contains the KiCad hardware design files, flash programming
firmware and some gateware samples for a basic FPGA breakout board based on the
Lattice Ice40 HX4K.

![Assembled Board](/img/hx4k_breakout_front_1000px.jpg)

## Building the hardware

### Prerequisites

- Soldering iron (TS-100 or TS-80 would be fine for this)
- Flux (Massively simplifies the soldering of the QFP packages)
- Fine tweezers (I recommend [Rhino SW-11](https://www.adafruit.com/product/3096) or comparable)
- [Optional] Hot air rework station. The '858D' hot air station is available
from many resellers at reasonably prices, and is more than adequate.

One of the design goals for this board was for it to be feasible to assemble by
hand, using only a soldering iron. To that end, all of the critical
components are leaded packages, and there are no passives smaller than 0805.
For some components (USB type-C connector J4, 12MHz oscillator X1 and RGB LEDs
D1 and U4), a hot air gun may be advantageous, but it is possible to get away
without one.

### Bill of materials

The KiCad design files are fully specified with the manufacturer and DigiKey
part numbers for all components involved. Also included in the hardware/ folder
is a DigiKey BOM CSV (`digikey_bom.csv`), which can be uploaded
[here](https://www.digikey.com/BOM/) and used to immediately add the necessary
components to an order. Note that the BOM item quantity is for _precisely_ one
unit - I would recommend going through and adding whatever fudge factor you
feel is reasonable to cheaper passives and other parts you want some margin for
error with.

![BOM View](/img/bom.png)

At time of writing, the total cost of the BOM for no more than one board is $37.60.
If you intend to make more than one, the unit price will of course decrease.

### Assembly

I highly recommend printing out the `F.Fab` and `B.Fab` layers of the KiCad
design before getting to work, as these contain a wireframe layout of all the
components, as well as their associated values.

![Board Assembly](/img/assembly.jpg)

# Flashing the microcontroller

Once you have the hardware assembled, you will need to program the onboard
microcontroller. This micro then acts in much the same way as an FTDI FT2232
(but costs significantly less), and once set up allows for

- Writing bitstreams / other data to the onboard flash chip
- Creating a USB serial bridge to the FPGA
- Any other custom logic you want to add. The microcontroller has one hardware
SPI connected directly to the FPGA for end user applications.

To build the firmware, you will need the `arm-none-eabi` GCC toolchain:

    sudo apt install gcc-arm-none-eabi gdb-arm-none-eabi binutils-arm-none-eabi

You should then be able to build the firmware as a normal CMake project:

    # Ensure submodule for libopencm3 is present
    git submodule init
    git submodule update
    # Build
    mkdir programmer_firmware/build
    pushd programmer_firmware/build
    cmake ../
    make -j$(nproc)

To program, it is assumed that you will be using the
[Black Magic Probe](https://1bitsquared.com/products/black-magic-probe)
with a
[TagConnect TC-2050-NL](https://www.tag-connect.com/product/tc2050-idc-nl-050-all)
cable to connect to the programming pads on the board. In this case, after
connecting power to the board through either the USB connector or auxiliary
power connector,  you can
run the cmake target `fpga_programmer_flash` to attach to the microcontroller
and load the firmware. You should see output like the following:

    ross@mjolnir:/h/r/P/G/f/p/build$ make fpga_programmer_flash
    [ 44%] Built target libopencm3
    [100%] Built target fpga_programmer_elf
    Black Magic Probe (Firmware v1.6.1-311-gfbf1963) (Hardware Version 3)

    Available Targets:
    No. Att Driver
     1      STM32F07 M0
    event_loop () at /home/ross/Programming/Github/fpga-swe-1/programmer_firmware/src/main.cpp:87
    87	}
    Loading section .text, size 0x29e4 lma 0x8000000
    Loading section .init_array, size 0x8 lma 0x80029e4
    Loading section .data, size 0x14 lma 0x80029ec
    Start address 0x8001e84, load size 10752
    Transfer rate: 18 KB/sec, 768 bytes/write.
    Section .text, range 0x8000000 -- 0x80029e4: matched.
    Section .init_array, range 0x80029e4 -- 0x80029ec: matched.
    Section .data, range 0x80029ec -- 0x8002a00: matched.
    Kill the program being debugged? (y or n) [answered Y; input not from terminal]
    [100%] Built target fpga_programmer_flash

You should then see the board show up as a USB device, with the default VID:PID
set to a generic test PID:

    ross@mjolnir:/h/r/P/G/f/p/build$ lsusb -d 1209:0001
    Bus 001 Device 106: ID 1209:0001 Generic pid.codes Test PID

You should also notice that it has supplied a new serial device. This serial
port controls the hardware UART on the microcontroller, which is connected
directly to two pins on the FPGA and by default expects a baudrate of 2M.

## Flashing the FPGA

In order to generate bitstreams for the FPGA, you will first need to install
[yosys](https://github.com/YosysHQ/yosys),
[icestorm](http://www.clifford.at/icestorm/)
and
[nextpnr](https://github.com/YosysHQ/nextpnr). Instructions for all of them are
on their respective homepages.

Once these are installed, if you run `make` in the `gateware/` subfolder you
should be able to generate a `top.bin` bitstream of the demo application, which
will blink some lights and act as an echo UART.

In order to upload the bitstream, you will then need to build
[faff](https://github.com/rschlaikjer/faff),
a tool that handles the USB communication necessary to erase and reprogram the
flash through the microcontroller.

Once you have that installed and in your `$PATH`, you can run `make prog` from
the `gateware/` directory and should see it connect and program successfully:

    ross@mjolnir:/h/r/P/G/f/gateware$ make prog
    faff -f top.bin
    Claimed device 1209:0001 with serial 004700254753511120303234
    Flash chip mfgr: 0xef, Device ID: 0x17 Unique ID: 0xe4682c404b163333
    Programming block 0x00020fa0 / 0x00020fbc
    Reading block 0x00020fa0 / 0x00020fbc

Your FPGA should now cycle through some various colours on the RGB LED.

![Blink Demo](/img/blink_light.gif)

