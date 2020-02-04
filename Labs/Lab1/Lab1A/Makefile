#NAME Parham Hajzavar
#EMAIL parhamzavar@gmail.com
#ID 405181239
#Project 1A

CC = gcc
CCFLAGS = -g -Wall -Wextra
TARGET = lab1a

default:
	$(CC) $(CFLAGS) -g -o $(TARGET) $(TARGET).c

dist:
	tar -czvf lab1a-405181239.tar.gz lab1a.c Makefile README

clean:
	rm -f $(TARGET) lab1a-405181239.tar.gz *.txt
