# Variables
SRC_DIR := src
OBJ_DIR := obj
EXE := sim
SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
# Compiler Settings
CC := g++
CFLAGS := -g -c -Wall -Wextra -std=c++11 -Iinclude/ `pkg-config --cflags glib-2.0`
LIBS := `pkg-config --libs glib-2.0`

CONFS = $(wildcard configs/*.cfg)
PROGRAMS = $(wildcard programs/*.dat)
OUTPUTS := $(foreach conf,$(CONFS),$(foreach program, $(PROGRAMS), outputs/$(program:programs/%.dat=%).$(conf:configs/%.cfg=%).out))
OUTPUTS_SOLUTION := $(foreach conf,$(CONFS),$(foreach program, $(PROGRAMS), outputs_solution/$(program:programs/%.dat=%).$(conf:configs/%.cfg=%).out))
DIFFS := $(foreach conf,$(CONFS),$(foreach program, $(PROGRAMS), diffs/$(program:programs/%.dat=%).$(conf:configs/%.cfg=%).diff))

.PHONY: all clean

all: $(EXE) 

$(EXE): $(OBJ)
	$(CC)  $^ $(LIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp |$(OBJ_DIR)
	$(CC)  $(CFLAGS) -c  $< -o $@

-include $(OBJ:.o=.d)

run: $(OUTPUTS) $(OUTPUTS_SOLUTION) $(DIFFS)

define run_rules
outputs/$(1:programs/%.dat=%).$(2:configs/%.cfg=%).out: $(EXE) $(1) $(2)
	@echo "Running ./$(EXE) -p $(1) -c $(2) -j outputs/$(1:programs/%.dat=%).$(2:configs/%.cfg=%).json -d > $$@"
	-@./$(EXE) -p $(1) -c $(2) -j outputs/$(1:programs/%.dat=%).$(2:configs/%.cfg=%).json -d > $$@
endef
$(foreach program,$(PROGRAMS),$(foreach conf, $(CONFS), $(eval $(call run_rules,$(program),$(conf)))))

define diff_rules
diffs/$(1:programs/%.dat=%).$(2:configs/%.cfg=%).diff: outputs/$(1:programs/%.dat=%).$(2:configs/%.cfg=%).out
	@echo "Running diff -dwy -W 170 $$< outputs_solution/$(1:programs/%.dat=%).$(2:configs/%.cfg=%).out > $$@"
	-@diff -dwy -W 170 $$< outputs_solution/$(1:programs/%.dat=%).$(2:configs/%.cfg=%).out > $$@
endef
$(foreach program,$(PROGRAMS),$(foreach conf, $(CONFS), $(eval $(call diff_rules,$(program),$(conf)))))

clean:
	@$(RM) -rf $(OBJ_DIR)/* $(EXE) outputs/* diffs/*
