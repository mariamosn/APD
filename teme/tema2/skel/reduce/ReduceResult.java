// Maria Moșneag 333CA

package reduce;

/**
 * Conține rezultatele executării operației de reduce
 * pornind de la unul dintre task-urile de reduce disponibile
 */
public class ReduceResult {
    private final String docName;
    private final Double rank;
    private final Integer maxLen;
    private final Integer numberOfMaxLen;

    public ReduceResult(String docName, Double rank, Integer maxLen, Integer numberOfMaxLen) {
        this.docName = docName;
        this.rank = rank;
        this.maxLen = maxLen;
        this.numberOfMaxLen = numberOfMaxLen;
    }

    public String getDocName() {
        return docName;
    }

    public Double getRank() {
        return rank;
    }

    public Integer getMaxLen() {
        return maxLen;
    }

    public Integer getNumberOfMaxLen() {
        return numberOfMaxLen;
    }
}
