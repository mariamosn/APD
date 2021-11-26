package task6;

import java.util.concurrent.ForkJoinPool;

public class MyMain {
    public static int N = 4;

    public static void main(String[] args) {
        ForkJoinPool fjp = new ForkJoinPool(4);
        int[] graph = new int[N];

        fjp.invoke(new MyTask(graph, 0));
        fjp.shutdown();
    }
}
