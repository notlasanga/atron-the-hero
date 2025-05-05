# Usage:
# make          # compile for debug
# make run      # run debug executable
# make clean    # remove ALL binaries and objects

EXECUTABLE_NAME := atron_the_hero

SRC     := src
BIN     := bin
DBG     := debug
OBJ     := $(BIN)/$(DBG)/obj

CC          := clang
CFLAGS      := -c -MMD -I $(SRC)/ -g -Wall -Wextra -pedantic
LDFLAGS     := -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL raylib/libraylib.a

EXECUTABLE := $(BIN)/$(DBG)/$(EXECUTABLE_NAME)

SOURCE_FILES         := $(patsubst ${SRC}/%, %, $(wildcard $(SRC)/*.c))
OBJECT_FILES         := $(patsubst %.c, $(OBJ)/%.o, $(SOURCE_FILES))
DEPENDENCY_FILES     := $(patsubst %.c, $(OBJ)/%.d, $(SOURCE_FILES))

GREEN := \033[0;32m
NC    := \033[0m # No Color

.PHONY: build run clean

build: $(EXECUTABLE)
	@echo "\n${GREEN}Build successful!${NC}"

run: build
	@echo "\n${GREEN}Running...${NC}"
	./$(EXECUTABLE)

clean:
	@echo "${GREEN}Cleaning up...${NC}"
	rm -rf $(BIN)

$(EXECUTABLE): $(OBJECT_FILES)
	@echo "${GREEN}Linking ${@F}${NC}"
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJECT_FILES): $(OBJ)/%.o: $(SRC)/%.c
	@echo "${GREEN}Compiling $<${NC}"
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $<

-include $(DEPENDENCY_FILES)

