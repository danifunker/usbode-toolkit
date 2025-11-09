# Makefile for USBODE Disc Manager
# For use with CodeWarrior Pro or MPW
# Targets PowerPC Mac OS 7.5.3 and higher

# Compiler settings
CC = mwcppc
REZ = rez
LINK = mwlinkppc

# Directories
OBJDIR = obj
BINDIR = bin

# Application name
APP = USBODE

# Compiler flags
CFLAGS = -w on -opt speed -inline auto \
         -align powerpc -proc 604 \
         -bool on -RTTI off -exceptions off \
         -includes unix -i- \
         -prefix MacHeaders.h

# Linker flags
LDFLAGS = -mpwtool -sym on -c 'USBO' -t 'APPL'

# Libraries
LIBS = -lInterfaceLib -lMathLib -lStdCLib -lToolLibs

# Source files
SOURCES = USBODE.c
OBJECTS = $(OBJDIR)/USBODE.o

# Resource file
RESOURCES = USBODE.r
RSRC = $(APP).rsrc

# Default target
all: directories $(BINDIR)/$(APP)

# Create directories
directories:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(BINDIR)

# Compile C source
$(OBJDIR)/%.o: %.c USBODE.h
	$(CC) $(CFLAGS) -o $@ $<

# Compile resources
$(RSRC): $(RESOURCES)
	$(REZ) -o $@ $< -i : -d SYSTYPE=PPCC

# Link application
$(BINDIR)/$(APP): $(OBJECTS) $(RSRC)
	$(LINK) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBS)
	$(REZ) -a $(RSRC) -o $@

# Clean build artifacts
clean:
	rm -rf $(OBJDIR)
	rm -rf $(BINDIR)
	rm -f $(RSRC)

# Rebuild everything
rebuild: clean all

.PHONY: all directories clean rebuild
