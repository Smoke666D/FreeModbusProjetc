################################################################################
# MRS Version: 1.9.1
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../u8g2-master/sys/arm/lpc804/lpc_chip_804/src/gpio.c \
../u8g2-master/sys/arm/lpc804/lpc_chip_804/src/i2c.c \
../u8g2-master/sys/arm/lpc804/lpc_chip_804/src/plu.c \
../u8g2-master/sys/arm/lpc804/lpc_chip_804/src/spi.c \
../u8g2-master/sys/arm/lpc804/lpc_chip_804/src/swm.c \
../u8g2-master/sys/arm/lpc804/lpc_chip_804/src/syscon.c \
../u8g2-master/sys/arm/lpc804/lpc_chip_804/src/system.c 

OBJS += \
./u8g2-master/sys/arm/lpc804/lpc_chip_804/src/gpio.o \
./u8g2-master/sys/arm/lpc804/lpc_chip_804/src/i2c.o \
./u8g2-master/sys/arm/lpc804/lpc_chip_804/src/plu.o \
./u8g2-master/sys/arm/lpc804/lpc_chip_804/src/spi.o \
./u8g2-master/sys/arm/lpc804/lpc_chip_804/src/swm.o \
./u8g2-master/sys/arm/lpc804/lpc_chip_804/src/syscon.o \
./u8g2-master/sys/arm/lpc804/lpc_chip_804/src/system.o 

C_DEPS += \
./u8g2-master/sys/arm/lpc804/lpc_chip_804/src/gpio.d \
./u8g2-master/sys/arm/lpc804/lpc_chip_804/src/i2c.d \
./u8g2-master/sys/arm/lpc804/lpc_chip_804/src/plu.d \
./u8g2-master/sys/arm/lpc804/lpc_chip_804/src/spi.d \
./u8g2-master/sys/arm/lpc804/lpc_chip_804/src/swm.d \
./u8g2-master/sys/arm/lpc804/lpc_chip_804/src/syscon.d \
./u8g2-master/sys/arm/lpc804/lpc_chip_804/src/system.d 


# Each subdirectory must supply rules for building sources it contributes
u8g2-master/sys/arm/lpc804/lpc_chip_804/src/%.o: ../u8g2-master/sys/arm/lpc804/lpc_chip_804/src/%.c
	@	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"C:\MRS_DATA\workspace\FreeModbusProjetc\Debug" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\port" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\modbus" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\modbus\functions" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\modbus\include" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\modbus\tcp" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\NetLib" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\Core" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\User" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\Peripheral\inc" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\FreeRTOS" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\FreeRTOS\include" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\FreeRTOS\portable\Common" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\FreeRTOS\portable\GCC\RISC-V" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\FreeRTOS\portable\GCC\RISC-V\chip_specific_extensions\RV32I_PFIC_no_extensions" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\FreeRTOS\portable\MemMang" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

