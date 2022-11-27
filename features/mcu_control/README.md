# MCU Control using Matlab/Simulink

* GNU Make 3.81, GNU Arm Embedded Toolchain 10.3-2021.10 (10.3.1 20210824)

* STM32CubeMX version 6.4.0 / HAL driver

* MATLAB R2020a / Simulink / Embedded Coder

## How to Build

### Linux

```bash
$ sudo apt update && sudo apt upgrade
$ sudo apt install make build-essential gcc-arm-none-eabi -y
$ ./build.sh
```

### Windows

1. Install gcc arm toolchain (GNU Tools for ARM Embedded Processors)

- https://launchpad.net/gcc-arm-embedded

![](https://github.com/nodang/board_test2/blob/main/Image/1.jpg?raw=true)

- Click the check box that says ( ***Add path to environment variable*** )

2. Install Windows versions of *make* and *rm*(CoreUtils for Windows)

- http://gnuwin32.sourceforge.net/packages/make.htm

![](https://github.com/nodang/board_test2/blob/main/Image/2.jpg?raw=true)

- http://gnuwin32.sourceforge.net/packages/coreutils.htm

- *rm* is installed in the same way as *make*

3.  build

```console
> mkdir out/object
> make
```

# How to flash the images for board (On Windows)

- STM32 Flash loader demonstrator (UM0462) (replaced by STM32CubeProgrammer)   https://www.st.com/en/development-tools/flasher-stm32.html
