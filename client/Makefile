# * ***************************************************************************************************
# * Copyright (c) 2021 Seung Hyeon Roh All Right Reserved
# *
# * - File Name : Makefile
# * - File Type : Makefile
# * - File Version(Last Update Date) : 1.0
# * - Date : Nov. 14, 2021.
# * - Description : login system source file
# * **************************************************************************************************/

TARGET = hts_client

CC = g++
CFLAGS = -std=c++11 -g
LINKER = g++
LFLAGS = -g

SRCDIR = src
OBJDIR = obj
BINDIR = bin

SOURCES  := $(wildcard $(SRCDIR)/*.cc)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.cc=$(OBJDIR)/%.o)
rm = rm -f

$(BINDIR)/$(TARGET): $(OBJECTS)
	@$(LINKER) $(LFLAGS) $(OBJECTS) -o $@

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cc
	@$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@$(rm) $(OBJECTS)

.PHONY: remove
remove: clean
	@$(rm) $(BINDIR)/$(TARGET)

