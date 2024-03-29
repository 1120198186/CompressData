# Makefile for this project

# ---------------------	macros --------------------------

# compiler and flags
CC := mpic++
CCFLAGS := -std=c++11 -Wall -Werror

# directories
OBJ_DIR := obj
INC_DIR := util
SRC_DIR := simulator
OUT_DIR := output

COMPILE := $(CC) $(CCFLAGS) -I./$(INC_DIR)/ -pthread

# --------------------- utils ----------------------

UTIL_CPPS := $(INC_DIR)/*.cpp
UTIL_CPPS := $(wildcard $(UTIL_CPPS))

UTIL_OBJS := $(addprefix $(OBJ_DIR)/, $(patsubst %.cpp,%.o,$(UTIL_CPPS)))

# ------------------- simulators -------------------

SIMULATOR_CPPS := $(SRC_DIR)/*.cpp
SIMULATOR_CPPS := $(wildcard $(SIMULATOR_CPPS))

SIMULATOR_OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%,$(SIMULATOR_CPPS))

# -------------------- targets ---------------------

TARGETS := $(SIMULATOR_OBJS) $(OBJ_DIR)/QuanPath

.PHONY: all clean
# .PRECIOUS: $(OBJ_DIR)/%.o

all: $(TARGETS)

# mkdir obj obj/util output
$(OBJ_DIR):
	@-mkdir -p $(OBJ_DIR)
	@-mkdir -p $(dir $(UTIL_OBJS))
	@-mkdir -p $(OUT_DIR)

# utility functions: util/*.cpp -> obj/util/*.o
$(OBJ_DIR)/%.o: %.cpp
	@echo "[INFO] Compiling" $< ...
	@$(COMPILE) -c $< -o $@

# executable files: simulator/*.cpp -> obj/*
$(OBJ_DIR)/%: $(OBJ_DIR) $(SIMULATOR_CPPS) $(UTIL_OBJS)
	@echo "[INFO] Linking" $@ ...
	@$(COMPILE) $(patsubst $(OBJ_DIR)/%,$(SRC_DIR)/%.cpp,$@) $(UTIL_OBJS) -o $@
	@echo "[INFO]" $@ "has been built. "

$(OBJ_DIR)/QuanPath: $(OBJ_DIR)/QuanTrans
	@cp $< $@
	@echo "[INFO]" $@ "has been built. "

# clean
clean:
	rm -rf $(OBJ_DIR)
