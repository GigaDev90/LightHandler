include ./compile.mk

# XC8 toolchain
BIN = $(XC8DIR)/bin
LIB = $(XC8DIR)/lib
CC = $(BIN)/xc8

# pk2cmd
PK2 = $(PK2DIR)/pk2cmd
PK2CMD = $(PK2)/pk2cmd

# Project settings
SOURCEFILES = main.c
OUTPUT_FILE = LightHandler
TARGET_CHIP = 16F690
CFLAGS = -Q --CHIP=$(TARGET_CHIP) --OPT=all

# Build files
DIR = build
P1_FILES = $(SOURCEFILES:%.c=%.p1)
DEP_FILES = $(P1_FILES:%.p1=%.deps)
AS_FILE = $(OUTPUT_FILE).as
HEX_FILE = $(OUTPUT_FILE).hex

.PHONY: all program dis check clean

all: $(DIR)/$(HEX_FILE)

# Load dependencies on #include'd files, if they exist (i.e. if we have already
# compiled at least once)
-include $(addprefix $(DIR)/,$(DEP_FILES))

# Build rules
$(DIR)/%.p1: %.c
	@echo "=> Generating $*.p1..."
	@$(CC) $(CFLAGS) --PASS1 $< -O$@
	@# Emulate GCC's -MP flag while creating the dependency list
	@(cat $(DIR)/$*.d; echo; for dep in `perl -pe 's/^.*?:.*?\.c//' $(DIR)/$*.d`; do echo "$$dep:"; done) > $(DIR)/$*.deps

$(DIR)/$(AS_FILE) $(DIR)/$(HEX_FILE): $(addprefix $(DIR)/,$(P1_FILES))
	@echo "=> Compiling $(HEX_FILE)..."
	@(cd $(DIR); $(CC) $(CFLAGS) $(P1_FILES) -O$(HEX_FILE))

program: $(DIR)/$(HEX_FILE)
	@echo "=> Programming $(HEX_FILE)..."
	sudo $(PK2CMD) -B$(PK2) -PPIC$(TARGET_CHIP) -F$(DIR)/$(HEX_FILE) -M -R -T

dis: $(DIR)/$(AS_FILE)
	@cat $(DIR)/$(AS_FILE)

check:
	sudo $(PK2CMD) -B$(PK2) -I -P

clean:
	@echo "=> Cleaning build files..."
	@-$(RM) $(DIR)/*
