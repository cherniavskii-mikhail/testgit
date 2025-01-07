# Makefile для компиляции C++ файла и запуска Python скрипта

# Директории
SRC_DIR = src
BUILD_DIR = build

# Название файлов
CPP_FILE = $(SRC_DIR)/main.cpp
OBJ_FILE = $(BUILD_DIR)/main.o
OUTPUT = main

# Флаги компиляции
CXXFLAGS = -std=c++11 -O3

# Название Python скрипта
PYTHON_SCRIPT = scripts/test.py

# Компиляция объектного файла
$(OBJ_FILE): $(CPP_FILE) | $(BUILD_DIR)
	@echo "Компиляция $(CPP_FILE) в объектный файл $(OBJ_FILE)..."
	g++ $(CXXFLAGS) -c $(CPP_FILE) -o $(OBJ_FILE)

# Компиляция исполняемого файла
$(OUTPUT): $(OBJ_FILE)
	@echo "Компоновка $(OBJ_FILE) в исполняемый файл $(OUTPUT)..."
	g++ $(CXXFLAGS) $(OBJ_FILE) -o $(OUTPUT)

# Создание папки build, если её нет
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Компиляция всего
.PHONY: build
build: $(OUTPUT)

# Запуск Python скрипта
.PHONY: run-python
run-python:
	@echo "Запуск $(PYTHON_SCRIPT)..."
	python3 $(PYTHON_SCRIPT)

# Компиляция и запуск Python скрипта
.PHONY: all
all: build run-python

# Очистка
.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR)
	@rm -f $(OUTPUT)
	@rm -rf __pycache__
