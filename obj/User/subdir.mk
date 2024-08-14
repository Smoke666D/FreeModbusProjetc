################################################################################
# MRS Version: 1.9.2
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/AC_ConvertLib.c \
../User/ADC.c \
../User/adc_task.c \
../User/ch32v30x_it.c \
../User/data_model.c \
../User/din_dout_task.c \
../User/drivers.c \
../User/init.c \
../User/led.c \
../User/main.c \
../User/mb_task.c \
../User/menu.c \
../User/menu_data.c \
../User/rtc_task.c \
../User/system_ch32v30x.c \
../User/system_init.c 

OBJS += \
./User/AC_ConvertLib.o \
./User/ADC.o \
./User/adc_task.o \
./User/ch32v30x_it.o \
./User/data_model.o \
./User/din_dout_task.o \
./User/drivers.o \
./User/init.o \
./User/led.o \
./User/main.o \
./User/mb_task.o \
./User/menu.o \
./User/menu_data.o \
./User/rtc_task.o \
./User/system_ch32v30x.o \
./User/system_init.o 

C_DEPS += \
./User/AC_ConvertLib.d \
./User/ADC.d \
./User/adc_task.d \
./User/ch32v30x_it.d \
./User/data_model.d \
./User/din_dout_task.d \
./User/drivers.d \
./User/init.d \
./User/led.d \
./User/main.d \
./User/mb_task.d \
./User/menu.d \
./User/menu_data.d \
./User/rtc_task.d \
./User/system_ch32v30x.d \
./User/system_init.d 


# Each subdirectory must supply rules for building sources it contributes
User/%.o: ../User/%.c
	@	@	riscv-none-embed-gcc -march=rv32imafcxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"C:\MRS_DATA\workspace\FreeModbusProjetc\Debug" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\u8g2-master\csrc" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\port" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\modbus" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\modbus\functions" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\modbus\include" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\modbus\tcp" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\NetLib" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\Core" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\User" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\Peripheral\inc" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\FreeRTOS" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\FreeRTOS\include" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\FreeRTOS\portable\Common" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\FreeRTOS\portable\GCC\RISC-V" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\FreeRTOS\portable\GCC\RISC-V\chip_specific_extensions\RV32I_PFIC_no_extensions" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\FreeRTOS\portable\MemMang" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\HAL" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\HW_LIB" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

