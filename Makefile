## Simple Makefile for Vector Sort (macOS/Linux)

.PHONY: default build run clean rebuild

# Default CSV file path
CSV ?= data/eBid_Monthly_Sales.csv

default: build

build:
	cmake -S . -B build
	cmake --build build -j

run: build
	./build/Vector_Sort $(CSV)

clean:
	rm -rf build

rebuild: clean build

