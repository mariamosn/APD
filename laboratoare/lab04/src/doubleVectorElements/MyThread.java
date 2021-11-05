package doubleVectorElements;

public class MyThread extends Thread {
    private final int id, N, P;
    int[]v;
    public MyThread(int id, int N, int P, int[]v) {
        this.id = id;
        this.N = N;
        this.P = P;
        this.v = v;
    }
    public void run() {
        double aux = N / P;
        int start = (int) (id * aux);
        int end = (int) ((id + 1) * aux);
        if (end > N || id == P - 1) {
            end = N;
        }
        for (int i = start; i < end; i++) {
            v[i] = v[i] * 2;
        }
    }
}
