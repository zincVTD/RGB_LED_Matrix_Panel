# Toolchain
CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
OBJCOPY = arm-none-eabi-objcopy

# Flags
CFLAGS = -mcpu=cortex-m3 -mthumb -std=c11 -Wall -g -O0 \
    -Iinclude \
    -Ilib/SPL/inc \
    -Ilib/CMSIS/Core \
	-DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER \

LDFLAGS = -Tlinker.ld -nostartfiles -Wl,--gc-sections
LIBS = -lm -lc

# Sources
SRC = $(wildcard src/*.c) \
      $(wildcard lib/SPL/src/*.c)

STARTUP = startup/startup_stm32f10x_md.s

OBJ = $(SRC:.c=.o) $(STARTUP:.s=.o)
OUT = build/test.elf

# Rules
all: $(OUT)

$(OUT): $(OBJ)
	mkdir -p build
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(LDFLAGS) $(LIBS)
	$(OBJCOPY) -O ihex $@ build/test.hex
	$(OBJCOPY) -O binary $@ build/test.bin

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(AS) -mcpu=cortex-m3 -mthumb $< -o $@

clean:
	rm -rf src/*.o lib/SPL/src/*.o build/*
