package map;

// un task de map
public class MapTask {
    private String docName;
    private Integer offset;
    private Integer size;
    private MapResult result;

    public MapTask(String docName, Integer offset, Integer size) {
        this.docName = docName;
        this.offset = offset;
        this.size = size;
        // System.out.println(docName + " " + offset);
    }

    public String getDocName() {
        return docName;
    }

    public void setDocName(String docName) {
        this.docName = docName;
    }

    public Integer getOffset() {
        return offset;
    }

    public void setOffset(Integer offset) {
        this.offset = offset;
    }

    public Integer getSize() {
        return size;
    }

    public void setSize(Integer size) {
        this.size = size;
    }

    public MapResult getResult() {
        return result;
    }

    public void setResult(MapResult result) {
        this.result = result;
    }
}
