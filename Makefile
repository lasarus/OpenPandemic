CC := gcc
CCFLAGS := -pedantic -Wall -Werror -g `sdl2-config --cflags --libs` -lGL -lGLEW -lGLU -lm

SRCDIRS := src
SOURCES := $(foreach DIR, $(SRCDIRS), $(wildcard $(DIR)/*.c))
TARGET := bin/OpenPandemic

.PHONY: all
.PHONY: clean

all: $(TARGET)

clean:
	rm -f $(TARGET)

$(TARGET): $(SOURCES)
	mkdir -p $(@D)
	$(CC) -o $(TARGET) $(SOURCES) $(CCFLAGS)
