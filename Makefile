# Toolchain
CC = arm-none-eabi-gcc
AS = arm-none-eabi-as

# Flags
CFLAGS = -mcpu=cortex-m3 -mthumb -std=c11 -Wall -g -O0 \
    -Isystem/inc \
    -Ibsp/inc \
    -Idriver/inc \
	-DSTM32F10X_MD \
	-DUSE_STDPERIPH_DRIVER

LDFLAGS = -Tlinker.ld -nostartfiles -Wl,--gc-sections
LIBS = -lm -lc

# Sources
BUILD_DIR = build

APP = $(wildcard app/src/*.c)
DRIVER = $(wildcard driver/src/*.c)
BSP = $(wildcard bsp/src/*.c)
SYSTEM = $(wildcard system/src/*.c)
STARTUP = system/src/startup_stm32f10x_md.s

OBJ = $(APP:%.c=$(BUILD_DIR)/%.o) \
      $(STARTUP:%.s=$(BUILD_DIR)/%.o) \
	  $(DRIVER:%.c=$(BUILD_DIR)/%.o) \
	  $(BSP:%.c=$(BUILD_DIR)/%.o) \
	  $(SYSTEM:%.c=$(BUILD_DIR)/%.o)

OUT = ${BUILD_DIR}/app.elf

# Rules
all: $(OUT)
	@echo "Firmware size:"
	@arm-none-eabi-size $(OUT)
	@echo "=================================="
	@echo "BUILD SUCCESS"
	@echo "Output: $(OUT)"
	@echo "=================================="

$(OUT): $(OBJ)
	mkdir -p build
	@echo "🔧 Linking..."
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(LDFLAGS) $(LIBS)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(AS) -mcpu=cortex-m3 -mthumb $< -o $@

flash: $(OUT)
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program $(OUT) verify reset exit"

clean:
	rm -rf build
