import java.util.concurrent.CyclicBarrier;

public class Testapd {
    public static int N;
    public static int P;
    public static CyclicBarrier barrier;

    public static void main(String[] args) {
        N = Integer.parseInt(args[0]);
        P = Integer.parseInt(args[1]);
        barrier = new CyclicBarrier(P);

        Thread[] threads = new Thread[P];

        int[] v = new int[N];
        for (int i = 0; i < N; i++) {
            v[i] = N - i + 5;
        }

        for (int i = 0; i < P; i++) {
            threads[i] = new Thread(new MyThread(i, v));
            threads[i].start();
        }

        for (int i = 0; i < P; i++) {
            try {
                threads[i].join();
            } catch (Exception e) {
                System.out.println(e.getMessage());
                e.printStackTrace();
            }
        }
    }
}
