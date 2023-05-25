CC = g++
FLAGS = -std=c++17 -Wall -pedantic -O3 -I$(INCLUDE)

OBJECTS=$(BUILD)SHA256.o $(BUILD)SHA224.o $(BUILD)SHA512.o $(BUILD)SHA384.o


INCLUDE = include/
BUILD = build/
SRC = src/

main: $(BUILD)main
	$(BUILD)main

Shaker: $(BUILD)Shaker
	$(BUILD)Shaker

profile: $(BUILD)profile
	$(BUILD)profile


$(BUILD)main: $(BUILD)  $(OBJECTS) $(BUILD)main.o
	$(CC) $(FLAGS) $(BUILD)main.o $(OBJECTS) -o $(BUILD)main
$(BUILD)Shaker: $(BUILD)  $(OBJECTS) $(BUILD)Shaker.o
	$(CC) $(FLAGS) $(BUILD)Shaker.o $(OBJECTS) -o $(BUILD)Shaker
$(BUILD)profile: $(BUILD) $(OBJECTS) $(BUILD)profile.o
	$(CC) $(FLAGS) $(BUILD)profile.o $(OBJECTS) -pg -o $(BUILD)profile

$(BUILD)main.o: $(SRC)main.cpp
	$(CC) $(FLAGS) -c $< -o $@
$(BUILD)Shaker.o: $(SRC)shashaker/Shake.cpp
	$(CC) $(FLAGS) -c $< -o $@
$(BUILD)profile.o: $(SRC)profile.cpp
	$(CC) $(FLAGS) -c $< -o $@

$(BUILD)SHA256.o: $(SRC)SHA256.cpp $(INCLUDE)hash/SHA256.hpp
	$(CC) $(FLAGS) -c $< -o $@
$(BUILD)SHA224.o: $(SRC)SHA224.cpp $(INCLUDE)hash/SHA224.hpp
	$(CC) $(FLAGS) -c $< -o $@
$(BUILD)SHA512.o: $(SRC)SHA512.cpp $(INCLUDE)hash/SHA512.hpp
	$(CC) $(FLAGS) -c $< -o $@
$(BUILD)SHA384.o: $(SRC)SHA384.cpp $(INCLUDE)hash/SHA384.hpp
	$(CC) $(FLAGS) -c $< -o $@

$(BUILD):
	mkdir $(BUILD)
.PHONY: clean
clean:
	rm -rfv $(BUILD)