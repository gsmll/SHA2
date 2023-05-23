CC = g++
FLAGS = -std=c++17 -march=native -Wall -pedantic -O3 -I$(INCLUDE)

OBJECTS=$(BUILD)SHA256.o $(BUILD)SHA224.o

INCLUDE = include/
BUILD = build/
SRC = src/

main: $(BUILD)main
	$(BUILD)main
profile: $(BUILD)profile
	$(BUILD)profile
test: $(BUILD)test
	$(BUILD)test


$(BUILD)main: $(BUILD)  $(OBJECTS) $(BUILD)main.o
	$(CC) $(BUILD)main.o $(OBJECTS) $(FLAGS) -o $(BUILD)main
$(BUILD)profile: $(BUILD) $(OBJECTS) $(BUILD)profile.o
	$(CC) $(BUILD)profile.o $(OBJECTS) -std=c++17 -O3 -Iinclude/ -Llib -lbenchmark -lpthread -o $(BUILD)profile 
$(BUILD)test: $(BUILD) $(OBJECTS) $(BUILD)test.o
	$(CC) $(BUILD)test.o $(OBJECTS) -std=c++17 -O3 -Iinclude/ -o $(BUILD)test 

$(BUILD)main.o: $(SRC)main.cpp
	$(CC) $(FLAGS) -c $< -o $@
$(BUILD)profile.o: $(SRC)profile.cpp
	$(CC) $(FLAGS) -c $< -o $@
$(BUILD)test.o: $(SRC)test.cpp
	$(CC) $(FLAGS) -c $< -o $@

$(BUILD)SHA256.o: $(SRC)SHA256.cpp $(INCLUDE)hash/SHA256.hpp
	$(CC) $(FLAGS) -c $< -o $@
$(BUILD)SHA224.o: $(SRC)SHA224.cpp $(INCLUDE)hash/SHA224.hpp
	$(CC) $(FLAGS) -c $< -o $@

$(BUILD):
	mkdir $(BUILD)
.PHONY: clean
clean:
	rm -rfv $(BUILD)