package ru.nsu.ccfit.thenemez.carfactory.dealers;

import ru.nsu.ccfit.thenemez.carfactory.FactoryProperties;
import ru.nsu.ccfit.thenemez.carfactory.details.Auto;
import ru.nsu.ccfit.thenemez.carfactory.storages.AutoStorage;

import java.io.FileInputStream;
import java.util.concurrent.TimeUnit;
import java.util.logging.Level;
import java.util.logging.LogManager;
import java.util.logging.Logger;

public class Dealer extends Thread {
    private final AutoStorage autoStorage;
    private Logger LOGGER;
    private Double sleepTime;

    public Dealer(AutoStorage autoStorage) {
        this.autoStorage = autoStorage;
        this.sleepTime = 1.0;
        if (FactoryProperties.LOGGING_ENABLED) {
            setupLogger();
        }
    }

    private void setupLogger() {
        try (FileInputStream configFile = new FileInputStream("src/main/resources/ru/nsu/ccfit/thenemez/carfactory/LoggerProperties.txt")) {
            LogManager.getLogManager().readConfiguration(configFile);
            LOGGER = Logger.getLogger(Dealer.class.getName());
        } catch (Exception e) {
            throw new RuntimeException("Error loading logger configuration file", e);
        }
    }

    public void setSleepTime(Double sleepTime) {
        this.sleepTime = sleepTime;
    }

    @Override
    public void run() {
        try {
            while (!Thread.currentThread().isInterrupted()) {
                Auto newAuto = autoStorage.get();
                TimeUnit.SECONDS.sleep(sleepTime.intValue());
                System.out.println("Buy new car: " + newAuto.getAutoInfo());
                if (LOGGER != null) {
                    LOGGER.log(Level.INFO, "Buy new car: " + newAuto.getAutoInfo());
                }
            }
        } catch (InterruptedException e) {
            System.err.println(e.getMessage());
            Thread.currentThread().interrupt();
        }
    }
}
