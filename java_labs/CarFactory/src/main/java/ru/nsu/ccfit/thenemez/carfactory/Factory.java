package ru.nsu.ccfit.thenemez.carfactory;

import ru.nsu.ccfit.thenemez.carfactory.details.Auto;
import ru.nsu.ccfit.thenemez.carfactory.details.Detail;
import ru.nsu.ccfit.thenemez.carfactory.storages.*;
import ru.nsu.ccfit.thenemez.carfactory.threadPool.Task;
import ru.nsu.ccfit.thenemez.carfactory.threadPool.ThreadPool;

import java.util.LinkedList;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;
import java.util.concurrent.atomic.AtomicInteger;

public class Factory {
    private final LinkedList<Storage<? extends Detail>> detailStorage;
    private final AutoStorage autoStorage;
    private final ThreadPool workers;
    private final AtomicInteger carsMade;
    private final FillListener listener;

    public Factory(EngineStorage engineStorage, BodyStorage bodyStorage, AccessoryStorage accessoryStorage, AutoStorage autoStorage, View view) {
        detailStorage = new LinkedList<>();
        detailStorage.add(engineStorage);
        detailStorage.add(bodyStorage);
        detailStorage.add(accessoryStorage);
        this.autoStorage = autoStorage;
        this.workers = new ThreadPool();
        carsMade =  new AtomicInteger(0);
        listener = view;
    }

    public void createAuto() throws ExecutionException, InterruptedException {
        //https://www.baeldung.com/java-future
            Future<Auto> future = workers.executorService.submit(new Task(detailStorage));
            Auto newAuto = future.get();
            autoStorage.put(newAuto);
            carsMade.incrementAndGet();
            listener.updateFill(carsMade, this.getClass().getSimpleName());
    }

    public ThreadPool getWorkers(){
        return workers;
    }
}
