################################################################################
# MRS Version: 1.9.1
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../HAL/hal_adc.c \
../HAL/hal_dma.c \
../HAL/hal_gpio_ch32.c \
../HAL/hal_irq.c \
../HAL/hal_spi_ch32.c \
../HAL/hal_timers.c 

OBJS += \
./HAL/hal_adc.o \
./HAL/hal_dma.o \
./HAL/hal_gpio_ch32.o \
./HAL/hal_irq.o \
./HAL/hal_spi_ch32.o \
./HAL/hal_timers.o 

C_DEPS += \
./HAL/hal_adc.d \
./HAL/hal_dma.d \
./HAL/hal_gpio_ch32.d \
./HAL/hal_irq.d \
./HAL/hal_spi_ch32.d \
./HAL/hal_timers.d 


# Each subdirectory must supply rules for building sources it contributes
HAL/%.o: ../HAL/%.c
	@	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"C:\MRS_DATA\workspace\FreeModbusProjetc\Debug" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\u8g2-master\csrc" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\port" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\modbus" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\modbus\functions" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\modbus\include" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\modbus\tcp" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\NetLib" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\Core" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\User" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\Peripheral\inc" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\FreeRTOS" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\FreeRTOS\include" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\FreeRTOS\portable\Common" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\FreeRTOS\portable\GCC\RISC-V" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\FreeRTOS\portable\GCC\RISC-V\chip_specific_extensions\RV32I_PFIC_no_extensions" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\FreeRTOS\portable\MemMang" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\HAL" -I"C:\MRS_DATA\workspace\FreeModbusProjetc\HW_LIB" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

