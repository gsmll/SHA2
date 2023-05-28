CC = g++
FLAGS = -std=c++17 -march=native -Wall -pedantic -O1 -I$(INCLUDE)

OBJECTS=$(BUILD)SHA256.o $(BUILD)SHA224.o $(BUILD)SHA512.o $(BUILD)SHA384.o
SHAKE_OBJECTS = $(OBJECTS) $(BUILD)ArgumentParser.o
ASM = $(BUILD)SHA256.asm $(BUILD)SHA224.asm $(BUILD)SHA512.asm $(BUILD)SHA384.asm

INCLUDE = include/
BUILD = build/
SRC = src/

main: $(BUILD)main
	$(BUILD)main

shaker: $(BUILD)Shaker
	$(BUILD)Shaker

profile: $(BUILD)profile
	$(BUILD)profile
time: $(BUILD)time
	$(BUILD)time
asm: $(ASM)

perf: $(BUILD)time
	perf record -F 10000 $(BUILD)time
	perf report


$(BUILD)main: $(BUILD)  $(OBJECTS) $(BUILD)main.o
	$(CC) $(BUILD)main.o $(OBJECTS) $(FLAGS) -o $(BUILD)main
$(BUILD)Shaker: $(BUILD)  $(SHAKE_OBJECTS) $(BUILD)Shaker.o
	$(CC) $(FLAGS) $(BUILD)Shaker.o $(SHAKE_OBJECTS) -o $(BUILD)Shaker
$(BUILD)profile: $(BUILD) $(OBJECTS) $(BUILD)profile.o
	$(CC) $(BUILD)profile.o $(OBJECTS) -std=c++17 -O3 -Iinclude/ -Llib -lbenchmark -lpthread -o $(BUILD)profile 
$(BUILD)time: $(BUILD) $(OBJECTS) $(BUILD)test.o
	$(CC) $(BUILD)test.o $(OBJECTS) -std=c++17 -O3 -Iinclude/ -o $(BUILD)time 

$(BUILD)main.o: $(SRC)main.cpp
	$(CC) $(FLAGS) -c $< -o $@
$(BUILD)Shaker.o: $(SRC)shashaker/Shake.cpp
	$(CC) $(FLAGS) -c $< -o $@
$(BUILD)profile.o: $(SRC)profile.cpp
	$(CC) $(FLAGS) -c $< -o $@
$(BUILD)test.o: $(SRC)test.cpp
	$(CC) $(FLAGS) -c $< -o $@

$(BUILD)SHA256.o: $(SRC)SHA256.cpp $(INCLUDE)hash/SHA256.hpp
	$(CC) $(FLAGS) -c $< -o $@
$(BUILD)SHA224.o: $(SRC)SHA224.cpp $(INCLUDE)hash/SHA224.hpp
	$(CC) $(FLAGS) -c $< -o $@
$(BUILD)SHA512.o: $(SRC)SHA512.cpp $(INCLUDE)hash/SHA512.hpp
	$(CC) $(FLAGS) -c $< -o $@
$(BUILD)SHA384.o: $(SRC)SHA384.cpp $(INCLUDE)hash/SHA384.hpp
	$(CC) $(FLAGS) -c $< -o $@

$(BUILD)ArgumentParser.o: $(SRC)shashaker/ArgumentParser.cpp $(INCLUDE)shashaker/ArgumentParser.hpp
	$(CC) $(FLAGS) -c $< -o $@

$(BUILD)SHA256.asm: $(SRC)SHA256.cpp $(INCLUDE)hash/SHA256.hpp
	$(CC) $(FLAGS) -c $< -S -o $@
$(BUILD)SHA224.asm: $(SRC)SHA224.cpp $(INCLUDE)hash/SHA224.hpp
	$(CC) $(FLAGS) -c $< -S -o $@
$(BUILD)SHA512.asm: $(SRC)SHA512.cpp $(INCLUDE)hash/SHA512.hpp
	$(CC) $(FLAGS) -c $< -S -o $@
$(BUILD)SHA384.asm: $(SRC)SHA384.cpp $(INCLUDE)hash/SHA384.hpp
	$(CC) $(FLAGS) -c $< -S -o $@

$(BUILD):
	mkdir $(BUILD)
.PHONY: clean
clean:
	rm -rfv $(BUILD)