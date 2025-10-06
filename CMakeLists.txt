set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)

cmake_minimum_required(VERSION 3.20)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/wch_gcc.cmake)

project(ch32v3_BOOT C CXX ASM)

# 编译参数 一般不用改
add_compile_options(-march=rv32imafcxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -fdump-rtl-expand -Os)
add_compile_options(-fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  )

                               


include_directories(USER
		    HAL_LIB
		    HAL_DRIVER
		    FreeRTOS/include
                    Core
                    Debug 
		    port
	            modbus/tcp
		    modbus/rtu
                    modbus/include
	            NetLib
		    u8g2-master/csrc	
		    FreeRTOS/portable/MemMang
		    FreeRTOS/portable/GCC/RISC-V
		    FreeRTOS/portable/GCC/RISC-V/chip_specific_extensions/RV32I_PFIC_no_extensions
		    FreeRTOS
                    Peripheral/inc)

 link_directories(NetLib)
# 宏定义
# add_definitions(-DDEBUG=1)


# 源码文件
file(GLOB_RECURSE SOURCES
		"FreeRTOS/*.c"
	        "FreeRTOS/portable/MemMang/*.c"
		"FreeRTOS/por`table/GCC/RISC-V/*.c"
		"FreeRTOS/portable/GCC/RISC-V/*.s"
		"u8g2-master/csrc/*.c"
                "USER/*.c"
		"HAL_LIB/*.c"
	        "HAL_DRIVER/*.c"
                "Core/core_riscv.c" 
                "Debug/debug.c"
                "Peripheral/src/*.c"
		"modbus/*.c" 
		"modbus/tcp/*.c"
		"modbus/rtu/*.c"
		"modbus/functions/*.c"
		"port/*.c"
		"NetLib/*.c"        
                "Startup/startup_ch32v30x_D8C.S"
                )


#add_library(wchnet STATIC  "libwchnet.a")

list(FILTER SOURCES EXCLUDE REGEX "hal_can_ch32.c")
list(FILTER SOURCES EXCLUDE REGEX "mpu_wrappers.c")
list(FILTER SOURCES EXCLUDE REGEX "eth_driver_MII.c")
list(FILTER SOURCES EXCLUDE REGEX "eth_driver_RGMII.c")
list(FILTER SOURCES EXCLUDE REGEX "eth_driver_RMII.c")


set(LINKER_SCRIPT  ${CMAKE_SOURCE_DIR}/Ld/Link.ld)
add_link_options(
                -march=rv32imafcxw 
		-mabi=ilp32
	        -nostartfiles 
                -Xlinker --gc-sections  
                -Wl,--print-memory-usage
                -Wl,-Map,${PROJECT_NAME}.map
 		-u _printf_float 
                --specs=nano.specs 
                --specs=nosys.specs
		)
add_link_options(-T ${LINKER_SCRIPT} )



# 编译可执行文件
add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})
target_link_libraries(${PROJECT_NAME}.elf PUBLIC m)
target_link_libraries(${PROJECT_NAME}.elf PRIVATE wchnet)


# 输出hex和bin
set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)
set(LST_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.lst)
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMAND ${CMAKE_OBJDUMP} --all-headers --demangle --disassemble $<TARGET_FILE:${PROJECT_NAME}.elf> > ${LST_FILE}
        COMMAND ${SIZE} --format=berkeley $<TARGET_FILE:${PROJECT_NAME}.elf>
)
