import map.MapCoordinator;

import java.io.*;
import java.util.*;

public class Tema2 {
    public static void main(String[] args) {
        if (args.length < 3) {
            System.err.println("Usage: Tema2 <workers> <in_file> <out_file>");
            return;
        }

        try {
            File fileIn = new File(args[1]);
            Scanner in = new Scanner(fileIn);

            int size = in.nextInt();
            // System.out.println(size);

            int numOfDocs = in.nextInt();
            // System.out.println(numOfDocs);

            ArrayList<String> docs = new ArrayList<>();
            for (int i = 1; i <= numOfDocs; i++) {
                String crt = in.next();
                // System.out.println(crt);
                docs.add(crt);
            }

            Integer numOfWorkers = Integer.parseInt(args[0]);

            MapCoordinator coordinator = new MapCoordinator(numOfWorkers, docs, size, numOfDocs);
            coordinator.map();
        } catch (Exception e) {
            System.out.println(e.getMessage());
            e.printStackTrace();
        }

    }
}
