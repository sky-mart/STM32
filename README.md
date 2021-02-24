## STM32 bare metal examples

arm-none-eabi toolchain is used to assembly and debug

CMSIS is used to operate with peripherals

CMake is used as a build system

```bash
# to build
cmake . -B build -DCMAKE_TOOLCHAIN_FILE=arm-none-eabi.cmake -DCMSIS_DIR=<path/to/CMSIS>
# make -C build know.elf
make -C build know.bin
```

```bash
# to flash or erase
make -C build know-flash
make -C build erase-flash
```

```bash
# to debug
openocd -f board/stm32f3discovery.cfg # separate window
make -C build know-debug
```