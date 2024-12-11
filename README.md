# FOTA Example for Alif Ensemble E1-E7

This folder contains an example to perform FOTA (application update) using [MCUboot](https://docs.mcuboot.com/) and [mcumgr](https://github.com/apache/mynewt-mcumgr) to be used with Alif Ensemble DevKit. Included applications are Bootloaders for both cores that are responsible for handling the applying the updates, Example application running in M55_HE that handles transporting the update to the device and Blinky application that represents application running on M55_HP.

Example verification public key has been generated from the MCUboot default RSA2048 private key.
NOTE! This key is NOT suitable for production as it's distributed publicly as example key for MCUboot!

## Supported configurations

This example is built to work on M55 HE and (optionally) M55 HP core(s). M55 HE acts as the update dowloader, storing the updates into MCUboot's secondary slots.

By default, both cores will apply the update via their respective bootloaders. If HE_UPDATES_BOTH is set to true in CMAKE configuration, HE bootloader will instruct the HP to shut down and then applies the update to both cores. In this scenario HP bootloader only validates the image before jumping into it.

Currently, this example supports OVERWRITE, SWAP and RAM_LOAD modes (see [Bootloader design](https://docs.mcuboot.com/design.html#image-slots)). Only supported storage is internal MRAM. [Verification](https://docs.mcuboot.com/design.html#security) is supported with embedded key and enabled by default but encryption is not supported.

The update can be uploaded to the device and scheduled for updating by three different methods. [Using](#updating-the-application-using-mcumgr-cli) [mcumgr-cli](https://github.com/apache/mynewt-mcumgr-cli) though uart or usb, [direct writing to MRAM by debugger](#updating-the-application-using-debugger-direct-mram-writing) or by [writing the update via Alif Security Toolkit](#observing-update-flow-while-flashing-with-alif-security-toolkit-only).

### MRAM configuration
```
+-------------------+   0x8000 0000
|                   |
|  HE Bootloader    |
|                   |
+-------------------+   0x8001 0000
|                   |
|  HE primary slot  |
|                   |
+-------------------+   0x8002 0000
|                   |
| HE secondary slot |
|                   |
+-------------------+   0x8003 0000
|                   |
|  HE Scratch area  |
|                   |
+-------------------+   0x8003 1000
.                   .
+-------------------+   0x8003 F000
|                   |
|  HP Scratch area  |   (HP scratch area is not in use if HE_UPDATES_BOTH=ON)
|                   |
+-------------------+   0x8004 0000
|                   |
|  HP Bootloader    |
|                   |
+-------------------+   0x8005 0000
|                   |
|  HP primary slot  |
|                   |
+-------------------+   0x8006 0000
|                   |
| HP secondary slot |
|                   |
+-------------------+   0x8007 0000
.                   .
+-------------------+   0x8057 FFFF
```

#### TCM usage in RAM_LOAD configuration
In RAM_LOAD configuration, the layout of the MRAM remains the same as above but the image from the MRAM is loaded into the core's ITCM.
```
+-------------------+   0x0000 0000
|                   |
|     EMPTY (*)     |
|                   |
+-------------------+   0x0000 0800
|                   |
|   Image header    |
|                   |
+-------------------+   0x0000 1000
|                   |
| Application image |
|                   |
+-------------------+   0x0001 0000
.                   .
+-------------------+   <end of ITCM>
```

\* = Due to the way the image signing tool works, RAM_LOAD target cannot start from address 0x0 so empty space is left in the beginning so that Application Image starts from next suitable address given the Vector table alignment requirement of Cortex-M55.

## Building

This folder contains CMakeLists.txt for the project.

1. Install and add to PATH:
    1. [cmake](https://cmake.org/) 3.19 or above
    1. build tool (make/nmake)
    1. [gcc-arm-none-eabi compiler](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads) (version 12.3 or above)
    1. [Python 3](https://www.python.org/)
1. Install the MCUboot imgtool requirements from libs/mcuboot/scripts/requirements.txt (`pip install -r libs/mcuboot/scripts/requirements.txt`).
1. Create build folder and enter it. (`mkdir build; cd build`).
1. Run cmake [path_to_this_folder]. (`cmake ..`).
1. Run compilation command (depends on the selected generator of cmake; by default make for Linux and nmake for Windows). (`make`)
1. The built elf-file, binary and map file will be generated in the bin directory under the build folder. Example applications are signed automatically.
1. OPTIONAL: Install [Go](https://go.dev/), download [mcumgr-cli](https://github.com/apache/mynewt-mcumgr-cli) and follow building instruction from associated README.md to issue update commands via mcumgr-cli.

### Building for RAM_LOAD
To build RAM_LOAD configuration, add parameter `-DMCUBOOT_MODE=RAM_LOAD` to cmake command.

The RAM_LOAD configuration does not support / require image test and confirmation, the later image is always loaded. All uploads will be stored to the secondary slot preserving the initial flashed application as 'golden image' in primary slot.

## Deployment

1. Build the MCUboot application and example applications, see [Building](#building).
1. Write MCUboot application and the signed example application to internal MRAM using [Alif Security Toolkit](https://alifsemi.com/support/software-tools/ensemble/). See alif_mcuboot.json in this folder.
1. Observe logs from UART2 and UART4, serial settings 115200-8-N-1, to verify the applications are running.

## Updating the application using mcumgr-cli

### Preparations
1. [Build](#building) the project with selected transport mechanism. Uart is built by default, usb transport mechanism can be configured with `cmake .. -DTRANSPORT=usb`.
1. Establish connection between your computer and the development board. For uart, UART3 is used by default. This requires attaching an FTDI cable to pins P1_2 and P1_3 on the board. For usb, attach an usb cable to 'SoC USB' port.
1. Create connection profile for the mcumgr-cli, for example: `mcumgr conn add uart type=serial connstring="dev=/dev/ttyACM0,baud=115200"`. Connection type and baud rate are same for uart and usb connections.
1. Test the connection between mcumgr-cli and the device: `mcumgr -cuart echo "Echo test"`. The command should print out 'Echo test' to indicate the device returned same string it was sent.

### Uploading the update
1. Query existing images on the device: `mcumgr -cuart image list`.
1. Upload the update: `mcumgr -cuart image upload build/bin/alif-example-app-update_signed.bin -w1`.
1. Query existing images on the device: `mcumgr -cuart image list`.

       $ ./mcumgr -cuart image list
       Images:
        image=0 slot=0
           version: 1.0.0
           bootable: true
           flags: active confirmed
           hash: 06147dfeb20df969eeea5b01028ec6635a9a54adea43035997a050618417cb97
        image=0 slot=1
           version: 2.0.0
           bootable: true
           flags: 
           hash: 5d2f3bf869e7aafd29825a932e285daafc42ac7aaa00958096dfde09dd3c2fb9
       Split status: N/A (0)

### Setting the update pending and updating the application
1. Mark the image to be test-booted on next reset: `mcumgr -cuart image test <hash>`.  
   * The image hash can be seen in the output of the `mcumgr -cuart image list` command, under image= 0 slot=1
1. Reset the device: `mcumgr -cuart reset`.
1. Validate new image is running: `mcumgr -cuart image list`.

### Reverting the update
1. After update, query existing images on the device: `mcumgr -cuart image list`.
1. Reset the device: `mcumgr -cuart reset`.
1. Validate the old firmware is now running: `mcumgr -cuart image list`.

### Setting the updated image as confirmed
1. After update, query existing images on the device: `mcumgr -cuart image list`.
1. Confirm the update so it won't be reverted on next reset: `mcumgr -cuart image confirm <hash>`.  
   * The hash for this command is the same as given to `mcumgr -cuart image test <hash>`-command in [Setting the update pending and updating the application](#setting-the-update-pending-and-updating-the-application)-phase. At this point, it can be seen under image=0 slot=0 in the `mcumgr -cuart image list`-command.
1. Reset the device: `mcumgr -cuart reset`.
1. Validate new image is still running: `mcumgr -cuart image list`.

## Updating the application using debugger (direct MRAM writing)

This flow can be used to inspect the application-side logic only. Debugger writing directly to the MRAM is used as transport layer.

### Uploading the update

#### With JLink debugger:
1. Ensure JLink folder is in the path when running cmake.
1. Run `make update` in the build directory.
1. Observe the logs from UART2, the application should indicate update is available to be set pending.

#### With other tools:
1. Write [build_folder]/bin/alif-example-app-update_signed.bin into address 0x80020000 with any tool.
1. Reboot the board/SoC by pressing SW2 (RESET).
1. Observe the logs from UART2, the application should indicate update is available to be set pending.

### Setting the update pending and updating the application
1. Press the SW1 button on the development board to set the update pending.
1. Reboot the board/SoC by pressing SW2 (RESET).
1. Observe logs from UART2, the blinking led also changes to different color blink.

### Reverting the update
1. After update, observe logs from UART2.
1. Reboot the board/SoC by pressing SW2 (RESET) without pressing the SW1 button on the development board.
1. Observe logs from UART2, the blinking led also changes to different color blink when original image is loaded again.
1. The update can be set pending again by pressing the SW1 button on the development board.

### Setting the updated image as confirmed
1. Press the SW1 button on the development board after update has been done.
1. Reboot the board/SoC by pressing SW2 (RESET).
1. Observe logs from UART2, updated image is loaded consistently through resets.

## Observing update flow while flashing with Alif Security Toolkit only
In this flow, both the 'initial' example application and the update candidate is written using Alif Security Toolkit.
1. Use alif_mcuboot_with_update.json as Alif Security Toolkit atoc configuration file.
1. Observe the logs from UART2, the application should indicate update is available to be set pending.
1. Press the SW1 button on the development board to set the update pending.
1. Reboot the board/SoC by pressing SW2 (RESET).
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
