#NAME Parham Hajzavar
#EMAIL parhamzavar@gmail.com
#ID 405181239
#Project 0 Warm Up

CC = gcc
CCFLAGS = -g -Wall -Wextra
TARGET = lab0

default:
	$(CC) $(CFLAGS) -g -o $(TARGET) $(TARGET).c

check: default checkIO checkSegfault checkCatch1 checkCatch2 checkOut checkIn checkArgument
	if [ -s messages.txt ]; \
	then \
		cat messages.txt; \
		rm -f messages.txt; \
	else \
		echo "Smoke test are complete."; \
	fi; \

checkIO:
	touch out.txt; \
	echo "this test is for IO" > in.txt; \
	./lab0.c --input=in.txt --output=out.txt; \
	chmod -w out.txt; \
	if [[ $$? -ne 0 ]]; \
	then \
		echo "ERROR: failed IO test"; \
	else \
		echo "IO test successful"; \
	fi; \
	rm -f in.txt out.txt; \

checkArgument:
	./lab0 --blah; \
	if [[ $$?==1 ]]; \
	then \
		echo "Argument test successful"; \
	else \
		echo "ERROR: failed argument test"; \
	fi; \

checkOut:
	echo "this test is for output" > out.txt; \
	chmod -w out.txt; \
	./lab0 --output=out.txt; \
	if [[ $$? -ne 3 ]]; \
	then \
		echo "ERROR: failed output file test"; \
	else \
		echo "Output file test successful"; \
	fi; \
	rm -f out.txt; \

checkIn:
	touch in.txt; \
	echo "this test is for input" > in.txt; \
	./lab0 --input=in.txt; \
	if [[ $$?==2 ]]; \
	then \
		echo "Input file test successful"; \
	else \
		echo "ERROR: failed input file test"; \
	fi; \
	rm -f in.txt; \

checkSegfault:
	./lab0 --segfault; \
	if [[ $$? -ne 139 ]]; \
	then \
		echo "ERROR: failed segfault test"; \
	else \
		echo "segfault test successful"; \
	fi; \

checkCatch1:
	./lab0 --segfault --catch; \
	if [[ $$? -ne 4 ]]; \
	then \
		echo "ERROR: failed catch segfault test"; \
	else \
		echo "Catch segfault test successful"; \
	fi; \

checkCatch2:
	./lab0 --catch --segfault; \
	if [[ $$? -ne 4 ]]; \
	then \
		echo "ERROR: failed catch segfault test"; \
	else \
		echo "Catch segfault test successful"; \
	fi; \

dist:
	tar -czvf lab0-405181239.tar.gz lab0.c backtrace.png breakpoint.png Makefile README

clean:
	rm  -f $(TARGET) lab0-405181239.tar.gz *.txt

