// Maria Moșneag 333CA

package reduce;

import map.MapResult;
import map_reduce_generic.Reduce;

import java.io.FileWriter;
import java.util.*;
import java.util.concurrent.Semaphore;

/**
 * Clasă responsabilă de crearea task-urilor pentru etapa de Reduce,
 * de gestionarea activității workerilor și de prelucrarea rezultatelor finale
 */
public class ReduceCoordinator extends Reduce<Map<String, ArrayList<MapResult>>> {
    private final Map<String, ReduceResult> results;
    private final List<Integer> fibonacci;
    private final Semaphore semaphore;
    private final ArrayList<String> docs;
    private final String fileOut;

    public ReduceCoordinator(Integer numOfWorkers,
                             ArrayList<String> docs, String fileOut) {
        super(numOfWorkers);
        this.docs = docs;
        this.fileOut = fileOut;

        results = Collections.synchronizedMap(new HashMap<>());
        semaphore = new Semaphore(1);

        fibonacci = Collections.synchronizedList(new ArrayList<>());
        fibonacci.add(0);
        fibonacci.add(1);
    }

    // sortează documentele descrescător în funcție de rang
    private void sort() {
        docs.sort((o1, o2) -> {
            if (results.get(o1).getRank() < results.get(o2).getRank()) {
                return 1;
            } else if (results.get(o1).getRank().equals(results.get(o2).getRank())) {
                return 0;
            }
            return -1;
        });
    }

    // scrie rezultatele finale în fișierul de ieșire
    private void write() {
        try {
            FileWriter out = new FileWriter(fileOut);
            for (String doc : docs) {
                // determină numele documentului
                String name = doc;
                if (doc.contains("/")) {
                    String[] aux = doc.split("/");
                    name = aux[aux.length - 1];
                }

                ReduceResult result = results.get(doc);
                out.write(name + "," +
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

    // creează task-uri de reduce pe baza rezultatelor din etapa de map
    protected void createTasks() {
        for (String doc : mapResults.keySet()) {
            ReduceTask task = new ReduceTask(doc, mapResults.get(doc));
            tasks.add(task);
        }
    }

    // creează thread-uri (workeri) și împarte task-urile
    protected void createWorkers() {
        for (int i = 0; i < numOfWorkers; i++) {
            Thread thread = new Thread(new ReduceWorker(i, numOfWorkers, tasks, results, fibonacci, semaphore));
            threads.add(thread);
            threads.get(threads.size() - 1).start();
        }
    }

    protected void finalProcessing() {
        // sortează documentele în funcție de rang
        sort();

        // scrie rezultatele finale în fișierul de ieșire
        write();
    }
}
