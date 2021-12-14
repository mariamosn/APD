// Maria Moșneag 333CA

package reduce;

import map.MapResult;

import java.util.ArrayList;

/**
 * Conține datele corespunzătoare unui task de reduce
 */
public class ReduceTask {
    private final String docName;
    private final ArrayList<MapResult> mapResults;

    public ReduceTask(String docName, ArrayList<MapResult> mapResults) {
        this.docName = docName;
        this.mapResults = mapResults;
    }

    public String getDocName() {
        return docName;
    }

    public ArrayList<MapResult> getMapResults() {
        return mapResults;
    }
}
