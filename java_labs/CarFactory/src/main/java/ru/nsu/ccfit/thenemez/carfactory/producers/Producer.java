package ru.nsu.ccfit.thenemez.carfactory.producers;

import ru.nsu.ccfit.thenemez.carfactory.details.Detail;
import ru.nsu.ccfit.thenemez.carfactory.storages.Storage;

import java.lang.reflect.InvocationTargetException;
import java.util.concurrent.TimeUnit;

public class Producer<S extends Storage<D>, D extends Detail> extends Thread{
    protected final S storage;
    protected Double sleepTime;
    protected final Class<D> clazz;
    public Producer(S storage, double sleepTime, Class<D> clazz){
        this.storage = storage;
        this.sleepTime = sleepTime;
        this.clazz = clazz;
    }

    public void setSleepTime(Double sleepTime) {
        this.sleepTime = sleepTime;
    }

    @Override
    public void run() {
        try {
            while (!Thread.currentThread().isInterrupted()) {
                storage.put(clazz.getDeclaredConstructor().newInstance());
                TimeUnit.SECONDS.sleep(sleepTime.intValue());
            }
        } catch (InterruptedException e) {
            System.err.println(e.getMessage());
        } catch (NoSuchMethodException | InvocationTargetException | InstantiationException | IllegalAccessException e) {
            throw new RuntimeException(e);
        }
    }
}
