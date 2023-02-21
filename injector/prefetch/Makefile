OBJCOPY := arm-none-eabi-objcopy
AS := arm-none-eabi-as

ASFLAGS=-march=armv7-r \
       	-mcpu=cortex-r7 \
       	# -mthumb \

ifndef BUILD_DIR
BUILD_DIR=build
endif

all : $(BUILD_DIR) $(BUILD_DIR)/abort.o $(BUILD_DIR)/abort.bin

$(BUILD_DIR) :
	@echo "MKDIR $(BUILD_DIR)"
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o : %.S
	@echo "$(AS) $(ASFLAGS) -o $@ $<"
	@$(AS) $(ASFLAGS) -o $@ $<

$(BUILD_DIR)/%.bin : $(BUILD_DIR)/%.o
	@echo "$(OBJCOPY) -O binary $< $@"
	@$(OBJCOPY) -O binary $< $@

clean:
	@echo "CLEAN $(BUILD_DIR)"
	@rm -rf $(BUILD_DIR)

.PHONY: all clean