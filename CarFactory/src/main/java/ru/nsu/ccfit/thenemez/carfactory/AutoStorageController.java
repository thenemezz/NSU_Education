package ru.nsu.ccfit.thenemez.carfactory;

import ru.nsu.ccfit.thenemez.carfactory.storages.AutoStorage;

import java.util.Arrays;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.TimeUnit;

public class AutoStorageController extends Thread {
    private final Factory factory;
    private  Double sleepTime;

    public AutoStorageController(Factory factory, AutoStorage autoStorage) {
        this.factory = factory;
        sleepTime = 1.0;
    }

    public void setSleepTime(Double sleepTime) {
        this.sleepTime = sleepTime;
    }

    @Override
    public void run() {
            try {
                while (!Thread.currentThread().isInterrupted()) {
                    factory.createAuto();
                    TimeUnit.MILLISECONDS.sleep((long) (sleepTime * 1000));
                }
            } catch (InterruptedException e) {
                System.out.println(Arrays.toString(e.getStackTrace()));
                Thread.currentThread().interrupt();
            } catch (ExecutionException e) {
                throw new RuntimeException(e);
            }
    }
}
