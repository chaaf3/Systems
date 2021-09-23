# *******************************************************************************
# Name        : mtsieve makefile
# Author      : Connor Haaf & Ming Lin
# Date        : July 1, 2021
# Description : prime number sieve.
# Pledge : I pledge my Honor that I have abided by the Stevens Honor System.
# ******************************************************************************/
CC     = gcc
C_FILE = $(wildcard *.c)
TARGET = $(patsubst %.c,%,$(C_FILE))
CFLAGS = -g -Wall -Werror -pedantic-errors

all:
	$(CC) $(CFLAGS) $(C_FILE) -o $(TARGET) -pthread -lm
clean:
	rm -f $(TARGET) $(TARGET).exe