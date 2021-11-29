package map;

import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.*;
import java.util.concurrent.Semaphore;

/**
 * Creează task-uri de map pentru workeri și gestionează workerii
 */
public class MapCoordinator extends map_reduce_generic.Map<Map<String, ArrayList<MapResult>>, MapTask> {
    private final ArrayList<String> inputFiles;
    private final Integer fragmentSize;
    private Integer numberOfDocs;
    public static Semaphore semaphore;

    public MapCoordinator(Integer numOfWorkers, ArrayList<String> inputFiles, Integer fragmentSize,
                          Integer numberOfDocs) {
        super(numOfWorkers);
        this.inputFiles = inputFiles;
        this.fragmentSize = fragmentSize;
        this.numberOfDocs = numberOfDocs;
        results = Collections.synchronizedMap(new HashMap<>());
        this.semaphore = new Semaphore(1);
    }

    // creează task-uri de map pe baza documentelor
    protected void createTasks() {
        for (String doc : inputFiles) {
            try {
                long bytes = Files.size(Paths.get(doc));
                // se parcurge documentul în blocuri de câte D bytes
                for (int i = 0; i < bytes; i += fragmentSize) {
                    MapTask task = new MapTask(doc, i, fragmentSize);
                    tasks.add(task);
                }
            } catch (Exception e) {
                System.out.println(e.getMessage());
                e.printStackTrace();
            }
        }
    }

    // creează thread-uri (workeri) și împarte task-urile
    protected void createWorkers() {
        for (int i = 0; i < numOfWorkers; i++) {
            Thread thread = new Thread(new MapWorker(i, tasks, numOfWorkers, results, semaphore));
            threads.add(thread);
            threads.get(threads.size() - 1).start();
        }
    }
}
