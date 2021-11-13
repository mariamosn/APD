package multipleProducersMultipleConsumersNBuffer;

import java.util.LinkedList;
import java.util.concurrent.Semaphore;

public class Buffer {
    
    LinkedList<Integer> queue;
    Semaphore gol;
    Semaphore plin;
    
    public Buffer(int size) {
        queue = new LimitedQueue<>(size);
        gol = new Semaphore(size);
        plin = new Semaphore(0);
    }

	public void put(int value) {
        try {
            gol.acquire();
        } catch (Exception e) {

        }
        synchronized (this) {
            queue.add(value);
        }
        plin.release();
	}

	public int get() {
        int a = -1;
        try {
            plin.acquire();
        } catch (Exception e) {

        }
        synchronized (this) {
            Integer result = queue.poll();
            if (result != null) {
                a = result;
            }
        }
        gol.release();
        return a;
	}
}
