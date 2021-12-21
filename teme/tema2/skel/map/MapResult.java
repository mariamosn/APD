// Maria Moșneag 333CA

package map;

import java.util.ArrayList;
import java.util.HashMap;

/**
 * Conține rezultatele executării operației de map
 * pornind de la unul dintre task-urile de map disponibile
 */
public class MapResult {
    private final String docName;
    private HashMap<Integer, Integer> map;
    private final ArrayList<String> maxLenWords;

    public MapResult(String docName, HashMap<Integer, Integer> map, ArrayList<String> maxLenWords) {
        this.docName = docName;
        this.map = map;
        this.maxLenWords = maxLenWords;
    }

    public String getDocName() {
        return docName;
    }

    public HashMap<Integer, Integer> getMap() {
        return map;
    }

    public void setMap(HashMap<Integer, Integer> map) {
        this.map = map;
    }

    public ArrayList<String> getMaxLenWords() {
        return maxLenWords;
    }
}
