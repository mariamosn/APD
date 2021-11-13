package readersWriters.readerPriority;

import java.util.concurrent.BrokenBarrierException;

public class Reader extends Thread {
    private final int id;

    public Reader(int id) {
        super();
        this.id = id;
    }

    @Override
    public void run() {
        try {
            Main.barrier.await();
        } catch (InterruptedException | BrokenBarrierException e) {
            e.printStackTrace();
        }

        do {
            // TODO
            try {
                Main.mutexNumberOfReaders.acquire();
            } catch (Exception e) {

            }
            Main.currentReaders++;
            // TODO
            if (Main.currentReaders == 1) {
                try {
                    Main.readWriteSem.acquire();
                } catch (Exception e) {

                }
            }
            Main.mutexNumberOfReaders.release();

            try {
                Thread.sleep(100);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            System.out.println("Reader " + id + " is reading");
            Main.hasRead[id] = true;

            // TODO
            try {
                Main.mutexNumberOfReaders.acquire();
            } catch (Exception e) {

            }
            Main.currentReaders--;
            // TODO
            if (Main.currentReaders == 0) {
                Main.readWriteSem.release();
            }
            Main.mutexNumberOfReaders.release();

        } while (!Main.hasRead[id]);
    }
}
