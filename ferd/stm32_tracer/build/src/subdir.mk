
# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/main.c \
../src/ili9341.c \
../src/uart.c \
../src/timer.c \
../src/mini-printf.c \
../src/adc.c \
../src/keys.c \
../src/i2c.c

OBJS += \
./src/main.o \
./src/ili9341.o \
./src/uart.o \
./src/timer.o \
./src/mini-printf.o \
./src/adc.o \
./src/keys.o \
./src/i2c.o

# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc $(CFLAGS) $(INCS) -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


