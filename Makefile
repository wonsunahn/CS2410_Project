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

.PHONY: all clean

all: $(EXE) 

$(EXE): $(OBJ)
	$(CC)  $^ $(LIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp |$(OBJ_DIR)
	$(CC)  $(CFLAGS) -c  $< -o $@

$(OBJ_DIR):
	mkdir -p $@

run: $(EXE)
	./$(EXE) ./examples/prog.dat config/baseline.cfg

-include $(OBJ:.o=.d)

clean:
	@$(RM) -rv $(OBJ_DIR) $(EXE)
