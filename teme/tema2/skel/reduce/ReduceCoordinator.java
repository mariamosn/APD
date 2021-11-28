package reduce;

import map.MapResult;

import java.io.FileWriter;
import java.util.*;
import java.util.concurrent.Semaphore;

public class ReduceCoordinator {
    private List<MapResult> mapResults;
    private Integer numOfWorkers;
    private ArrayList<ReduceTask> tasks;
    private ArrayList<Thread> threads;
    private HashMap<String, ArrayList<MapResult>> ht;
    // private List<ReduceResult> results;
    private Map<String, ReduceResult> results;
    private List<Integer> fibonacci;
    private Semaphore semaphore;
    private ArrayList<String> docs;
    private String fileOut;

    public ReduceCoordinator(List<MapResult> mapResults, Integer numOfWorkers, ArrayList<String> docs, String fileOut) {
        this.mapResults = mapResults;
        this.numOfWorkers = numOfWorkers;
        this.docs = docs;
        this.fileOut = fileOut;
        tasks = new ArrayList<>();
        threads = new ArrayList<>();
        ht = new HashMap<>();
        // results = Collections.synchronizedList(new ArrayList<>());
        results = Collections.synchronizedMap(new HashMap<>());
        fibonacci = Collections.synchronizedList(new ArrayList<>());
        fibonacci.add(0);
        fibonacci.add(1);
        semaphore = new Semaphore(1);
    }

    public void reduce() {
        for (MapResult result : mapResults) {
            if (ht.containsKey(result.getDocName())) {
                ArrayList<MapResult> aux = ht.get(result.getDocName());
                aux.add(result);
                ht.replace(result.getDocName(), aux);
            } else {
                ArrayList<MapResult> aux = new ArrayList<>();
                aux.add(result);
                ht.put(result.getDocName(), aux);
            }
        }

        for (String doc : ht.keySet()) {
            ReduceTask task = new ReduceTask(doc, ht.get(doc));
            tasks.add(task);
        }

        // imparte task-urile la workeri
        for (int i = 0; i < numOfWorkers; i++) {
            // aici trebuie sa pornesc mai multe thread-uri
            Thread thread = new Thread(new ReduceWorker(i, numOfWorkers, tasks, results, fibonacci, semaphore));
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

        sort();

        write();
    }

    private void sort() {
        Collections.sort(docs, new Comparator<String>() {
            @Override
            public int compare(String o1, String o2) {
                if (results.get(o1).getRank() < results.get(o2).getRank()) {
                    return 1;
                }
                return -1;
            }
        });
    }

    private void write() {
        try {
            FileWriter out = new FileWriter(fileOut);
            for (String doc : docs) {
                ReduceResult result = results.get(doc);
                out.write(doc + "," +
                            String.format("%.2f", result.getRank()) + "," +
                            result.getMaxLen() + "," +
                            result.getNumberOfMaxLen() + "\n");
            }
            out.close();
        } catch (Exception e) {
            System.out.println(e.getMessage());
            e.printStackTrace();
        }
    }
}
