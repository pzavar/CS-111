#NAME Parham Hajzavar
#EMAIL parhamzavar@gmail.com
#ID 405181239
#Project 1B

CC = gcc
CCFLAGS = -g -Wall -Wextra
TARGET = lab1b-client
TARGET1 = lab1b-server

default:
	$(CC) $(CFLAGS) -g -o $(TARGET) $(TARGET).c -lmcrypt
	$(CC) $(CFLAGS) -g -o $(TARGET1) $(TARGET1).c -lmcrypt

dist:
	tar -czvf lab1b-405181239.tar.gz $(TARGET).c $(TARGET1).c Makefile README my.key

clean:
	rm -f lab1b-405181239.tar.gz $(TARGET) $(TARGET1)
