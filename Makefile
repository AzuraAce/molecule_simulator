# Makefile

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2

# SDL2, GLEW, OpenGL flags from pkg-config
PKG_CONFIG = sdl2 glew opengl
CFLAGS += $(shell pkg-config --cflags $(PKG_CONFIG))
LDFLAGS = $(shell pkg-config --libs $(PKG_CONFIG)) -lm -lSDL2_ttf

# Directories
SRCDIR := src
BUILDDIR := obj
BINDIR := bin

# File patterns
SRC := $(wildcard $(SRCDIR)/*.c)
OBJ := $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(SRC))
OUT := $(BINDIR)/main


# Default target
all: $(OUT)

# Linking
$(OUT): $(OBJ) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compilation
$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Directories
$(BINDIR) $(BUILDDIR):
	mkdir -p $@

# Clean rule
clean:
	rm -rf $(BUILDDIR) $(OUT)
