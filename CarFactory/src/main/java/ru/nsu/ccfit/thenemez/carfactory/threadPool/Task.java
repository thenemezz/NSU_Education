package ru.nsu.ccfit.thenemez.carfactory.threadPool;

import ru.nsu.ccfit.thenemez.carfactory.details.Auto;
import ru.nsu.ccfit.thenemez.carfactory.details.Detail;
import ru.nsu.ccfit.thenemez.carfactory.storages.Storage;

import java.util.LinkedList;
import java.util.concurrent.Callable;


//Difference mejdu Runnable & Callable ->  https://www.baeldung.com/java-runnable-callable -> возвращает тип Auto + ..
public class Task implements Callable<Auto> {
    private final LinkedList<Storage<? extends Detail>> detailStorages;

    public Task(LinkedList<Storage<? extends Detail>> inpDetailStorage) {
        this.detailStorages = new LinkedList<>(inpDetailStorage);
    }

    @Override
    public Auto call() {
        LinkedList<Detail> details = new LinkedList<>();
        for (Storage<? extends Detail> str : detailStorages) {
            details.add(str.get());
        }
        return new Auto(details);
    }
}
