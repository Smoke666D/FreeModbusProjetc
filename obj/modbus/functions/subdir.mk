################################################################################
# MRS Version: 1.9.1
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
	@	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"C:\MRS_DATA\workspace\FreeModbus\Debug" -I"C:\MRS_DATA\workspace\FreeModbus\port" -I"C:\MRS_DATA\workspace\FreeModbus\modbus" -I"C:\MRS_DATA\workspace\FreeModbus\modbus\functions" -I"C:\MRS_DATA\workspace\FreeModbus\modbus\include" -I"C:\MRS_DATA\workspace\FreeModbus\modbus\tcp" -I"C:\MRS_DATA\workspace\FreeModbus\NetLib" -I"C:\MRS_DATA\workspace\FreeModbus\Core" -I"C:\MRS_DATA\workspace\FreeModbus\User" -I"C:\MRS_DATA\workspace\FreeModbus\Peripheral\inc" -I"C:\MRS_DATA\workspace\FreeModbus\FreeRTOS" -I"C:\MRS_DATA\workspace\FreeModbus\FreeRTOS\include" -I"C:\MRS_DATA\workspace\FreeModbus\FreeRTOS\portable\Common" -I"C:\MRS_DATA\workspace\FreeModbus\FreeRTOS\portable\GCC\RISC-V" -I"C:\MRS_DATA\workspace\FreeModbus\FreeRTOS\portable\GCC\RISC-V\chip_specific_extensions\RV32I_PFIC_no_extensions" -I"C:\MRS_DATA\workspace\FreeModbus\FreeRTOS\portable\MemMang" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

