// MAria Moșneag 333CA

package map_reduce_generic;

import java.util.ArrayList;

/**
 * Clasă abstractă generică ce reprezintă "prototipul"
 * pentru clasa care definește pașii etapei de Map
 * @param <E> este tipul rezultatului obținut în urma operațiilor de map
 * @param <MapTaskType> este tipul task-urilor din această etapă
 */
public abstract class Map<E, MapTaskType> {
    protected final Integer numOfWorkers;
    protected final ArrayList<MapTaskType> tasks;
    protected final ArrayList<Thread> threads;
    protected E results;

    public Map(Integer numOfWorkers) {
        this.numOfWorkers = numOfWorkers;
        this.tasks = new ArrayList<>();
        this.threads = new ArrayList<>();
    }

    public E map() {
        // creează task-uri de map pe baza documentelor
        createTasks();

        // creează thread-uri (workeri) și împarte task-urile
        createWorkers();

        // thread-urile își încheie execuția
        doneWorkers();

        return results;
    }

    // creează task-uri de map pe baza documentelor
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
}
