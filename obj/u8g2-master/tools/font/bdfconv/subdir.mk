################################################################################
# MRS Version: 1.9.1
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../u8g2-master/tools/font/bdfconv/bdf_8x8.c \
../u8g2-master/tools/font/bdfconv/bdf_font.c \
../u8g2-master/tools/font/bdfconv/bdf_glyph.c \
../u8g2-master/tools/font/bdfconv/bdf_kern.c \
../u8g2-master/tools/font/bdfconv/bdf_map.c \
../u8g2-master/tools/font/bdfconv/bdf_parser.c \
../u8g2-master/tools/font/bdfconv/bdf_rle.c \
../u8g2-master/tools/font/bdfconv/bdf_tga.c \
../u8g2-master/tools/font/bdfconv/fd.c \
../u8g2-master/tools/font/bdfconv/main.c 

OBJS += \
./u8g2-master/tools/font/bdfconv/bdf_8x8.o \
./u8g2-master/tools/font/bdfconv/bdf_font.o \
./u8g2-master/tools/font/bdfconv/bdf_glyph.o \
./u8g2-master/tools/font/bdfconv/bdf_kern.o \
./u8g2-master/tools/font/bdfconv/bdf_map.o \
./u8g2-master/tools/font/bdfconv/bdf_parser.o \
./u8g2-master/tools/font/bdfconv/bdf_rle.o \
./u8g2-master/tools/font/bdfconv/bdf_tga.o \
./u8g2-master/tools/font/bdfconv/fd.o \
./u8g2-master/tools/font/bdfconv/main.o 

C_DEPS += \
./u8g2-master/tools/font/bdfconv/bdf_8x8.d \
./u8g2-master/tools/font/bdfconv/bdf_font.d \
./u8g2-master/tools/font/bdfconv/bdf_glyph.d \
./u8g2-master/tools/font/bdfconv/bdf_kern.d \
./u8g2-master/tools/font/bdfconv/bdf_map.d \
./u8g2-master/tools/font/bdfconv/bdf_parser.d \
./u8g2-master/tools/font/bdfconv/bdf_rle.d \
./u8g2-master/tools/font/bdfconv/bdf_tga.d \
./u8g2-master/tools/font/bdfconv/fd.d \
./u8g2-master/tools/font/bdfconv/main.d 


# Each subdirectory must supply rules for building sources it contributes
u8g2-master/tools/font/bdfconv/%.o: ../u8g2-master/tools/font/bdfconv/%.c
	@	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"C:\MRS_DATA\workspace\FreeModbusProjetc\Debug" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\port" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\modbus" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\modbus\functions" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\modbus\include" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\modbus\tcp" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\NetLib" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\Core" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\User" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\Peripheral\inc" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\FreeRTOS" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\FreeRTOS\include" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\FreeRTOS\portable\Common" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\FreeRTOS\portable\GCC\RISC-V" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\FreeRTOS\portable\GCC\RISC-V\chip_specific_extensions\RV32I_PFIC_no_extensions" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\FreeRTOS\portable\MemMang" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

