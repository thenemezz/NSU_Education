package ru.nsu.ccfit.thenemez.carfactory;

import java.util.concurrent.atomic.AtomicInteger;

public interface FillListener {
    void updateFill(AtomicInteger fill, String clazz);
}
