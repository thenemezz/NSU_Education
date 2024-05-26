package ru.nsu.ccfit.thenemez.carfactory.threadPool;

import ru.nsu.ccfit.thenemez.carfactory.FactoryProperties;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class ThreadPool {
    public ExecutorService executorService;

    //создаем исполнителя с фиксированным числом потоков
    public ThreadPool() {
        this.executorService = Executors.newFixedThreadPool(FactoryProperties.NUMBER_OF_WORKERS);
    }

    public void killThreadPool() {
        executorService.shutdown();
    }
}
