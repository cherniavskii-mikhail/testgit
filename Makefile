CPP_FILE = src/main.cpp
OUTPUT = main
PYTHON_SCRIPT = scripts/tests.py

CXXFLAGS = -std=c++11 -O3

$(OUTPUT): $(CPP_FILE)
	@echo "Compiling $(CPP_FILE) into executable $(OUTPUT)..."
	g++ $(CXXFLAGS) $(CPP_FILE) -o $(OUTPUT)

.PHONY: build
build: $(OUTPUT)

.PHONY: run-python
run-python:
	@echo "Running $(PYTHON_SCRIPT)..."
	python3 $(PYTHON_SCRIPT)

.PHONY: all
all: build run-python

.PHONY: clean
clean:
	@rm -f $(OUTPUT)
	@rm -f results/results.txt
	@rm -rf __pycache__
