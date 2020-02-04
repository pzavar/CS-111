default:
	gcc -Wall -Wextra -g -o lab4b lab4b.c -lmraa -lm

check:
	./lab4b --scale=F --period=3 --log=logfile.txt
	STOP
	START
	OFF
	rm -f logfile.txt

dist:
	tar -czvf lab4b-405181239.tar.gz lab4b.c Makefile README

clean:
	rm -f lab4b lab4b-405181239.tar.gz
