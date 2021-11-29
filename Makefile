# * ***************************************************************************************************
# * Copyright (c) 2021 Seung Hyeon Roh All Right Reserved
# *
# * - File Name : Makefile
# * - File Type : Makefile
# * - File Version(Last Update Date) : 1.0
# * - Date : Nov. 14, 2021.
# * - Description : login system source file
# * **************************************************************************************************/

all: hts_server hts_client

CC = g++
CFLAGS = -std=c++11 -g
LINKER = g++
LFLAGS = -g

SRCDIR = src
OBJDIR = obj
BINDIR = bin
SOURCES  := $(wildcard $(SRCDIR)/*.cc)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS := $(SOURCES:$(SRCDIR)/%.cc=$(OBJDIR)/%.o)

rm = rm -f

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cc
	@$(CC) $(CFLAGS) -c $< -o $@

hts_server:  $(OBJDIR)/server.o $(OBJDIR)/dept.o $(OBJDIR)/orderbook.o
	@$(LINKER) $(LFLAGS) $(OBJDIR)/server.o $(OBJDIR)/dept.o $(OBJDIR)/orderbook.o -o $@ -lpthread

hts_client: $(OBJDIR)/client.o $(OBJDIR)/dept.o
	@$(LINKER) $(LFLAGS) $(OBJDIR)/client.o $(OBJDIR)/dept.o -o $@ -lpthread

.PHONY: clean
clean:
	@$(rm) $(OBJDIR)/*.o hts_client hts_server

