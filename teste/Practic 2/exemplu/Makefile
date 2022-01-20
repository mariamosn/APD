build:
	mpicc testapd2.c -o testapd2

run: build
	mpirun -np 6 --oversubscribe ./testapd2 file.in

clean:
	rm testapd2
