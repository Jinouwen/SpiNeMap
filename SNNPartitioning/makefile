SOURCE  := main.cc
SOURCE  += $(shell find include/ -name "*.cc")
CC      := g++
FLAGS   := -O3 -std=c++17 -w -Iinclude
LD      := -lboost_program_options 
TARGET  := clustering
OUT	:= out

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(FLAGS) $(SOURCE) -o $(TARGET) $(LD)

clean:
	rm -f $(TARGET)
