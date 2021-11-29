package map_reduce_generic;

/**
 * Coordonatorul operațiilor Map-Reduce
 * @param <E> este tipul rezultatului obținut în urma operațiilor de map
 *           pe baza căruia se vor efectua operațiile de reduce
 */
@SuppressWarnings("unchecked")
public class MapReduce<E, MapTaskType> {
    private final Map<E, MapTaskType> mapCoordinator;
    private final Reduce reduceCoordinator;

    public MapReduce(Map<E, MapTaskType> mapCoordinator, Reduce reduceCoordinator) {
        this.mapCoordinator = mapCoordinator;
        this.reduceCoordinator = reduceCoordinator;
    }

    public void mapReduce() {
        E mapResults = mapCoordinator.map();
        reduceCoordinator.reduce(mapResults);
    }
}
