package thenemez.nsu;

import thenemez.nsu.server.NonBlockableServer;

public class Main {
    public static void main(String[] args) {
        System.out.println();
        NonBlockableServer server = new NonBlockableServer("127.0.0.1", 8080);
        System.out.println("IP: 127.0.0.1 , Port: 8080 ready to use");
        server.start();
    }
}