#NAME: Parham Hajzavar
#EMAIL: parhamzavar@gmail.com
#ID: 405181239
#SLIPDAYS: 0

CC = gcc
CCFLAGS = -Wall -Wextra -g

build:
	$(CC) $(CCFLAGS) -pthread -o lab2_list lab2_list.c SortedList.c -lprofiler

tests:
#lab2b_1.png & lab2b_2.png
#	./lab2_list --threads=1  --iterations=1000 --sync=m   >> lab2b_list.csv
#	./lab2_list --threads=2  --iterations=1000 --sync=m   >> lab2b_list.csv
#	./lab2_list --threads=4  --iterations=1000 --sync=m   >> lab2b_list.csv
#	./lab2_list --threads=8  --iterations=1000 --sync=m   >> lab2b_list.csv
#	./lab2_list --threads=12 --iterations=1000 --sync=m   >> lab2b_list.csv
#	./lab2_list --threads=16 --iterations=1000 --sync=m   >> lab2b_list.csv
#	./lab2_list --threads=24 --iterations=1000 --sync=m   >> lab2b_list.csv
#	./lab2_list --threads=1  --iterations=1000 --sync=s   >> lab2b_list.csv
#	./lab2_list --threads=2  --iterations=1000 --sync=s   >> lab2b_list.csv
#	./lab2_list --threads=4  --iterations=1000 --sync=s   >> lab2b_list.csv
#	./lab2_list --threads=8  --iterations=1000 --sync=s   >> lab2b_list.csv
#	./lab2_list --threads=12 --iterations=1000 --sync=s   >> lab2b_list.csv
#	./lab2_list --threads=16 --iterations=1000 --sync=s   >> lab2b_list.csv
#	./lab2_list --threads=24 --iterations=1000 --sync=s   >> lab2b_list.csv


#lab2b_3.png
	./lab2_list --threads=1  --iterations=1   --yield=id --lists=4   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=2   --yield=id --lists=4   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=4   --yield=id --lists=4   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=8   --yield=id --lists=4   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=16  --yield=id --lists=4   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1   --yield=id --lists=4   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=2   --yield=id --lists=4   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=4   --yield=id --lists=4   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=8   --yield=id --lists=4   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=16  --yield=id --lists=4   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1   --yield=id --lists=4   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=2   --yield=id --lists=4   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=4   --yield=id --lists=4   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=8   --yield=id --lists=4   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=16  --yield=id --lists=4   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1   --yield=id --lists=4   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=2   --yield=id --lists=4   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=4   --yield=id --lists=4   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=8   --yield=id --lists=4   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=16  --yield=id --lists=4   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=1   --yield=id --lists=4   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=2   --yield=id --lists=4   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=4   --yield=id --lists=4   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=8   --yield=id --lists=4   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=16  --yield=id --lists=4   >> lab2b_list.csv

	./lab2_list --threads=1  --iterations=10   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=20   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=40   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=80   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=10   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=20   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=40   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=80   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=10   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=20   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=40   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=80   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=10   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=20   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=40   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=80   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=10   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=20   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=40   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=80   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=10   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=20   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=40   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=80   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=10   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=20   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=40   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=80   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=10   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=20   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=40   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=80   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=10   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=20   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=40   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=80   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=10   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=20   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=40   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=80   --yield=id --lists=4  --sync=s   >> lab2b_list.csv

#lab2b_4.png
	./lab2_list --iterations=1000  --threads=1  --lists=1    --sync=m   >> lab2b_list.csv
	./lab2_list --iterations=1000  --threads=1  --lists=4    --sync=m   >> lab2b_list.csv
	./lab2_list --iterations=1000  --threads=1  --lists=8    --sync=m   >> lab2b_list.csv
	./lab2_list --iterations=1000  --threads=1  --lists=16   --sync=m   >> lab2b_list.csv
	./lab2_list --iterations=1000  --threads=2  --lists=1    --sync=m   >> lab2b_list.csv
	./lab2_list --iterations=1000  --threads=2  --lists=4    --sync=m   >> lab2b_list.csv
	./lab2_list --iterations=1000  --threads=2  --lists=8    --sync=m   >> lab2b_list.csv
	./lab2_list --iterations=1000  --threads=2  --lists=16   --sync=m   >> lab2b_list.csv
	./lab2_list --iterations=1000  --threads=4  --lists=1    --sync=m   >> lab2b_list.csv
	./lab2_list --iterations=1000  --threads=4  --lists=4    --sync=m   >> lab2b_list.csv
	./lab2_list --iterations=1000  --threads=4  --lists=8    --sync=m   >> lab2b_list.csv
	./lab2_list --iterations=1000  --threads=4  --lists=16   --sync=m   >> lab2b_list.csv
	./lab2_list --iterations=1000  --threads=8  --lists=1    --sync=m   >> lab2b_list.csv
	./lab2_list --iterations=1000  --threads=8  --lists=4    --sync=m   >> lab2b_list.csv
	./lab2_list --iterations=1000  --threads=8  --lists=8    --sync=m   >> lab2b_list.csv
	./lab2_list --iterations=1000  --threads=8  --lists=16   --sync=m   >> lab2b_list.csv
	./lab2_list --iterations=1000  --threads=12  --lists=1   --sync=m   >> lab2b_list.csv
	./lab2_list --iterations=1000  --threads=12  --lists=4   --sync=m   >> lab2b_list.csv
	./lab2_list --iterations=1000  --threads=12  --lists=8   --sync=m   >> lab2b_list.csv
	./lab2_list --iterations=1000  --threads=12  --lists=16  --sync=m   >> lab2b_list.csv


