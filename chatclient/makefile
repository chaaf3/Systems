# *******************************************************************************
# Name        : chatclient makefile
# Author      : Ming Lin & Connor Haaf
# Date        : July 6, 2021
# Description : A chat server.
# Pledge : I pledge my Honor that I have abided by the Stevens Honor System.
# ******************************************************************************/
CC     = gcc
C_FILE = $(wildcard *.c)
TARGET = $(patsubst %.c,%,$(C_FILE))
CFLAGS = -g -Wall -Werror -pedantic-errors

all:
	$(CC) $(CFLAGS) $(C_FILE) -o $(TARGET)
clean:
	rm -f $(TARGET) $(TARGET).exe