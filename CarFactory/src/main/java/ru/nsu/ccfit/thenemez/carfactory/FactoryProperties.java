package ru.nsu.ccfit.thenemez.carfactory;

import java.io.*;

public class FactoryProperties {
    public static int ENGINE_STORAGE_SIZE = 0;
    public static int BODY_STORAGE_SIZE = 0;
    public static int ACCESSORY_STORAGE_SIZE = 0;
    public static int NUMBER_OF_WORKERS = 0;
    public static int AUTO_STORAGE_SIZE = 0;

    public static void readInfo() throws RuntimeException {
        try (BufferedReader file = new BufferedReader(new FileReader("src/main/resources/ru/nsu/ccfit/thenemez/carfactory/FactoryProperties.txt"))) {
            String line;
            while ((line = file.readLine()) != null) {
                String[] parts = line.split(" ");
                if (Integer.parseInt(parts[1]) <= 0) {
                    throw new IllegalArgumentException("Invalid data in factory properties");
                } else switch (parts[0]) {
                    case "ENGINE_STORAGE_SIZE" -> ENGINE_STORAGE_SIZE = Integer.parseInt(parts[1]);
                    case "BODY_STORAGE_SIZE" -> BODY_STORAGE_SIZE = Integer.parseInt(parts[1]);
                    case "ACCESSORY_STORAGE_SIZE" -> ACCESSORY_STORAGE_SIZE = Integer.parseInt(parts[1]);
                    case "NUMBER_OF_WORKERS" -> NUMBER_OF_WORKERS = Integer.parseInt(parts[1]);
                    case "AUTO_STORAGE_SIZE" -> AUTO_STORAGE_SIZE = Integer.parseInt(parts[1]);
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
