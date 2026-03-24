CXX      := clang++
CXXFLAGS := -std=c++17 -Wall -Wextra -Werror -O2
LDFLAGS  :=
LDLIBS   := -lSDL2

SDL2_PREFIX := $(shell brew --prefix sdl2 2>/dev/null)
CXXFLAGS += -I$(SDL2_PREFIX)/include -I$(SDL2_PREFIX)/include/SDL2
LDFLAGS  += -L$(SDL2_PREFIX)/lib

TARGET   := chip8
OBJ_DIR  := build

SRCS     := $(wildcard *.cpp)
OBJS     := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRCS))

.PHONY: all clean fclean re run debug

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS) $(LDLIBS)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

debug: CXXFLAGS += -g -O0
debug: re

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(TARGET)

re: fclean all