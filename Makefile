all: rdtscp_loop

workload.o: workload.c
	cc -O0 -c -m64 workload.c

rdtscp_loop: rdtscp_loop.c workload.o
	cc -O3 -m64 -o rdtscp_loop rdtscp_loop.c workload.o

clean:
	rm -f rdtscp_loop workload.o
