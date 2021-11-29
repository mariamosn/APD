package map;

/**
 * Conține datele corespunzătoare unui task de map
 */
public class MapTask {
    private String docName;
    private Integer offset;
    private Integer size;

    public MapTask(String docName, Integer offset, Integer size) {
        this.docName = docName;
        this.offset = offset;
        this.size = size;
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
}
