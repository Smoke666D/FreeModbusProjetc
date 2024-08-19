################################################################################
# MRS Version: 1.9.2
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../modbus/functions/mbfunccoils.c \
../modbus/functions/mbfuncdiag.c \
../modbus/functions/mbfuncdisc.c \
../modbus/functions/mbfuncholding.c \
../modbus/functions/mbfuncinput.c \
../modbus/functions/mbfuncother.c \
../modbus/functions/mbutils.c 

OBJS += \
./modbus/functions/mbfunccoils.o \
./modbus/functions/mbfuncdiag.o \
./modbus/functions/mbfuncdisc.o \
./modbus/functions/mbfuncholding.o \
./modbus/functions/mbfuncinput.o \
./modbus/functions/mbfuncother.o \
./modbus/functions/mbutils.o 

C_DEPS += \
./modbus/functions/mbfunccoils.d \
./modbus/functions/mbfuncdiag.d \
./modbus/functions/mbfuncdisc.d \
./modbus/functions/mbfuncholding.d \
./modbus/functions/mbfuncinput.d \
./modbus/functions/mbfuncother.d \
./modbus/functions/mbutils.d 


# Each subdirectory must supply rules for building sources it contributes
modbus/functions/%.o: ../modbus/functions/%.c
	@	@	riscv-none-embed-gcc -march=rv32imafcxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"C:\Users\smoke\Documents\FreeModbusProjetc\Debug" -I"C:\Users\smoke\Documents\FreeModbusProjetc\u8g2-master\csrc" -I"C:\Users\smoke\Documents\FreeModbusProjetc\port" -I"C:\Users\smoke\Documents\FreeModbusProjetc\modbus" -I"C:\Users\smoke\Documents\FreeModbusProjetc\modbus\functions" -I"C:\Users\smoke\Documents\FreeModbusProjetc\modbus\include" -I"C:\Users\smoke\Documents\FreeModbusProjetc\modbus\tcp" -I"C:\Users\smoke\Documents\FreeModbusProjetc\modbus\rtu" -I"C:\Users\smoke\Documents\FreeModbusProjetc\NetLib" -I"C:\Users\smoke\Documents\FreeModbusProjetc\Core" -I"C:\Users\smoke\Documents\FreeModbusProjetc\User" -I"C:\Users\smoke\Documents\FreeModbusProjetc\Peripheral\inc" -I"C:\Users\smoke\Documents\FreeModbusProjetc\FreeRTOS" -I"C:\Users\smoke\Documents\FreeModbusProjetc\FreeRTOS\include" -I"C:\Users\smoke\Documents\FreeModbusProjetc\FreeRTOS\portable\Common" -I"C:\Users\smoke\Documents\FreeModbusProjetc\FreeRTOS\portable\GCC\RISC-V" -I"C:\Users\smoke\Documents\FreeModbusProjetc\FreeRTOS\portable\GCC\RISC-V\chip_specific_extensions\RV32I_PFIC_no_extensions" -I"C:\Users\smoke\Documents\FreeModbusProjetc\FreeRTOS\portable\MemMang" -I"C:\Users\smoke\Documents\FreeModbusProjetc\HAL" -I"C:\Users\smoke\Documents\FreeModbusProjetc\HW_LIB" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

