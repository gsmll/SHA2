CC = g++
FLAGS = -std=c++17 -march=native -Wall -pedantic -O3 -I$(INCLUDE)

OBJECTS=$(BUILD)SHA256.o $(BUILD)SHA224.o $(BUILD)SHA512.o $(BUILD)SHA384.o
SHAKE_OBJECTS = $(OBJECTS) $(BUILD)ArgumentParser.o $(BUILD)Process.o
ASM = $(BUILD)SHA256.asm $(BUILD)SHA224.asm $(BUILD)SHA512.asm $(BUILD)SHA384.asm

INCLUDE = include/
BUILD = build/
SRC = src/
TOOL = tool/

test: $(BUILD)main
	$(BUILD)main
sha: sha256 sha224 sha512 sha384
shaker: $(BUILD)Shaker

sha256: $(TOOL)sha256
sha224: $(TOOL)sha224
sha512: $(TOOL)sha512
sha384: $(TOOL)sha384

$(TOOL)sha256: $(TOOL) $(BUILD) $(BUILD)SHA256.o
	$(CC) $(SRC)tools/sha256.cpp $(BUILD)SHA256.o $(FLAGS) -o $@
$(TOOL)sha224: $(TOOL) $(BUILD) $(BUILD)SHA224.o
	$(CC) $(SRC)tools/sha224.cpp $(BUILD)SHA224.o $(FLAGS) -o $@
$(TOOL)sha512: $(TOOL) $(BUILD) $(BUILD)SHA512.o
	$(CC) $(SRC)tools/sha512.cpp $(BUILD)SHA512.o $(FLAGS) -o $@
$(TOOL)sha384: $(TOOL) $(BUILD) $(BUILD)SHA384.o
	$(CC) $(SRC)tools/sha384.cpp $(BUILD)SHA384.o $(FLAGS) -o $@

# Used for development. Do not used. you likely dont have the libraries or tools to do so
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
$(BUILD)Process.o: $(SRC)shashaker/Process.cpp $(INCLUDE)shashaker/Process.hpp
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
$(TOOL):
	mkdir $(TOOL)
.PHONY: clean
clean:
	rm -rfv $(BUILD)
	rm -rfv $(TOOL)
