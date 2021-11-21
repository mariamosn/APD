package multipleProducersMultipleConsumers;

import java.util.concurrent.ArrayBlockingQueue;

public class Buffer {
	// int value;
	ArrayBlockingQueue<Integer> value = new ArrayBlockingQueue(1);

	void put(int value) {
		// this.value = value;
		Integer v = value;
		try {
			this.value.put(v);
		} catch (Exception e) {
			System.out.println(e.getMessage());
		}
	}

	int get() {
		// return this.value;
		int v = -1;
		try {
			v = value.take();
		} catch (Exception e) {
			System.out.println(e.getMessage());
		}
		return v;
	}
}
