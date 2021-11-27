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
    private ArrayList<MapTask> tasks = new ArrayList<>();
    private ArrayList<Thread> threads = new ArrayList<>();
    public static List<MapResult> results = Collections.synchronizedList(new ArrayList<>());

    public MapCoordinator(Integer numOfWorkers, ArrayList<String> inputFiles, Integer fragmentSize, Integer numberOfDocs) {
        this.numOfWorkers = numOfWorkers;
        this.inputFiles = inputFiles;
        this.fragmentSize = fragmentSize;
        this.numberOfDocs = numberOfDocs;
    }

    public void map() {
        // imparte fiecare document in mai multe task-uri
        for (String doc : inputFiles) {
            try {
                long bytes = Files.size(Paths.get(doc));
                // System.out.println(bytes);
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
            Thread thread = new Thread(new MapWorker(i, tasks, numOfWorkers));
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
    }

    // preia rezultatele cumva
    // creeaza task-uri noi pe baza lor?
}
