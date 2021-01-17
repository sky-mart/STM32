file(GLOB_RECURSE SOURCES "*.c" "*.S")

add_executable(${PROJECT_NAME}.elf ${SOURCES})

target_link_options(${PROJECT_NAME}.elf PRIVATE -Wl,-Map=${PROJECT_NAME}.map)

set_target_properties(${PROJECT_NAME}.elf PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})

add_custom_target(${PROJECT_NAME}.bin 
	COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	DEPENDS ${PROJECT_NAME}.elf
)

add_custom_target(${PROJECT_NAME}-flash
	COMMAND st-flash write ${PROJECT_NAME}.bin 0x8000000
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	DEPENDS ${PROJECT_NAME}.bin
)

# openocd -f board/stm32f3discovery.cfg in another window
add_custom_target(${PROJECT_NAME}-debug
	COMMAND arm-none-eabi-gdb ${PROJECT_NAME}.elf -ex 'target remote localhost:3333' -ex 'monitor reset halt'
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	DEPENDS ${PROJECT_NAME}.elf
)
