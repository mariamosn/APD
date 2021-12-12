// Maria Moșneag 333CA

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class Testapd {
    public static int P;

    public static void main(String[] args) {
        if (args.length < 2) {
            System.err.println("Usage: Testapd <P> <P - 1 time values>");
            return;
        }
        P = Integer.parseInt(args[0]);

        if (args.length < P || args.length > P) {
            System.err.println("Usage: Testapd <P> <P - 1 time values>");
            return;
        }
        if (P < 2 || P > 8) {
            System.err.println("Usage: 2 <= P <= 8");
            return;
        }

        int[] times = new int[P - 1];
        List<Integer> doneThreads = Collections.synchronizedList(new ArrayList<>());

        for (int i = 0; i < P - 1; i++) {
            times[i] = Integer.parseInt(args[i + 1]);
            if (times[i] < 1 || times[i] > 7) {
                System.err.println("Usage: 1 <= time <= 7");
                return;
            }
        }

        Thread[] threads = new Thread[P];

        for (int i = 0; i < P; i++) {
            threads[i] = new Thread(new MyThread(i, times, doneThreads));
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
