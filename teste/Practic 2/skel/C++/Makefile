build:
	mpic++ testapd2.cpp -o testapd2

run: build
	mpirun -np 6 --oversubscribe ./testapd2

clean:
	rm testapd2
