package bug2;

/**
 * Why does this code not block? We took the same lock twice!
 *
 * There is nothing to modify for this example. Just explain why we do NOT
 * have a deadlock.
 */
public class MyThread implements Runnable {
    static int i;

    @Override
    public void run() {
        synchronized (this) {
            // aici poate ajunge un singur thread la un moment dat
            synchronized (this) {
                // si aici tot un singur thread, dar oricum nu puteau ajunge mai multe in acelasi timp din cauza
                // synchronized-ului anterior
                i++;
            }
        }
    }
}
