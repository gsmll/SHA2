CC = g++
FLAGS = -std=c++17 -Wall -pedantic -O3 -I$(INCLUDE)

OBJECTS=$(BUILD)SHA256.o $(BUILD)SHA224.o

INCLUDE = include/
BUILD = build/
SRC = src/

run-main: $(BUILD)main
	$(BUILD)main

$(BUILD)main: $(BUILD) $(OBJECTS)
	$(CC) $(FLAGS) $(SRC)main.cpp $(OBJECTS) -o $@

$(BUILD)%.o: $(SRC)%.cpp $(BUILD)
	$(CC) $(FLAGS) -c $< -o $@
$(BUILD)SHA256.o: $(SRC)SHA256.cpp $(INCLUDE)hash/SHA256.hpp $(BUILD)
	$(CC) $(FLAGS) -c $< -o $@

$(BUILD):
	mkdir $(BUILD)
.PHONY: clean
clean:
	rm -rfv $(BUILD)