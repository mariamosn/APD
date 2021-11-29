package map_reduce_generic;

import reduce.ReduceTask;
import java.util.ArrayList;

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

        finalProcessing();
    }

    protected abstract void createTasks();

    protected abstract void createWorkers();

    // thread-urile își încheie execuția
    // această metodă poate fi suprascrisă dacă utilizatorul dorește
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

    protected abstract void finalProcessing();
}
