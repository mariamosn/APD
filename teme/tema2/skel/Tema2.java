import map.MapCoordinator;
import map.MapResult;
import map.MapTask;
import map_reduce_generic.MapReduce;
import reduce.ReduceCoordinator;

import java.io.*;
import java.util.*;

public class Tema2 {

    public static void main(String[] args) {
        if (args.length < 3) {
            System.err.println("Usage: Tema2 <workers> <in_file> <out_file>");
            return;
        }
        Integer numOfWorkers = Integer.parseInt(args[0]);
        Integer fragmentSize = 0;
        Integer numOfDocs = 0;
        ArrayList<String> docs = null;

        try {
            File fileIn = new File(args[1]);
            Scanner in = new Scanner(fileIn);

            fragmentSize = in.nextInt();

            numOfDocs = in.nextInt();

            docs = new ArrayList<>();
            for (int i = 1; i <= numOfDocs; i++) {
                String crt = in.next();
                docs.add(crt);
            }
        } catch (Exception e) {
            System.out.println(e.getMessage());
            e.printStackTrace();
        }

        MapCoordinator mapCoord = new MapCoordinator(numOfWorkers, docs, fragmentSize, numOfDocs);
        ReduceCoordinator reduceCoord = new ReduceCoordinator(numOfWorkers, docs, args[2]);

        MapReduce<Map<String, ArrayList<MapResult>>, MapTask> mapReduce = new MapReduce<>(mapCoord, reduceCoord);
        mapReduce.mapReduce();
    }
}
