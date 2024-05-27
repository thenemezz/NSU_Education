package ru.nsu.ccfit.thenemez.carfactory.storages;

import javafx.application.Platform;
import ru.nsu.ccfit.thenemez.carfactory.FillListener;
import ru.nsu.ccfit.thenemez.carfactory.details.Detail;

import java.util.LinkedList;
import java.util.concurrent.atomic.AtomicInteger;

public class Storage<D extends Detail> {

    protected final LinkedList<D> itemList;
    protected final int storageCapacity;
    private final AtomicInteger storageFill;
    private final FillListener listener;

    public Storage(int storageCapacity, FillListener listener) {
        this.storageCapacity = storageCapacity;
        this.listener = listener;
        itemList = new LinkedList<>();
        //https://www.digitalocean.com/community/tutorials/atomicinteger-java
        storageFill = new AtomicInteger(0);

    }
    //WAIT
    public synchronized void put(D newDetail) {
        while (itemList.size() >= storageCapacity) {
            try {
                wait();
            } catch (InterruptedException e) {
                System.err.println(e.getMessage());
            }
        }
        itemList.add(newDetail);
        storageFill.incrementAndGet();//this, value++
        Platform.runLater(() -> listener.updateFill(storageFill, this.getClass().getSimpleName()));
        System.out.println("Put new " + newDetail.getClass().getSimpleName() + ", id: " + newDetail.getID() + " fill:" + storageFill.intValue());
        notifyAll();
    }
    //WAIT
    public synchronized D get() {
        while (itemList.isEmpty()) {
            try {
                wait();
            } catch (InterruptedException e) {
                System.err.println(e.getMessage());
            }
        }
        D retItem = itemList.removeFirst();
        storageFill.decrementAndGet();
        //https://stackoverflow.com/questions/13784333/platform-runlater-and-task-in-javafx
        Platform.runLater(() -> listener.updateFill(storageFill, this.getClass().getSimpleName()));
        notifyAll();
        return retItem;
    }

}
