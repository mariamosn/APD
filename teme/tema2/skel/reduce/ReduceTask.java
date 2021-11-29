package reduce;

import map.MapResult;

import java.util.ArrayList;

/**
 * Conține datele corespunzătoare unui task de reduce
 */
public class ReduceTask {
    private String docName;
    private ArrayList<MapResult> mapResults;

    public ReduceTask(String docName, ArrayList<MapResult> mapResults) {
        this.docName = docName;
        this.mapResults = mapResults;
    }

    public String getDocName() {
        return docName;
    }

    public void setDocName(String docName) {
        this.docName = docName;
    }

    public ArrayList<MapResult> getMapResults() {
        return mapResults;
    }

    public void setMapResults(ArrayList<MapResult> mapResults) {
        this.mapResults = mapResults;
    }
}
