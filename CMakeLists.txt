cmake_minimum_required(VERSION 3.14)
project(know C ASM)

set(CMAKE_C_STANDARD 90)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ggdb -O0")

set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/STM32F303XC_FLASH.ld")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${LINKER_SCRIPT}")

include_directories(${CMSIS_DIR})

include_directories(inc)
add_subdirectory(src)

add_custom_target(erase-flash
	COMMAND st-flash erase
)