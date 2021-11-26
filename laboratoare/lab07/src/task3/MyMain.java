package task3;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicInteger;

public class MyMain {
    public static int N = 4;

    public static void main(String[] args) {
        AtomicInteger inQueue = new AtomicInteger(0);
        ExecutorService tpe = Executors.newFixedThreadPool(4);

        int[] graph = new int[N];

        inQueue.incrementAndGet();
        tpe.submit(new MyRunnable(graph, 0, tpe, inQueue));
    }
}
