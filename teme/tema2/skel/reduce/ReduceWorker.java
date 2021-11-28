package reduce;

import map.MapResult;

import java.awt.*;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.Semaphore;

public class ReduceWorker implements Runnable {
    private Integer id;
    private Integer numOfWorkers;
    private ArrayList<ReduceTask> tasks;
    private Integer start;
    private Integer end;
    private HashMap<Integer, Integer> map;
    private ArrayList<String> maxLenWords;
    private Integer maxLen;
    private Integer wordCnt;
    // private List<ReduceResult> results;
    private Map<String, ReduceResult> results;
    private List<Integer> fibonacci;
    private Semaphore semaphore;

    public ReduceWorker(Integer id, Integer numOfWorkers, ArrayList<ReduceTask> tasks, Map<String,
                        ReduceResult> results, List<Integer> fibonacci, Semaphore semaphore) {
        this.id = id;
        this.numOfWorkers = numOfWorkers;
        this.tasks = tasks;
        this.results = results;
        this.fibonacci = fibonacci;
        this.semaphore = semaphore;
        start = (int) (id * (double)tasks.size() / numOfWorkers);
        end = (int) ((id + 1) * (double)tasks.size() / numOfWorkers);
        if (end > tasks.size()) {
            end = tasks.size();
        }
    }

    @Override
    public void run() {
        for (int i = start; i < end; i++) {
            ReduceTask crtTask = tasks.get(i);

            // combine stage
            map = crtTask.getMapResults().get(0).getMap();
            maxLenWords = crtTask.getMapResults().get(0).getMaxLenWords();
            if (maxLenWords.size() == 0) {
                maxLen = 0;
            } else {
                maxLen = maxLenWords.get(0).length();
            }
            wordCnt = 0;

            for (int j = 1; j < crtTask.getMapResults().size(); j++) {
                MapResult result = crtTask.getMapResults().get(j);

                // update hashmap
                for (Integer len : result.getMap().keySet()) {
                    if (map.containsKey(len)) {
                        map.replace(len, map.get(len) + result.getMap().get(len));
                    } else {
                        map.put(len, result.getMap().get(len));
                    }
                }

                // update list
                if (result.getMaxLenWords().size() > 0 && result.getMaxLenWords().get(0).length() > maxLen) {
                    maxLenWords = result.getMaxLenWords();
                    maxLen = maxLenWords.get(0).length();
                } else if (result.getMaxLenWords().size() > 0 && result.getMaxLenWords().get(0).length() == maxLen) {
                    maxLenWords.addAll(result.getMaxLenWords());
                }
            }

            // processing stage

            // rank
            Double rank = 0.0;
            for (Integer len : map.keySet()) {
                Integer fib = 0;
                try {
                    semaphore.acquire();
                } catch (Exception e) {
                    System.out.println(e.getMessage());
                    e.printStackTrace();
                }
                while (fibonacci.size() <= len + 1) {
                    fibonacci.add(fibonacci.get(fibonacci.size() - 1) + fibonacci.get(fibonacci.size() - 2));
                }
                semaphore.release();
                fib = fibonacci.get(len + 1);
                rank += fib * map.get(len);
                wordCnt += map.get(len);
            }
            rank /= wordCnt;

            // max len already done

            // results.put(crtTask.getDocName(), new ReduceResult(crtTask.getDocName(), rank, maxLen, maxLenWords.size()));
            results.put(crtTask.getDocName(), new ReduceResult(crtTask.getDocName(), rank, maxLen, map.get(maxLen)));
        }
    }
}
