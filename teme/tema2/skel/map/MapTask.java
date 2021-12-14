// Maria Moșneag 333CA

package map;

/**
 * Conține datele corespunzătoare unui task de map
 */
public class MapTask {
    private final String docName;
    private final Integer offset;
    private final Integer size;

    public MapTask(String docName, Integer offset, Integer size) {
        this.docName = docName;
        this.offset = offset;
        this.size = size;
    }

    public String getDocName() {
        return docName;
    }

    public Integer getOffset() {
        return offset;
    }

    public Integer getSize() {
        return size;
    }
}
