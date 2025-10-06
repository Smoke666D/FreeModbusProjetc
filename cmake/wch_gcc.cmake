


set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(TOOLPATH  "C:/MounRiver/MounRiver_Studio/toolchain/RISC-V Embedded GCC/bin/riscv-none-embed-")
#set(TOOLPATH  "C:/MounRiver/MounRiver_Studio/toolchain/RISC-V Embedded GCC12/bin/riscv-none-elf-")

    
set(CMAKE_C_COMPILER ${TOOLPATH}gcc.exe)
set(CMAKE_CXX_COMPILER ${TOOLPATH}g++.exe)
set(CMAKE_ASM_COMPILER ${TOOLPATH}gcc.exe)
set(CMAKE_AR ${TOOLPATH}ar.exe)
set(CMAKE_OBJCOPY ${TOOLPATH}objcopy.exe)
set(CMAKE_OBJDUMP ${TOOLPATH}objdump.exe)
set(SIZE ${TOOLPATH}size.exe)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)









