package shortestPathsFloyd_Warshall;

public class MyThread extends Thread {
    private final int id, N, P;
    int[][] graph;

    public MyThread(int id, int N, int P, int[][] graph) {
        this.id = id;
        this.N = N;
        this.P = P;
        this.graph = graph;
    }
    public void run() {
        double aux = N / P;
        int start = (int) (id * aux);
        int end = (int) ((id + 1) * aux);
        if (end > N || id == P - 1) {
            end = N;
        }

        for (int i = start; i < end; i++) {
            for (int k = 0; k < 5; k++) {
                for (int j = 0; j < 5; j++) {
                    graph[i][j] = Math.min(graph[i][k] + graph[k][j], graph[i][j]);
                }
            }
        }
    }
}
