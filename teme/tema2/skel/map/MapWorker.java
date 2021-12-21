// Maria Moșneag 333CA

package map;

import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.Semaphore;

/**
 * Reprezintă un worker din cadrul etapei de map
 */
public class MapWorker implements Runnable {
    private final Integer id;
    private final Integer numOfWorkers;
    private final Map<String, ArrayList<MapResult>> results;
    private final ArrayList<MapTask> tasks;
    private final Integer start;
    private Integer end;
    private final Semaphore semaphore;

    private HashMap<Integer, Integer> map;
    private ArrayList<String> maxLenWords;
    private Integer maxLen;

    private RandomAccessFile in = null;
    private String crtDoc = null;

    public MapWorker(Integer id, ArrayList<MapTask> tasks, Integer numOfWorkers, Map<String,
            ArrayList<MapResult>> results, Semaphore semaphore) {
        this.id = id;
        this.tasks = tasks;
        this.numOfWorkers = numOfWorkers;
        this.results = results;
        this.semaphore = semaphore;

        start = (int) (this.id * (double)tasks.size() / numOfWorkers);
        end = (int) ((this.id + 1) * (double)tasks.size() / numOfWorkers);
        if (end > tasks.size()) {
            end = tasks.size();
        }
    }

    @Override
    public void run() {
        try {
            // parcurge task-urile corespunzătoare thread-ului curent
            for (int i = start; i < end; i++) {
                this.map = new HashMap<>();
                this.maxLenWords = new ArrayList<>();
                this.maxLen = 0;

                // preia un task
                MapTask crtTask = tasks.get(i);

                // citește fragmentul de dimensiune D din document
                String fragment = getFragment(crtTask);

                // construiește dicționarul și lista de cuvinte de lungime maximă
                compute(fragment, crtTask);

                // salvează rezultatul operației de map
                saveResult(new MapResult(crtTask.getDocName(), map, maxLenWords));
            }

            in.close();
        } catch (Exception e) {
            System.out.println(e.getMessage());
            e.printStackTrace();
        }
    }

    // citește fragmentul de dimensiune D din document
    private String getFragment(MapTask crtTask) throws IOException {
        // pentru a nu deschide același fișier de mai multe ori decât este cazul,
        // fișierul va fi deschis numai dacă este diferit față de cel de la pasul anterior
        if (crtDoc == null || in == null || !crtDoc.equals(crtTask.getDocName())) {
            if (in != null) {
                in.close();
            }
            in = new RandomAccessFile(crtTask.getDocName(), "r");
            crtDoc = crtTask.getDocName();
        }

        // se determină dimensiunea fragmentului ce urmează să fie preluat
        int size = Math.min(crtTask.getSize(), (int) in.length() - crtTask.getOffset());

        // și offset-ul la care acesta se găsește în cadrul documentului
        int off = crtTask.getOffset();
        if (off != 0) {
            off--;
            size++;
        }
        byte[] buf = new byte[size];
        in.seek(off);
        in.readFully(buf);

        return new String(buf);
    }

    private int offsetGiver(MapTask task) {
        int off = task.getOffset();
        if (off != 0) {
            off--;
        }
        return off;
    }

    // construiește dicționarul și lista de cuvinte de lungime maximă
    private void compute(String fragment, MapTask crtTask) throws IOException {
        String delim = ";:/? ̃\\.,><‘[]{}()!@#$%ˆ&- +’=*”| \t\n";
        String[] words = fragment.split("[]});:/?~\\\\.,><`\\[{(!@#$%^&_+'=*\"| \t\r\n-]+");
        int st = 0;
        if (crtTask.getOffset() != 0 && delim.indexOf(fragment.charAt(0)) == -1) {
            st = 1;
        }
        for (int j = st; j < words.length - 1; j++) {
            updateResult(words[j]);
        }

        // preia ultimul cuvânt
        if (st != 1 && words.length > 0 || words.length > 1) {
            int off = offsetGiver(crtTask);
            int lastWord = fragment.lastIndexOf(words[words.length - 1]) + off;
            byte[] buf2 = new byte[(int) in.length() - lastWord];
            in.seek(lastWord);
            in.readFully(buf2);
            fragment = new String(buf2);
            String[] words2 = fragment.split("[]});:/?~\\\\.,><`\\[{(!@#$%^&_+'=*\"| \t\r\n-]+");
            updateResult(words2[0]);
        }
    }

    // actualizează informațiile din dicționar și din lista cu cuvinte de lungime maximă
    // astfel încât acestea să includă și cuvântul primit ca parametru
    private void updateResult(String word) {
        if (word.length() < 1) {
            return;
        }
        if (map.containsKey(word.length())) {
            map.replace(word.length(), map.get(word.length()) + 1);
        } else {
            map.put(word.length(), 1);
        }

        if (word.length() > maxLen) {
            maxLen = word.length();
            maxLenWords.clear();
            maxLenWords.add(word);
        } else if (word.length() == maxLen) {
            maxLenWords.add(word);
        }
    }

    // salvează rezultatul operației de map
    private void saveResult(MapResult result) {
        try {
            semaphore.acquire();
        } catch (Exception e){
            System.out.println(e.getMessage());
            e.printStackTrace();
        }
        if (results.containsKey(result.getDocName())) {
            ArrayList<MapResult> aux = results.get(result.getDocName());
            aux.add(result);
            results.replace(result.getDocName(), aux);
        } else {
            ArrayList<MapResult> aux = new ArrayList<>();
            aux.add(result);
            results.put(result.getDocName(), aux);
        }
        semaphore.release();
    }
}
