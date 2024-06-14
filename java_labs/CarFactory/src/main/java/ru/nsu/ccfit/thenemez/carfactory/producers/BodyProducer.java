package ru.nsu.ccfit.thenemez.carfactory.producers;

import ru.nsu.ccfit.thenemez.carfactory.details.Body;
import ru.nsu.ccfit.thenemez.carfactory.storages.BodyStorage;

public class BodyProducer extends Producer<BodyStorage, Body> {
    public BodyProducer(BodyStorage bodyStorage) {
        super(bodyStorage, 1.0, Body.class);
    }

}
