package ru.nsu.ccfit.thenemez.carfactory;

import java.io.*;

public class FactoryProperties {
    public static int ENGINE_STORAGE_SIZE = 0;
    public static int BODY_STORAGE_SIZE = 0;
    public static int ACCESSORY_STORAGE_SIZE = 0;
    public static int NUMBER_OF_WORKERS = 0;
    public static int AUTO_STORAGE_SIZE = 0;
    public static boolean LOGGING_ENABLED = true;
    public static double ENGINE_PRODUCER_SLEEP_TIME = 1.0;
    public static double BODY_PRODUCER_SLEEP_TIME = 1.0;
    public static double ACCESSORY_PRODUCER_SLEEP_TIME = 1.0;
    public static double DEALER_SLEEP_TIME = 1.0;
    public static double AUTO_STORAGE_CONTROLLER_SLEEP_TIME = 1.0;

    public static void readInfo() throws RuntimeException {
        try (BufferedReader file = new BufferedReader(new FileReader("src/main/resources/ru/nsu/ccfit/thenemez/carfactory/FactoryProperties.txt"))) {
            String line;
            while ((line = file.readLine()) != null) {
                String[] parts = line.split(" ");
                switch (parts[0]) {
                    case "ENGINE_STORAGE_SIZE" -> ENGINE_STORAGE_SIZE = Integer.parseInt(parts[1]);
                    case "BODY_STORAGE_SIZE" -> BODY_STORAGE_SIZE = Integer.parseInt(parts[1]);
                    case "ACCESSORY_STORAGE_SIZE" -> ACCESSORY_STORAGE_SIZE = Integer.parseInt(parts[1]);
                    case "NUMBER_OF_WORKERS" -> NUMBER_OF_WORKERS = Integer.parseInt(parts[1]);
                    case "AUTO_STORAGE_SIZE" -> AUTO_STORAGE_SIZE = Integer.parseInt(parts[1]);
                    case "LOGGING_ENABLED" -> LOGGING_ENABLED = Boolean.parseBoolean(parts[1]);
                    case "ENGINE_PRODUCER_SLEEP_TIME" -> ENGINE_PRODUCER_SLEEP_TIME = Double.parseDouble(parts[1]);
                    case "BODY_PRODUCER_SLEEP_TIME" -> BODY_PRODUCER_SLEEP_TIME = Double.parseDouble(parts[1]);
                    case "ACCESSORY_PRODUCER_SLEEP_TIME" -> ACCESSORY_PRODUCER_SLEEP_TIME = Double.parseDouble(parts[1]);
                    case "DEALER_SLEEP_TIME" -> DEALER_SLEEP_TIME = Double.parseDouble(parts[1]);
                    case "AUTO_STORAGE_CONTROLLER_SLEEP_TIME" -> AUTO_STORAGE_CONTROLLER_SLEEP_TIME = Double.parseDouble(parts[1]);
                    default -> throw new IllegalArgumentException("Unknown argument in factory properties");
                }
            }
            if (ENGINE_STORAGE_SIZE <= 0 || BODY_STORAGE_SIZE <= 0 || ACCESSORY_STORAGE_SIZE <= 0
                    || NUMBER_OF_WORKERS <= 0 || AUTO_STORAGE_SIZE <= 0) {
                throw new IllegalArgumentException("Invalid data in factory properties");
            }
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }
}
