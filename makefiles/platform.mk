
OUT_DIR += /platform \
/platform/boot \
/platform/boot/8258 \
/platform/services/b85m \
/platform/tc32 \
/platform/chip_8258 \
/platform/chip_8258/flash

OBJS += \
$(BUILD_PATH)/platform/boot/link_cfg.o \
$(BUILD_PATH)/platform/services/b85m/irq_handler.o \
$(BUILD_PATH)/platform/tc32/div_mod.o 


# Each subdirectory must supply rules for building sources it contributes
$(BUILD_PATH)/platform/%.o: $(SDK_PATH)/platform/%.c
	@echo 'Building file: $<'
	@$(CC) $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"

$(BUILD_PATH)/platform/%.o: $(SDK_PATH)/platform/%.S
	@echo 'Building file: $<'
	@$(CC) $(GCC_FLAGS) $(ASM_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"