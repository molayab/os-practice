# Makefile por Carolina Arcos Prats y Mateo Olaya Bernal
CC := g++
SRC := ../src
BUILD := ../bin
TARGET := $(BUILD)/evaluator

SOURCES := \
		$(SRC)/evaluator.cpp

OBJECTS := $(SOURCES:.cpp=.o)

CFLAGS :=
LIBS :=
INCLUDES :=

all: $(OBJECTS)

$(SRC)/evaluator.o:
	$(CC) $(CFLAGS) $(SRC)/evaluator.o -o $(TARGET)
