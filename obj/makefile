################################################################################
# MRS Version: 1.9.1
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include u8g2-master/csrc/subdir.mk
-include port/subdir.mk
-include modbus/tcp/subdir.mk
-include modbus/functions/subdir.mk
-include modbus/subdir.mk
-include User/subdir.mk
-include Startup/subdir.mk
-include Peripheral/src/subdir.mk
-include NetLib/subdir.mk
-include HW_LIB/subdir.mk
-include HAL/subdir.mk
-include FreeRTOS/portable/MemMang/subdir.mk
-include FreeRTOS/portable/GCC/RISC-V/subdir.mk
-include FreeRTOS/subdir.mk
-include Debug/subdir.mk
-include Core/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
FreeModbus.hex \

SECONDARY_LIST += \
FreeModbus.lst \

SECONDARY_SIZE += \
FreeModbus.siz \


# All Target
all: FreeModbus.elf secondary-outputs

# Tool invocations
FreeModbus.elf: $(OBJS) $(USER_OBJS)
	@	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -T "C:\MRS_DATA\workspace\FreeModbusProjetc\Ld\Link.ld" -nostartfiles -Xlinker --gc-sections -L"C:\MRS_DATA\workspace\FreeModbusProjetc\NetLib" -Wl,-Map,"FreeModbus.map" --specs=nano.specs --specs=nosys.specs -o "FreeModbus.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@	@
FreeModbus.hex: FreeModbus.elf
	@	riscv-none-embed-objcopy -O ihex "FreeModbus.elf"  "FreeModbus.hex"
	@	@
FreeModbus.lst: FreeModbus.elf
	@	riscv-none-embed-objdump --all-headers --demangle --disassemble -M xw "FreeModbus.elf" > "FreeModbus.lst"
	@	@
FreeModbus.siz: FreeModbus.elf
	@	riscv-none-embed-size --format=berkeley "FreeModbus.elf"
	@	@
# Other Targets
clean:
	-$(RM) $(ASM_UPPER_DEPS)$(OBJS)$(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_DEPS)$(S_UPPER_DEPS)$(C_DEPS) FreeModbus.elf
	-@
secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
