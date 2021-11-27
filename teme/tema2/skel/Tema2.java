import map.MapCoordinator;
import map.MapResult;
import reduce.ReduceCoordinator;

import java.io.*;
import java.util.*;

public class Tema2 {
    private static Integer fragmentSize;
    private static Integer numOfDocs;
    private static ArrayList<String> docs;
    private static Integer numOfWorkers;
    private static List<MapResult> mapResults;

    public static void main(String[] args) {
        if (args.length < 3) {
            System.err.println("Usage: Tema2 <workers> <in_file> <out_file>");
            return;
        }
        numOfWorkers = Integer.parseInt(args[0]);

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
        mapResults = mapCoord.map();

        ReduceCoordinator reduceCoord = new ReduceCoordinator(mapResults, numOfWorkers, docs, args[2]);
        reduceCoord.reduce();
    }
}
