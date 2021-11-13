package barber;

public class Barber extends Thread {
    @Override
    public void run() {
        int servedClients = 0;

        do {
            // TODO
            try {
                Main.clientsSem.acquire();
            } catch (Exception e) {

            }

            try {
                Main.chairsSem.acquire();
            } catch (Exception e) {

            }

            Main.chairs++;

            // TODO
            Main.barberReady.release();
            Main.chairsSem.release();

            try {
                Thread.sleep(100);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

            System.out.println("Barber served client");
            ++servedClients;

        } while (checkIfThereAnyClients());
    }

    private boolean checkIfThereAnyClients() {
        int count = 0;
        for (var client: Main.leftClients) {
            if (client == 0) {
                count++;
            }
        }

        return count != 0;
    }
}
