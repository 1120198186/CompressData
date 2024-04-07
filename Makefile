# Makefile for this project

# g++ test.cpp -o test.exe -fopenmp -l msmpi -L "D:\abuqiqi\AppData\Microsoft SDKs\MPI\Lib\x64" -I "D:\abuqiqi\AppData\Microsoft SDKs\MPI\Include"

# "D:\abuqiqi\AppData\Microsoft MPI\Bin\mpiexec.exe" -n 3 test.exe

# ---------------------	macros --------------------------

# compiler and flags
CC := g++
CCFLAGS := -std=c++11 -Wall -Werror

# directories
OBJ_DIR := obj
INC_DIR := util
SIM_DIR := simulator
SRC_DIR := main
OUT_DIR := output

INCFLAGS := -I./$(INC_DIR)/ -I./$(SIM_DIR)/

COMPILE := $(CC) $(CCFLAGS) $(INCFLAGS) -pthread 

# --------------------- utils ----------------------

UTIL_CPPS := $(INC_DIR)/*.cpp
UTIL_CPPS := $(wildcard $(UTIL_CPPS))

UTIL_OBJS := $(addprefix $(OBJ_DIR)/, $(patsubst %.cpp,%.o,$(UTIL_CPPS)))

# ------------------- simulators -------------------

SIM_CPPS := $(SIM_DIR)/*.cpp
SIM_CPPS := $(wildcard $(SIM_CPPS))

SIM_OBJS := $(addprefix $(OBJ_DIR)/, $(patsubst %.cpp,%.o,$(SIM_CPPS)))

# --------------------- main -----------------------

MAIN_CPPS := $(SRC_DIR)/main.cpp
MAIN_CPPS := $(wildcard $(MAIN_CPPS))

MAIN_OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%,$(MAIN_CPPS))

# -------------------- targets ---------------------

TARGETS := $(MAIN_OBJS)

.PHONY: all clean
.PRECIOUS: $(OBJ_DIR)/%.o

all: $(TARGETS)

# mkdir obj obj/util output
$(OBJ_DIR):
	mkdir $(OBJ_DIR)
	mkdir $(OBJ_DIR)\util
	mkdir $(OBJ_DIR)\simulator
	mkdir $(OUT_DIR)
# @-mkdir -p $(OBJ_DIR)
# @-mkdir -p $(dir $(UTIL_OBJS))
# @-mkdir -p $(OUT_DIR)

# utility functions: util/*.cpp -> obj/util/*.o
# simulator functions: simulator/*.cpp -> obj/simulator/*.o
$(OBJ_DIR)/%.o: %.cpp
	@echo "[INFO] Compiling" $< ...
	@$(COMPILE) -c $< -o $@

# executable files: main/*.cpp -> obj/*
$(OBJ_DIR)/%: $(OBJ_DIR) $(MAIN_CPPS) $(UTIL_OBJS) $(SIM_OBJS)
	@echo "[INFO] Linking" $@ ...
	@$(COMPILE) $(patsubst $(OBJ_DIR)/%,$(SRC_DIR)/%.cpp,$@) $(UTIL_OBJS) $(SIM_OBJS) -o $@
	@echo "[INFO]" $@ "has been built. "

# clean
clean:
# rm -rf $(OBJ_DIR)
	del /s /q $(OBJ_DIR)

cleanssd:
	del /s /q $(OUT_DIR)