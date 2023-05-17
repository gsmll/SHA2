CC = g++
FLAGS = -std=c++17 -Wall -pedantic -O3 -I$(INCLUDE)

OBJECTS=

INCLUDE = include/
BUILD = build/
SRC = src/

run-main: $(BUILD)main
	$(BUILD)main

$(BUILD)main: $(BUILD) $(OBJECTS)
	$(CC) $(FLAGS) $(SRC)main.cpp $(OBJECTS) -o $@

$(BUILD)%.hpp: $(SRC)%.cpp $(BUILD)
	$(CC) $(FLAGS) -c $< -o $@

$(BUILD):
	mkdir $(BUILD)
.PHONY: clean
clean:
	rm -rfv $(BUILD)