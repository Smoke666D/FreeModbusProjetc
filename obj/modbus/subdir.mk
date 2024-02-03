################################################################################
# MRS Version: 1.9.1
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../modbus/mb.c 

OBJS += \
./modbus/mb.o 

C_DEPS += \
./modbus/mb.d 


# Each subdirectory must supply rules for building sources it contributes
modbus/%.o: ../modbus/%.c
	@	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"C:\MRS_DATA\workspace\FreeModbus\Debug" -I"C:\MRS_DATA\workspace\FreeModbus\port" -I"C:\MRS_DATA\workspace\FreeModbus\modbus" -I"C:\MRS_DATA\workspace\FreeModbus\modbus\functions" -I"C:\MRS_DATA\workspace\FreeModbus\modbus\include" -I"C:\MRS_DATA\workspace\FreeModbus\modbus\tcp" -I"C:\MRS_DATA\workspace\FreeModbus\NetLib" -I"C:\MRS_DATA\workspace\FreeModbus\Core" -I"C:\MRS_DATA\workspace\FreeModbus\User" -I"C:\MRS_DATA\workspace\FreeModbus\Peripheral\inc" -I"C:\MRS_DATA\workspace\FreeModbus\FreeRTOS" -I"C:\MRS_DATA\workspace\FreeModbus\FreeRTOS\include" -I"C:\MRS_DATA\workspace\FreeModbus\FreeRTOS\portable\Common" -I"C:\MRS_DATA\workspace\FreeModbus\FreeRTOS\portable\GCC\RISC-V" -I"C:\MRS_DATA\workspace\FreeModbus\FreeRTOS\portable\GCC\RISC-V\chip_specific_extensions\RV32I_PFIC_no_extensions" -I"C:\MRS_DATA\workspace\FreeModbus\FreeRTOS\portable\MemMang" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

