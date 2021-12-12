public class Testapd {
    public static int N;
    public static int P;
    public static void main(String[] args) {
        N = Integer.parseInt(args[0]);
        P = Integer.parseInt(args[1]);

        Thread[] threads = new Thread[P];

        for (int i = 0; i < P; i++) {
            threads[i] = new Thread(new MyThread());
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
