package map;

import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

// creeaza task-uri pentru celelalte thread-uri
public class MapCoordinator {
    private Integer numOfWorkers;
    private ArrayList<String> inputFiles;
    private Integer fragmentSize;
    private Integer numberOfDocs;
    private ArrayList<MapTask> tasks;
    private ArrayList<Thread> threads;
    private List<MapResult> results;

    public MapCoordinator(Integer numOfWorkers, ArrayList<String> inputFiles, Integer fragmentSize, Integer numberOfDocs) {
        this.numOfWorkers = numOfWorkers;
        this.inputFiles = inputFiles;
        this.fragmentSize = fragmentSize;
        this.numberOfDocs = numberOfDocs;
        tasks = new ArrayList<>();
        threads = new ArrayList<>();
        results = Collections.synchronizedList(new ArrayList<>());
    }

    public List<MapResult> map() {
        // imparte fiecare document in mai multe task-uri
        for (String doc : inputFiles) {
            try {
                long bytes = Files.size(Paths.get(doc));
                for (int i = 0; i < bytes; i += fragmentSize) {
                    MapTask task = new MapTask(doc, i, fragmentSize);
                    tasks.add(task);
                }
            } catch (Exception e) {
                System.out.println(e.getMessage());
                e.printStackTrace();
            }
        }

        // imparte task-urile la workeri
        for (int i = 0; i < numOfWorkers; i++) {
            // aici trebuie sa pornesc mai multe thread-uri
            Thread thread = new Thread(new MapWorker(i, tasks, numOfWorkers, results));
            threads.add(thread);
            threads.get(threads.size() - 1).start();
        }

        for (int i = 0; i < numOfWorkers; i++) {
            try {
                threads.get(i).join();
            } catch (Exception e) {
                System.out.println(e.getMessage());
                e.printStackTrace();
            }
        }

        return results;
    }
}