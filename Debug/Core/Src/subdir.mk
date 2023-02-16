################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/adc.c \
../Core/Src/adjust.c \
../Core/Src/analog.c \
../Core/Src/clrc663.c \
../Core/Src/consumable.c \
../Core/Src/dma.c \
../Core/Src/filterslide.c \
../Core/Src/gpio.c \
../Core/Src/grinder.c \
../Core/Src/hopper.c \
../Core/Src/iwdg.c \
../Core/Src/main.c \
../Core/Src/recipe.c \
../Core/Src/rfid.c \
../Core/Src/scale.c \
../Core/Src/spi.c \
../Core/Src/stm32l1xx_hal_msp.c \
../Core/Src/stm32l1xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32l1xx.c \
../Core/Src/tim.c \
../Core/Src/usart.c \
../Core/Src/work.c 

OBJS += \
./Core/Src/adc.o \
./Core/Src/adjust.o \
./Core/Src/analog.o \
./Core/Src/clrc663.o \
./Core/Src/consumable.o \
./Core/Src/dma.o \
./Core/Src/filterslide.o \
./Core/Src/gpio.o \
./Core/Src/grinder.o \
./Core/Src/hopper.o \
./Core/Src/iwdg.o \
./Core/Src/main.o \
./Core/Src/recipe.o \
./Core/Src/rfid.o \
./Core/Src/scale.o \
./Core/Src/spi.o \
./Core/Src/stm32l1xx_hal_msp.o \
./Core/Src/stm32l1xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32l1xx.o \
./Core/Src/tim.o \
./Core/Src/usart.o \
./Core/Src/work.o 

C_DEPS += \
./Core/Src/adc.d \
./Core/Src/adjust.d \
./Core/Src/analog.d \
./Core/Src/clrc663.d \
./Core/Src/consumable.d \
./Core/Src/dma.d \
./Core/Src/filterslide.d \
./Core/Src/gpio.d \
./Core/Src/grinder.d \
./Core/Src/hopper.d \
./Core/Src/iwdg.d \
./Core/Src/main.d \
./Core/Src/recipe.d \
./Core/Src/rfid.d \
./Core/Src/scale.d \
./Core/Src/spi.d \
./Core/Src/stm32l1xx_hal_msp.d \
./Core/Src/stm32l1xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32l1xx.d \
./Core/Src/tim.d \
./Core/Src/usart.d \
./Core/Src/work.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L151xC -c -I../Core/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc -I../Drivers/STM32L1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L1xx/Include -I../Drivers/CMSIS/Include -I"../../nlccoffee/Core shared/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/adc.d ./Core/Src/adc.o ./Core/Src/adc.su ./Core/Src/adjust.d ./Core/Src/adjust.o ./Core/Src/adjust.su ./Core/Src/analog.d ./Core/Src/analog.o ./Core/Src/analog.su ./Core/Src/clrc663.d ./Core/Src/clrc663.o ./Core/Src/clrc663.su ./Core/Src/consumable.d ./Core/Src/consumable.o ./Core/Src/consumable.su ./Core/Src/dma.d ./Core/Src/dma.o ./Core/Src/dma.su ./Core/Src/filterslide.d ./Core/Src/filterslide.o ./Core/Src/filterslide.su ./Core/Src/gpio.d ./Core/Src/gpio.o ./Core/Src/gpio.su ./Core/Src/grinder.d ./Core/Src/grinder.o ./Core/Src/grinder.su ./Core/Src/hopper.d ./Core/Src/hopper.o ./Core/Src/hopper.su ./Core/Src/iwdg.d ./Core/Src/iwdg.o ./Core/Src/iwdg.su ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/recipe.d ./Core/Src/recipe.o ./Core/Src/recipe.su ./Core/Src/rfid.d ./Core/Src/rfid.o ./Core/Src/rfid.su ./Core/Src/scale.d ./Core/Src/scale.o ./Core/Src/scale.su ./Core/Src/spi.d ./Core/Src/spi.o ./Core/Src/spi.su ./Core/Src/stm32l1xx_hal_msp.d ./Core/Src/stm32l1xx_hal_msp.o ./Core/Src/stm32l1xx_hal_msp.su ./Core/Src/stm32l1xx_it.d ./Core/Src/stm32l1xx_it.o ./Core/Src/stm32l1xx_it.su ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32l1xx.d ./Core/Src/system_stm32l1xx.o ./Core/Src/system_stm32l1xx.su ./Core/Src/tim.d ./Core/Src/tim.o ./Core/Src/tim.su ./Core/Src/usart.d ./Core/Src/usart.o ./Core/Src/usart.su ./Core/Src/work.d ./Core/Src/work.o ./Core/Src/work.su

.PHONY: clean-Core-2f-Src