#lab2b_5.png
	./lab2_list --iterations=1000  --threads=1  --lists=1    --sync=s   >> lab2b_list.csv
	./lab2_list --iterations=1000  --threads=1  --lists=4    --sync=s   >> lab2b_list.csv
	./lab2_list --iterations=1000  --threads=1  --lists=8    --sync=s   >> lab2b_list.csv
	./lab2_list --iterations=1000  --threads=1  --lists=16   --sync=s   >> lab2b_list.csv
	./lab2_list --iterations=1000  --threads=2  --lists=1    --sync=s   >> lab2b_list.csv
	./lab2_list --iterations=1000  --threads=2  --lists=4    --sync=s   >> lab2b_list.csv
	./lab2_list --iterations=1000  --threads=2  --lists=8    --sync=s   >> lab2b_list.csv
	./lab2_list --iterations=1000  --threads=2  --lists=16   --sync=s   >> lab2b_list.csv
	./lab2_list --iterations=1000  --threads=4  --lists=1    --sync=s   >> lab2b_list.csv
	./lab2_list --iterations=1000  --threads=4  --lists=4    --sync=s   >> lab2b_list.csv
	./lab2_list --iterations=1000  --threads=4  --lists=8    --sync=s   >> lab2b_list.csv
	./lab2_list --iterations=1000  --threads=4  --lists=16   --sync=s   >> lab2b_list.csv
	./lab2_list --iterations=1000  --threads=8  --lists=1    --sync=s   >> lab2b_list.csv
	./lab2_list --iterations=1000  --threads=8  --lists=4    --sync=s   >> lab2b_list.csv
	./lab2_list --iterations=1000  --threads=8  --lists=8    --sync=s   >> lab2b_list.csv
	./lab2_list --iterations=1000  --threads=8  --lists=16   --sync=s   >> lab2b_list.csv
	./lab2_list --iterations=1000  --threads=12  --lists=1   --sync=s   >> lab2b_list.csv
	./lab2_list --iterations=1000  --threads=12  --lists=4   --sync=s   >> lab2b_list.csv
	./lab2_list --iterations=1000  --threads=12  --lists=8   --sync=s   >> lab2b_list.csv
	./lab2_list --iterations=1000  --threads=12  --lists=16  --sync=s   >> lab2b_list.csv



profile:
	-rm -f ./raw.gperf profile.out
	LD_PRELOAD=/w/home.23/cs/ugrad/hajzavar/cs111/lab2/lab2b/gperftools/.libs/libprofiler.so CPUPROFILE=./raw.gperf ./lab2_list --threads=4 --iterations=1000 --sync=s
	/w/home.23/cs/ugrad/hajzavar/cs111/lab2/lab2b/gperftools/src/pprof --text ./lab2_list.c ./raw.gperf > profile.out
	/w/home.23/cs/ugrad/hajzavar/cs111/lab2/lab2b/gperftools/src/pprof --list=thread_worker ./lab2_list ./raw.gperf >> profile.out
	-rm -f ./raw.gperf


graphs:
	./lab2b_list.gp

clean:
	rm -f $(TARGET)_add $(TARGET)_list $(TARGET)a-405181239.tar.gz *.png *.csv

dist:
	tar -czvf lab2b-405181239.tar.gz README Makefile SortedList.h SortedList.c lab2_list.c lab2b_list.gp *.png *.csv
