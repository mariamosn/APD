// Maria Moșneag 333CA

import java.util.List;

public class MyThread implements Runnable {
    private int[] times;
    private int id;
    private int crtDoneThreads = 0;
    private List<Integer> doneThreads;

    public MyThread(int id, int[] times, List<Integer> doneThreads) {
        this.times = times;
        this.id = id;
        this.doneThreads = doneThreads;
    }

    public void run() {
        if (id == 0) {
            while (crtDoneThreads < Testapd.P - 1) {
                synchronized (doneThreads) {
                    while (doneThreads.size() == crtDoneThreads) {
                        try {
                            doneThreads.wait();
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                    }
                    System.out.println("Thread-ul " + doneThreads.get(crtDoneThreads) + " a terminat");
                    crtDoneThreads++;
                }
            }
        } else {
            try {
                Thread.sleep(times[id - 1] * 1000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            synchronized (doneThreads) {
                doneThreads.add(id);
                doneThreads.notify();
            }
        }
    }
}
