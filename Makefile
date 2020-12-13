TARGET = usart-log
CMSIS_INCLUDE = /Users/Vlad/Dev/Packages/CMSIS/CMSIS/Include
LINKER_SCRIPT = STM32F303XC_FLASH.ld

# REMEMBER about debug flag
OBJECT_GEN_FLAGS = -ggdb -O0 -mthumb -fno-builtin -Wall -ffunction-sections -fdata-sections -fomit-frame-pointer -mabi=aapcs

C_FLAGS = $(OBJECT_GEN_FLAGS) -I$(CMSIS_INCLUDE) -Iinc
ASM_FLAGS = $(OBJECT_GEN_FLAGS) -x assembler-with-cpp

EXE_LINKER_FLAGS = -Wl,--gc-sections --specs=nano.specs -specs=nosys.specs -mthumb -mabi=aapcs -Wl,-Map=out/$(TARGET).map

COMPILER = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
DEBUGGER = arm-none-eabi-gdb

clean:
	rm out/*

out/%.o: src/%.c
	$(COMPILER) $(C_FLAGS) -c $< -o $@

out/%.o: src/%.S	
	$(COMPILER) $(ASM_FLAGS) -c $< -o $@

objects := $(patsubst src/%.c,out/%.o,$(wildcard src/*.c)) $(patsubst src/%.S,out/%.o,$(wildcard src/*.S))

out/$(TARGET).elf: $(objects)
	$(COMPILER) -T $(LINKER_SCRIPT) $(EXE_LINKER_FLAGS) $(objects) -o out/$(TARGET).elf

out/$(TARGET).bin: out/$(TARGET).elf
	$(OBJCOPY) -O binary out/$(TARGET).elf out/$(TARGET).bin

flash: out/$(TARGET).bin
	st-flash write out/$(TARGET).bin 0x8000000

erase:
	st-flash erase	

build: out/$(TARGET).elf	

debug: build 
	$(DEBUGGER) out/$(TARGET).elf \
  -ex 'target remote localhost:3333' \
  -ex 'monitor reset halt'