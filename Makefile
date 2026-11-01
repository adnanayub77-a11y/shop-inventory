# Shop Inventory and Billing System — Portfolio Project #2
# Default compiler: MinGW GCC (g++). Use `make clang` when clang++ is installed.

CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -pedantic -Iinclude
SRC       = src/main.cpp src/Product.cpp src/InventorySystem.cpp
BIN       = shop.exe

.PHONY: all gcc clang run clean

all: gcc

gcc:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(BIN)

clang:
	clang++ $(CXXFLAGS) $(SRC) -o $(BIN)

run: $(BIN)
	./$(BIN)

$(BIN): $(SRC) include/config.h include/Product.h include/Sale.h include/InventorySystem.h include/textutil.h
	$(CXX) $(CXXFLAGS) $(SRC) -o $(BIN)

clean:
	rm -f $(BIN) shop
