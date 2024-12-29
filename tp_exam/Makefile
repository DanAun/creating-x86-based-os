#!/usr/bin/make -f

include ../utils/config.mk
objects += tp.o


# Define local source directories
SRC_DIR    := src
INC_DIR    := include

# Add local includes to global includes
INCLUDE    += -I./$(INC_DIR)

# Gather all source files
SRCS       := $(wildcard $(SRC_DIR)/*.c)
ASM_SRCS   := $(wildcard $(SRC_DIR)/*.s)

# Generate object file names
LOCAL_OBJS := $(SRCS:$(SRC_DIR)/%.c=%.o)
LOCAL_OBJS += $(ASM_SRCS:$(SRC_DIR)/%.s=%.o)

# Add them to global objects
objects    += $(LOCAL_OBJS)

# Override implicit rules to look for sources in SRC_DIR
%.o: $(SRC_DIR)/%.c
	@echo "    CC    $<"
	@$(CC) $(INCLUDE) $(CFLAGS) -o $@ -c $<

%.o: $(SRC_DIR)/%.s
	@echo "    AS    $<"
	@$(CPP) $< $(CFLAGS) -o $<.s
	@$(CC) $(CFLAGS) -o $@ -c $<.s
	@$(RM) $<.s

# Generate dependencies
%.d: $(SRC_DIR)/%.c
	@echo "    DP    $<"
	@$(CPP) -M -MG -MT '$(<:.c=.o)' $(INCLUDE) $(CFLAGS) $< | \
	$(SED) 's,\($*\)\.o[ :]*,\1.o $@ : ,' > $@

%.d: $(SRC_DIR)/%.s
	@echo "    DP    $<"
	@$(CPP) -M -MG -MT '$(<:.s=.o)' $(INCLUDE) $(CFLAGS) $< | \
	$(SED) 's,\($*\)\.o[ :]*,\1.o $@ : ,' > $@

include ../utils/rules.mk
#-include $(dependencies)
