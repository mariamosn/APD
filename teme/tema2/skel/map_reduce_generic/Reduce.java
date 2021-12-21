// Maria Moșneag 333CA

package map_reduce_generic;

import reduce.ReduceTask;
import java.util.ArrayList;

/**
 * Clasă abstractă generică ce reprezintă "prototipul"
 * pentru clasa care definește pașii etapei de Reduce
 * @param <E> este tipul rezultatului obținut în urma etapei de Map
 *            pe baza căruia se vor efectua operațiile de Reduce
 */
public abstract class Reduce<E> {
    protected final Integer numOfWorkers;
    protected final ArrayList<ReduceTask> tasks;
    protected final ArrayList<Thread> threads;
    protected E mapResults;

    public Reduce(Integer numOfWorkers) {
        this.numOfWorkers = numOfWorkers;
        this.tasks = new ArrayList<>();
        this.threads = new ArrayList<>();
    }

    public void reduce(E mapResults) {
        this.mapResults = mapResults;
        // creează task-uri de reduce pe baza rezultatelor din etapa de map
        createTasks();

        // creează thread-uri (workeri) și împarte task-urile
        createWorkers();

        // thread-urile își încheie execuția
        doneWorkers();

        // prelucrarea finală a rezultatelor obținute și scrierea acestora în fișierul de output
        finalProcessing();
    }

    // creează task-uri de reduce pe baza rezultatelor din etapa de map
    protected abstract void createTasks();

    // creează thread-uri (workeri) și împarte task-urile
    protected abstract void createWorkers();

    // thread-urile își încheie execuția
    // această metodă poate fi suprascrisă dacă utilizatorul dorește acest lucru
    protected void doneWorkers() {
        for (int i = 0; i < numOfWorkers; i++) {
            try {
                threads.get(i).join();
            } catch (Exception e) {
                System.out.println(e.getMessage());
                e.printStackTrace();
            }
        }
    }

    // prelucrarea finală a rezultatelor obținute și scrierea acestora în fișierul de output
    protected abstract void finalProcessing();
}
