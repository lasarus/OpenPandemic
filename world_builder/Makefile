CC := gcc
CCFLAGS := -pedantic -Wall -Werror -g `sdl2-config --cflags --libs` -lSDL2_image -lGL -lGLEW -lGLU

SRCDIRS := src
SOURCES := $(foreach DIR, $(SRCDIRS), $(wildcard $(DIR)/*.c))
TARGET := bin/worldbuilder

.PHONY: all
.PHONY: clean

all: $(TARGET)

clean:
	rm -f $(TARGET)

$(TARGET): $(SOURCES)
	mkdir -p $(@D)
	$(CC) -o $(TARGET) $(SOURCES) $(CCFLAGS)
