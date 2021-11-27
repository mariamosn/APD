package reduce;

import map.MapResult;

import java.util.ArrayList;

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

    // cred ca o sa aiba o lista de MapResult pentru un anumit document
    // combina toate dictionarele
    // face lista cu cuvintele de lungime maxima
    // calculeaza rangul documentului curent
    // determina numarul de cuvinte de lungime maxima
}
