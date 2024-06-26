# MCUboot Example for Alif Ensemble E1-E7

This folder contains an example of MCUboot to be used on AppKits and DevKits containing Alif Ensemble E1-E7 along with example applications that can be loaded to demonstrate update functionality.

Example verification public key has been generated from the MCUboot default RSA2048 private key.
NOTE! This key is NOT suitable for production as it's distributed publicly as example key for MCUboot!

## Building

This folder contains CMakeLists.txt for the project.

1. Install and add to PATH:
    1. [cmake](https://cmake.org/) 3.19 or above
    1. build tool (make/nmake)
    1. [gcc-arm-none-eabi compiler](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads) (version 12.3 or above)
    1. [Python 3](https://www.python.org/)
1. Install the MCUboot imgtool requirements from libs/mcuboot/scripts/requirements.txt (`pip install -r libs/mcuboot/scripts/requirements.txt`)
1. Create build folder and enter it. (`mkdir build; cd build`)
1. Run cmake [path_to_this_folder]. (`cmake ..`)
1. Run compilation command (depends on the selected generator of cmake; by default make for Linux and nmake for Windows). (`make`)
1. The built elf-file, binary and map file will be generated in the bin directory under the build folder. Example applications are signed automatically.

## Supported modes

This example is built to work on and manage the application of M55 HE core by default.

Currently, this example supports only OVERWRITE and SWAP modes (see [Bootloader design](https://docs.mcuboot.com/design.html#image-slots)). Only supported storage is internal MRAM. [Verification](https://docs.mcuboot.com/design.html#security) is supported with embedded key and enabled by default but encryption is not supported.

## Deployment

1. Build the MCUboot application and example applications, see [Building](#building)
1. Write MCUboot application and the signed example application to internal MRAM using [Alif Security Toolkit](https://alifsemi.com/support/software-tools/ensemble/). See alif_mcuboot.json in this folder for reference.

## Preparing update

With JLink debugger:
1. Ensure JLink folder is in the path when running cmake.
1. Run `make update` in the build directory.
1. Observe the logs from UART2, the application should indicate update is available to be set pending.

With other tools:
1. Write [build_folder]/bin/alif-example-app-update_signed.bin into address 0x80020000 with any tool.
1. Reboot the board/SoC.
1. Observe the logs from UART2, the application should indicate update is available to be set pending.

## Setting the update pending and updating the application

1. Press the button on the development board to set the update pending.
1. Reboot the board/SoC.
1. Observe logs from UART2, the blinking led also changes to different color blink.

### Observing update flow while flashing with Alif Security Toolkit only

1. Use alif_mcuboot_with_update.json as Alif Security Toolkit atoc configuration file.
1. Observe the logs from UART2, the application should indicate update is available to be set pending.
1. Press the button on the development board to set the update pending.
1. Reboot the board/SoC.
1. Observe logs from UART2, the blinking led also changes to different color blink.

## Verification using UART2 output

### Example output during normal boot (after writing initial images with Alif Security Toolkit)

    INF: Image index: 0, Swap type: none            << Internal MCUboot logging

    Loading image, version 1.0.0 (build: 0)         << Bootloader app prints, see bootloader_app/src/main.c
      image size: 29808.

    Example app running!                            << Example app print, see example_app/src/main.c
    PRIMARY slot:
      offset:    0x80010000
      size:      0x10000
      magic:     1
      swap_type: 1
      copy_done: 2
      image_ok:  1
      image_num: 0
      version:   1.0.0
    SECONDARY slot:
      offset:    0x80020000
      size:      0x10000
      magic:     3
      swap_type: 1
      copy_done: 3
      image_ok:  3
      image_num: 0
    ERR: Bad image magic 0x0
    No update available.

### Example output when update is available

    INF: Image index: 0, Swap type: none            << Internal MCUboot logging

    Loading image, version 1.0.0 (build: 0)         << Bootloader app prints, see bootloader_app/src/
      image size: 29808.

    Example app running!                            << Example app print, see example_app/src/main.c
    PRIMARY slot:
      offset:    0x80010000
      size:      0x10000
      magic:     1
      swap_type: 1
      copy_done: 2
      image_ok:  1
      image_num: 0
      version:   1.0.0
    SECONDARY slot:
      offset:    0x80020000
      size:      0x10000
      magic:     3
      swap_type: 1
      copy_done: 3
      image_ok:  3
      image_num: 0
      version:   2.0.0
    Press button to pend update.



### Example output during update

    INF: Image index: 0, Swap type: perm            << Internal MCUboot logging
    INF: Image 0 upgrade secondary slot -> primary slot
    INF: Erasing the primary slot
    INF: Image 0 copying the secondary slot to the primary slot: 0xzx bytes
    DBG: writing magic; fa_id=1 off=0xfff0 (0x8001fff0)
    DBG: erasing secondary header
    DBG: erasing secondary trailer

    Loading image, version 2.0.0 (build: 0)         << Bootloader app prints, see bootloader_app/src/
      image size: 29808.

    Updated app running!                            << Example app print, see example_app/src/main.c
    PRIMARY slot:
      offset:    0x80010000
      size:      0x10000
      magic:     1
      swap_type: 1
      copy_done: 3
      image_ok:  3
      image_num: 0
      version:   2.0.0
    SECONDARY slot:
      offset:    0x80020000
      size:      0x10000
      magic:     3
      swap_type: 1
      copy_done: 3
      image_ok:  3
      image_num: 0
    ERR: Bad image magic 0x0
    No update available.


### Example output on subsequent resets after app is updated

    INF: Image index: 0, Swap type: none            << Internal MCUboot logging

    Loading image, version 2.0.0 (build: 0)         << Bootloader app prints, see bootloader_app/src/
      image size: 29808.

    Updated app running!                            << Example app print, see example_app/src/main.c
    PRIMARY slot:
      offset:    0x80010000
      size:      0x10000
      magic:     1
      swap_type: 1
      copy_done: 3
      image_ok:  3
      image_num: 0
      version:   2.0.0
    SECONDARY slot:
      offset:    0x80020000
      size:      0x10000
      magic:     3
      swap_type: 1
      copy_done: 3
      image_ok:  3
      image_num: 0
    ERR: Bad image magic 0x0
    No update available.