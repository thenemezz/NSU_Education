package ru.nsu.thenemez.Logging;

import java.io.FileInputStream;
import java.util.logging.Level;
import java.util.logging.LogManager;
import java.util.logging.Logger;
public class MyLogger {
    private static final Logger LOGGER = Logger.getLogger(Logger.class.getName());

    static {
        try (FileInputStream configFile = new FileInputStream("/Users/thenemez/Desktop/proga/java_labs/StackCalculator/src/main/java/ru/nsu/thenemez/Logging/LogConfig")) {
            LogManager.getLogManager().readConfiguration(configFile);
            LOGGER.log(Level.CONFIG, "Logger configuration loaded.\n");
        } catch (Exception e) {
            LOGGER.log(Level.SEVERE, "Error loading logger configuration file. Thrown exception: " + e + "\n");
            throw new RuntimeException("Error loading logger configuration file");
        }
    }
    public static Logger getLogger() {
        return LOGGER;
    }
}
