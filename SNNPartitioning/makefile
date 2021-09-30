SOURCE  := main.cc
SOURCE  += $(shell find include/ -name "*.cc")
CC      := g++
FLAGS   := -O3 -std=c++17 -w -I include
LD      := -lboost_program_options -lpthread
TARGET  := ext
OUT	:= out

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(FLAGS) $(SOURCE) -o $(TARGET) $(LD)

clean:
	rm -f $(TARGET)
