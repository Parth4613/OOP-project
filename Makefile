# Makefile for Railway Reservation System
# Compiles with g++ using C++17 standard

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
TARGET = railway

# Since all code is in headers (for simplicity), we only need to compile main.cpp
SRCS = main.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(TARGET).exe

.PHONY: all run clean
