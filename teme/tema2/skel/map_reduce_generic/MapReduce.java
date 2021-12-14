// Maria Moșneag 333CA

package map_reduce_generic;

/**
 * Coordonatorul operațiilor Map-Reduce
 * @param <E> este tipul rezultatului obținut în urma operațiilor de map
 *           pe baza căruia se vor efectua operațiile de reduce
 */
public class MapReduce<E, MapTaskType> {
    private final Map<E, MapTaskType> mapCoordinator;
    private final Reduce<E> reduceCoordinator;

    public MapReduce(Map<E, MapTaskType> mapCoordinator, Reduce<E> reduceCoordinator) {
        this.mapCoordinator = mapCoordinator;
        this.reduceCoordinator = reduceCoordinator;
    }

    public void mapReduce() {
        E mapResults = mapCoordinator.map();
        reduceCoordinator.reduce(mapResults);
    }
}
