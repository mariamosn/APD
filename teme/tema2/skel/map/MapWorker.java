package map;

import java.io.RandomAccessFile;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

public class MapWorker implements Runnable {
    private Integer id;
    private Integer numOfWorkers;
    private List<MapResult> results;
    private ArrayList<MapTask> tasks;
    private Integer start;
    private Integer end;

    private HashMap<Integer, Integer> map;
    private ArrayList<String> maxLenWords;
    private Integer maxLen;

    private RandomAccessFile in = null;
    private String crtDoc = null;

    private String delim = ";:/? ̃\\.,><‘[]{}()!@#$%ˆ&- +’=*”| \t\n";

    public MapWorker(Integer id, ArrayList<MapTask> tasks, Integer numOfWorkers, List<MapResult> results) {
        this.id = id;
        this.tasks = tasks;
        this.numOfWorkers = numOfWorkers;
        this.results = results;
        start = (int) (id * (double)tasks.size() / numOfWorkers);
        end = (int) ((id + 1) * (double)tasks.size() / numOfWorkers);
        if (end > tasks.size()) {
            end = tasks.size();
        }
    }

    @Override
    public void run() {
        try {
            for (int i = start; i < end; i++) {
                this.map = new HashMap<>();
                this.maxLenWords = new ArrayList<>();
                this.maxLen = 0;
                String fragment;
                // preia un task
                MapTask crtTask = tasks.get(i);

                // citeste fragmentul de dim D din doc
                if (crtDoc == null || in == null || !crtDoc.equals(crtTask.getDocName())) {
                    if (in != null) {
                        in.close();
                    }
                    in = new RandomAccessFile(crtTask.getDocName(), "r");
                    crtDoc = crtTask.getDocName();
                }

                int size = Math.min(crtTask.getSize(), (int) in.length() - crtTask.getOffset());
                int off = crtTask.getOffset();
                if (off != 0) {
                    off--;
                    size++;
                }
                byte[] buf = new byte[size];
                in.seek(off);
                in.readFully(buf);
                fragment = new String(buf);

                // construieste dictionarul si lista de cuvinte de lungime maxima
                String[] words = fragment.split("[]});:/?~\\\\.,><`\\[{(!@#$%^&_+'=*\"| \t\r\n-]+");
                int st = 0;
                if (crtTask.getOffset() != 0 && delim.indexOf(fragment.charAt(0)) == -1) {
                    st = 1;
                }
                for (int j = st; j < words.length - 1; j++) {
                    updateResult(words[j]);
                }

                // preia ultimul cuvant
                if (st != 1 && words.length > 0 || words.length > 1) {
                    int lastWord = fragment.lastIndexOf(words[words.length - 1]) + off;
                    byte[] buf2 = new byte[(int) in.length() - lastWord];
                    in.seek(lastWord);
                    in.readFully(buf2);
                    fragment = new String(buf2);
                    String[] words2 = fragment.split("[]});:/?~\\\\.,><`\\[{(!@#$%^&_+'=*\"| \t\r\n-]+");
                    updateResult(words2[0]);
                }

                // pune rezultatul intr-un MapResult
                MapResult result = new MapResult(crtTask.getDocName(), map, maxLenWords);
                results.add(result);
            }

            in.close();
        } catch (Exception e) {
            System.out.println(e.getMessage());
            e.printStackTrace();
        }
    }

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
}
