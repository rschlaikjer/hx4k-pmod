cmake_minimum_required(VERSION 3.1.3)

# Set the ARM toolchain
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/cmake/toolchain-arm.cmake")
message(STATUS "Cmake toolchain: ${CMAKE_TOOLCHAIN_FILE}")

# Need to define a project right after the toolchain to force evaluation
project(fpga_programmer C CXX)

# Common paths
get_filename_component(LINKERSCRIPT_DIR "ldscripts" REALPATH)
get_filename_component(SCRIPTS_DIR "scripts" REALPATH)

# Include macro definitions
include(cmake/macros.cmake)

# Include vendored dependencies
add_subdirectory(vendor)

# Global CFLAGS defaults
set(OPT "-Os")
set(DEBUG "-ggdb3")
set(CSTD "-std=c99")
set(CXXSTD "-std=c++14")

set(CFLAGS "-Wall -Wextra -Wno-unused -Wshadow -Wimplicit-function-declaration \
            -Wredundant-decls -Wmissing-prototypes -Wstrict-prototypes \
            -fno-common -ffunction-sections -fdata-sections \
            -fno-builtin -fno-exceptions -fno-unwind-tables")

set(CXXFLAGS
    "-Wall -Wextra -Wshadow -Wredundant-decls -Wno-unused -Weffc++ \
     -fno-common -ffunction-sections -fdata-sections -fno-threadsafe-statics \
     -fno-builtin -fno-rtti -fno-exceptions -fno-unwind-tables -fno-use-cxa-atexit")


# Project specific data
set(LDSCRIPT STM32F070CBT6.ld)
set(CM3_FAMILY opencm3_stm32f0)
set(BMP_PORT "/dev/ttyACM0")

set(SRCS
    src/flash.cpp
    src/fpga.cpp
    src/main.cpp
    src/rgb.cpp
    src/spi.cpp
    src/uart.cpp
    src/usb.cpp
    src/usb_interfaces.c
    src/usb_protocol.cpp
)

set(DEFS "${DEFS} ${USER_DEFS} -DSTM32F0 -DPRINTF_DISABLE_SUPPORT_FLOAT")
set(FP_FLAGS "${FP_FLAGS} -msoft-float")
set(ARCH_FLAGS "${ARCH_FLAGS} -mthumb -mcpu=cortex-m0 ${FP_FLAGS}")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OPT} ${DEBUG} ${CSTD} ${CFLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OPT} ${DEBUG} ${CXXSTD} ${CXXFLAGS}")

set(LINKER_FLAGS
    "--static -nostartfiles \
    -T${LINKERSCRIPT_DIR}/${LDSCRIPT} ${ARCH_FLAGS} ${DEBUG} \
    -Wl,-Map=${PROJECT_NAME}.map -Wl,--cref \
    -Wl,--gc-sections -Wl,--no-wchar-size-warning \
    -Wl,--start-group -lc -lgcc -lstdc++ -lnosys -Wl,--end-group -lm")

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${libopencm3_INCLUDE_DIRS}
)

link_directories(
    ${libopencm3_LIB_DIRS}
)

add_executable(fpga_programmer_elf
    ${SRCS}
)
add_dependencies(fpga_programmer_elf
    libopencm3
)
target_link_libraries(fpga_programmer_elf
    ${CM3_FAMILY}
)
target_compile_definitions(fpga_programmer_elf PRIVATE ${FLAGS})
set_target_properties(fpga_programmer_elf PROPERTIES COMPILE_FLAGS
    "${DEFS} ${ARCH_FLAGS}")
set_target_properties(fpga_programmer_elf PROPERTIES LINK_FLAGS
    "${LINKER_FLAGS}")
get_target_property(PROP fpga_programmer_elf COMPILE_FLAGS)

# Create additional targets for size / flashing
define_flash_target(fpga_programmer)
define_size_target(fpga_programmer)
include_gdbinit()
