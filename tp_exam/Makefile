#!/usr/bin/make -f

include ../utils/config.mk
objects += tp.o


# Define local source directories
SRC_DIR    := src
INC_DIR    := include

# Add local includes to global includes
INCLUDE    += -I./$(INC_DIR)

# Add tp.c explicitly as the main entry point
objects    += tp.o

# Gather all source files from src directory
SRCS       := $(wildcard $(SRC_DIR)/*.c)
ASM_SRCS   := $(wildcard $(SRC_DIR)/*.s)

# Generate object file names for additional modules
LOCAL_OBJS := $(SRCS:$(SRC_DIR)/%.c=%.o)
LOCAL_OBJS += $(ASM_SRCS:$(SRC_DIR)/%.s=%.o)

# Add module objects to global objects
objects    += $(LOCAL_OBJS)

# Rules for tp.c (in root directory)
tp.o: tp.c
	@echo "    CC    $<"
	@$(CC) $(INCLUDE) $(CFLAGS) -o $@ -c $<

tp.d: tp.c
	@echo "    DP    $<"
	@$(CPP) -M -MG -MT '$(<:.c=.o)' $(INCLUDE) $(CFLAGS) $< | \
	$(SED) 's,\($*\)\.o[ :]*,\1.o $@ : ,' > $@

# Rules for module sources in src directory
%.o: $(SRC_DIR)/%.c
	@echo "    CC    $<"
	@$(CC) $(INCLUDE) $(CFLAGS) -o $@ -c $<

%.o: $(SRC_DIR)/%.s
	@echo "    AS    $<"
	@$(CPP) $< $(CFLAGS) -o $<.s
	@$(CC) $(CFLAGS) -o $@ -c $<.s
	@$(RM) $<.s

# Dependencies for module sources
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
