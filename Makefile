CXX = clang++
INCLUDES := includes/itch-parser
SRC := src
OBJ := obj
BIN := bin
TESTS := tests

CXX_FLAGS = -std=c++23 -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined -g -I $(INCLUDES)

.PHONY: all clean exec test

all: $(BIN)/exec

$(BIN)/exec: $(OBJ)/main.o $(OBJ)/itch-parsers.o | $(BIN)
	$(CXX) $(CXX_FLAGS) $^ -o $@

$(BIN)/parser-tests: $(OBJ)/itch-parsers-test.o $(OBJ)/itch-parsers.o | $(BIN)
	$(CXX) $(CXX_FLAGS) $^ -o $@

$(OBJ)/main.o: $(SRC)/main.cpp | $(OBJ)
	$(CXX) $(CXX_FLAGS) -c $< -o $@

$(OBJ)/itch-parsers.o: $(SRC)/itch-parser/itch-parsers.cpp | $(OBJ)
	$(CXX) $(CXX_FLAGS) -c $< -o $@

$(OBJ)/itch-parsers-test.o: $(TESTS)/itch-parsers-test.cpp | $(OBJ)
	$(CXX) $(CXX_FLAGS) -c $< -o $@

$(BIN) $(OBJ):
	mkdir -p $@

exec: $(BIN)/exec
	./$(BIN)/exec

test: $(BIN)/parser-tests
	./$(BIN)/parser-tests

clean:
	rm -rf $(BIN)/* $(OBJ)/*
