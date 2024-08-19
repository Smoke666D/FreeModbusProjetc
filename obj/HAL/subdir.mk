################################################################################
# MRS Version: 1.9.2
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../HAL/hal_adc.c \
../HAL/hal_dac.c \
../HAL/hal_dma.c \
../HAL/hal_gpio_ch32.c \
../HAL/hal_i2c_ch32.c \
../HAL/hal_irq.c \
../HAL/hal_rtc_ch32.c \
../HAL/hal_spi_ch32.c \
../HAL/hal_timers.c \
../HAL/hal_usart.c \
../HAL/hal_wdt_ch32.c 

OBJS += \
./HAL/hal_adc.o \
./HAL/hal_dac.o \
./HAL/hal_dma.o \
./HAL/hal_gpio_ch32.o \
./HAL/hal_i2c_ch32.o \
./HAL/hal_irq.o \
./HAL/hal_rtc_ch32.o \
./HAL/hal_spi_ch32.o \
./HAL/hal_timers.o \
./HAL/hal_usart.o \
./HAL/hal_wdt_ch32.o 

C_DEPS += \
./HAL/hal_adc.d \
./HAL/hal_dac.d \
./HAL/hal_dma.d \
./HAL/hal_gpio_ch32.d \
./HAL/hal_i2c_ch32.d \
./HAL/hal_irq.d \
./HAL/hal_rtc_ch32.d \
./HAL/hal_spi_ch32.d \
./HAL/hal_timers.d \
./HAL/hal_usart.d \
./HAL/hal_wdt_ch32.d 


# Each subdirectory must supply rules for building sources it contributes
HAL/%.o: ../HAL/%.c
	@	@	riscv-none-embed-gcc -march=rv32imafcxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"C:\Users\smoke\Documents\FreeModbusProjetc\Debug" -I"C:\Users\smoke\Documents\FreeModbusProjetc\u8g2-master\csrc" -I"C:\Users\smoke\Documents\FreeModbusProjetc\port" -I"C:\Users\smoke\Documents\FreeModbusProjetc\modbus" -I"C:\Users\smoke\Documents\FreeModbusProjetc\modbus\functions" -I"C:\Users\smoke\Documents\FreeModbusProjetc\modbus\include" -I"C:\Users\smoke\Documents\FreeModbusProjetc\modbus\tcp" -I"C:\Users\smoke\Documents\FreeModbusProjetc\modbus\rtu" -I"C:\Users\smoke\Documents\FreeModbusProjetc\NetLib" -I"C:\Users\smoke\Documents\FreeModbusProjetc\Core" -I"C:\Users\smoke\Documents\FreeModbusProjetc\User" -I"C:\Users\smoke\Documents\FreeModbusProjetc\Peripheral\inc" -I"C:\Users\smoke\Documents\FreeModbusProjetc\FreeRTOS" -I"C:\Users\smoke\Documents\FreeModbusProjetc\FreeRTOS\include" -I"C:\Users\smoke\Documents\FreeModbusProjetc\FreeRTOS\portable\Common" -I"C:\Users\smoke\Documents\FreeModbusProjetc\FreeRTOS\portable\GCC\RISC-V" -I"C:\Users\smoke\Documents\FreeModbusProjetc\FreeRTOS\portable\GCC\RISC-V\chip_specific_extensions\RV32I_PFIC_no_extensions" -I"C:\Users\smoke\Documents\FreeModbusProjetc\FreeRTOS\portable\MemMang" -I"C:\Users\smoke\Documents\FreeModbusProjetc\HAL" -I"C:\Users\smoke\Documents\FreeModbusProjetc\HW_LIB" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

