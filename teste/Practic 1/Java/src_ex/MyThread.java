import java.util.concurrent.BrokenBarrierException;

public class MyThread implements Runnable {
    private int[] v;
    private int id;
    private int start;
    private int end;

    public MyThread(int id, int[] v) {
        this.id = id;
        this.v = v;
        start = (int) (id * (double) Testapd.N / Testapd.P);
        end = (int) ((id + 1) * (double) Testapd.N / Testapd.P);
    }

    public void run() {
        int[] initial = new int[end - start];
        int[] pos = new int[end - start];

        for (int i = start; i < end; i++) {
            initial[i - start] = v[i];
            pos[i - start] = 0;

            for (int j = 0; j < Testapd.N; j++) {
                if (i != j && v[j] < v[i]) {
                    pos[i - start]++;
                }
            }
        }

        try {
            Testapd.barrier.await();
        } catch (InterruptedException e) {
            e.printStackTrace();
        } catch (BrokenBarrierException e) {
            e.printStackTrace();
        }

        for (int i = start; i < end; i++) {
            v[pos[i - start]] = initial[i - start];
        }

        try {
            Testapd.barrier.await();
        } catch (InterruptedException e) {
            e.printStackTrace();
        } catch (BrokenBarrierException e) {
            e.printStackTrace();
        }

        if (id == 0) {
            StringBuilder str = new StringBuilder();
            for (int i = 0; i < Testapd.N; i++) {
                str.append(v[i]);
                str.append(' ');
            }
            System.out.println(str);
        }
    }
}
