
# toolchain
TOOLCHAIN    = arm-none-eabi-
CC           = $(TOOLCHAIN)gcc
CP           = $(TOOLCHAIN)objcopy
AS           = $(TOOLCHAIN)gcc -x assembler-with-cpp
HEX          = $(CP) -O ihex
BIN          = $(CP) -O binary -S

# define mcu, specify the target processor
MCU          = cortex-m3

# all the files will be generated with this name (main.elf, main.bin, main.hex, etc)
PROJECT_NAME=music-box-stm32f1

# specify define
DDEFS       =

# define root dir
ROOT_DIR     = .

# define include dir
INCLUDE_DIRS =

# define stm32f10x lib dir
STM32F10x_LIB_DIR      = $(ROOT_DIR)/stm32f10x_lib

# define freertos dir
FREERTOS_DIR = $(ROOT_DIR)/freertos

# define user dir
USER_DIR     = $(ROOT_DIR)/user

# link file
LINK_SCRIPT  = $(ROOT_DIR)/stm32_flash.ld

# user specific
SRC       =
ASM_SRC  += $(USER_DIR)/Synth_m3.s
ASM_SRC  += $(USER_DIR)/PlayerUtils.s
SRC      += $(USER_DIR)/main.c
SRC      += $(USER_DIR)/SynthCore.c
SRC      += $(USER_DIR)/Player.c
SRC      += $(USER_DIR)/score.c
SRC      += $(USER_DIR)/EnvelopTable.c
SRC      += $(USER_DIR)/WaveTable_Celesta_C5.c
SRC      += $(USER_DIR)/WaveTable_Celesta_C6.c
SRC      += $(USER_DIR)/AlgorithmTest.c
SRC      += $(USER_DIR)/Hal/uart_log.c
SRC      += $(USER_DIR)/Hal/gcc-stdio-retarget.c
SRC      += $(USER_DIR)/Hal/stm32_it.c
SRC      += $(USER_DIR)/Hal/hal.c
SRC      += $(USER_DIR)/Usb/usb_desc.c
SRC      += $(USER_DIR)/Usb/usb_endp.c
SRC      += $(USER_DIR)/Usb/usb_istr.c
SRC      += $(USER_DIR)/Usb/usb_prop.c
SRC      += $(USER_DIR)/Usb/usb_pwr.c

# user include
INCLUDE_DIRS  = $(USER_DIR)
INCLUDE_DIRS  += $(USER_DIR)/Usb
INCLUDE_DIRS  += $(USER_DIR)/Hal

# include sub makefiles
include makefile_std_lib.mk   # STM32 Standard Peripheral Library
include makefile_freertos.mk  # freertos source

INC_DIR  = $(patsubst %, -I%, $(INCLUDE_DIRS))

# run from Flash
DEFS	 = $(DDEFS) -DRUN_FROM_FLASH=1

OBJECTS  = $(ASM_SRC:.s=.o) $(SRC:.c=.o)

DEPS = $(SRC:.c=.d) $(ASM_SRC:.s=.d)


# Define optimisation level here
OPT = -Os

MC_FLAGS = -mcpu=$(MCU)

AS_FLAGS = $(MC_FLAGS) -g -gdwarf-2 -mthumb  -Wa,-amhls=$(<:.s=.lst)
CP_FLAGS = $(MC_FLAGS) $(OPT) -fdata-sections -ffunction-sections -g -gdwarf-2 -mthumb -fomit-frame-pointer -Wall -fverbose-asm -Wa,-ahlms=$(<:.c=.lst) $(DEFS)
LD_FLAGS = $(MC_FLAGS) -g -specs=nano.specs -specs=nosys.specs -gdwarf-2 -mthumb -nostartfiles -Xlinker -T$(LINK_SCRIPT) -Wl,-Map=$(PROJECT_NAME).map,--cref,--no-warn-mismatch,--gc-sections
DEPFLAGS = -MT $@ -MMD -MP -MF $*.d
#
# makefile rules
#



all: $(OBJECTS) $(PROJECT_NAME).elf  $(PROJECT_NAME).hex $(PROJECT_NAME).bin
	$(TOOLCHAIN)size $(PROJECT_NAME).elf


# Don't delete dependency files
.PRECIOUS: %.d

# Don't rebuild deps if cleaning
ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)	
endif


%.o: %.c Makefile makefile_std_lib.mk
	@echo [CC] $(notdir $<)
	@$(CC) $(DEPFLAGS) -c $(CP_FLAGS) -I . $(INC_DIR) $< -o $@

%.o: %.s
	@echo [AS] $(notdir $<)
	@$(AS) $(DEPFLAGS) -c $(AS_FLAGS) $< -o $@

%.elf: $(OBJECTS)  $(LINK_SCRIPT)
	@echo [LD] $(PROJECT_NAME).elf
	@$(CC) $(OBJECTS) $(LD_FLAGS) -o $@

%.hex: %.elf
	@echo [HEX] $(PROJECT_NAME).hex
	@$(HEX) $< $@

%.bin: %.elf
	@echo [BIN] $(PROJECT_NAME).bin
	@$(BIN)  $< $@

flash: $(PROJECT_NAME).bin
	st-flash write $(PROJECT_NAME).bin 0x8000000

erase:
	st-flash erase

clean:
	@echo [RM] OBJ
	@-rm -rf $(OBJECTS)
	@echo [RM] BIN "(ELF,HEX,BIN)"
	@-rm -rf $(PROJECT_NAME).elf
	@-rm -rf $(PROJECT_NAME).hex
	@-rm -rf $(PROJECT_NAME).bin
	@echo [RM] LST
	@-rm -rf $(SRC:.c=.lst)
	@-rm -rf $(ASM_SRC:.s=.lst)
	@echo [RM] MAP
	@-rm -rf $(PROJECT_NAME).map
	@echo [RM] DEPENDENCY
	@-rm -rf $(DEPS)
