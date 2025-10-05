################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/adc.c \
../src/led.c \
../src/main.c \
../src/software_timer.c 

OBJS += \
./src/adc.o \
./src/led.o \
./src/main.o \
./src/software_timer.o 

C_DEPS += \
./src/adc.d \
./src/led.d \
./src/main.d \
./src/software_timer.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/adc.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


