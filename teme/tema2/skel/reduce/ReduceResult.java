package reduce;

/**
 * Conține rezultatele executării operației de reduce
 * pornind de la unul dintre task-urile de reduce disponibile
 */
public class ReduceResult {
    private String docName;
    private Double rank;
    private Integer maxLen;
    private Integer numberOfMaxLen;

    public ReduceResult(String docName, Double rank, Integer maxLen, Integer numberOfMaxLen) {
        this.docName = docName;
        this.rank = rank;
        this.maxLen = maxLen;
        this.numberOfMaxLen = numberOfMaxLen;
    }

    public String getDocName() {
        return docName;
    }

    public void setDocName(String docName) {
        this.docName = docName;
    }

    public Double getRank() {
        return rank;
    }

    public void setRank(Double rank) {
        this.rank = rank;
    }

    public Integer getMaxLen() {
        return maxLen;
    }

    public void setMaxLen(Integer maxLen) {
        this.maxLen = maxLen;
    }

    public Integer getNumberOfMaxLen() {
        return numberOfMaxLen;
    }

    public void setNumberOfMaxLen(Integer numberOfMaxLen) {
        this.numberOfMaxLen = numberOfMaxLen;
    }
}
