CC = gcc
CFLAGS = -Wall
LIBS_LIST = lua5.4 lua sdl2 libpng
CFLAGS += $(foreach lib,$(LIBS_LIST),$(shell pkg-config --cflags $(lib))) -Wno-unused-function -mconsole
LIBS = $(foreach lib,$(LIBS_LIST),$(shell pkg-config --libs --static $(lib))) -lm

SRCDIR = src
TOOLSDIR = tools
OBJDIR = obj
BUILDDIR = bin
TARGET = nibble8

SOURCES = $(shell find $(SRCDIR) -type f -name *.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

all: CFLAGS += -O2
all: $(BUILDDIR)/$(TARGET)

debug: CFLAGS += -g
debug: $(BUILDDIR)/$(TARGET)

$(BUILDDIR)/$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(OBJDIR)

font2bin: $(TOOLSDIR)/font2bin.c
	$(CC) $(CFLAGS) -o $(BUILDDIR)/font2bin $^ $(LIBS)