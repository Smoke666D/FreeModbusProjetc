################################################################################
# MRS Version: 1.9.1
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../u8g2-master/sys/sdl/gm2/item.c \
../u8g2-master/sys/sdl/gm2/main.c \
../u8g2-master/sys/sdl/gm2/mapgen.c \
../u8g2-master/sys/sdl/gm2/u8g2_d_tga.c \
../u8g2-master/sys/sdl/gm2/ugl_arrays.c \
../u8g2-master/sys/sdl/gm2/ugl_bc.c \
../u8g2-master/sys/sdl/gm2/ugl_error.c \
../u8g2-master/sys/sdl/gm2/ugl_main.c \
../u8g2-master/sys/sdl/gm2/ugl_parse.c 

OBJS += \
./u8g2-master/sys/sdl/gm2/item.o \
./u8g2-master/sys/sdl/gm2/main.o \
./u8g2-master/sys/sdl/gm2/mapgen.o \
./u8g2-master/sys/sdl/gm2/u8g2_d_tga.o \
./u8g2-master/sys/sdl/gm2/ugl_arrays.o \
./u8g2-master/sys/sdl/gm2/ugl_bc.o \
./u8g2-master/sys/sdl/gm2/ugl_error.o \
./u8g2-master/sys/sdl/gm2/ugl_main.o \
./u8g2-master/sys/sdl/gm2/ugl_parse.o 

C_DEPS += \
./u8g2-master/sys/sdl/gm2/item.d \
./u8g2-master/sys/sdl/gm2/main.d \
./u8g2-master/sys/sdl/gm2/mapgen.d \
./u8g2-master/sys/sdl/gm2/u8g2_d_tga.d \
./u8g2-master/sys/sdl/gm2/ugl_arrays.d \
./u8g2-master/sys/sdl/gm2/ugl_bc.d \
./u8g2-master/sys/sdl/gm2/ugl_error.d \
./u8g2-master/sys/sdl/gm2/ugl_main.d \
./u8g2-master/sys/sdl/gm2/ugl_parse.d 


# Each subdirectory must supply rules for building sources it contributes
u8g2-master/sys/sdl/gm2/%.o: ../u8g2-master/sys/sdl/gm2/%.c
	@	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"C:\MRS_DATA\workspace\FreeModbusProjetc\Debug" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\port" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\modbus" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\modbus\functions" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\modbus\include" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\modbus\tcp" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\NetLib" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\Core" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\User" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\Peripheral\inc" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\FreeRTOS" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\FreeRTOS\include" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\FreeRTOS\portable\Common" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\FreeRTOS\portable\GCC\RISC-V" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\FreeRTOS\portable\GCC\RISC-V\chip_specific_extensions\RV32I_PFIC_no_extensions" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\FreeRTOS\portable\MemMang" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

