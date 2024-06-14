package ru.nsu.ccfit.thenemez.carfactory.producers;

import ru.nsu.ccfit.thenemez.carfactory.details.Engine;
import ru.nsu.ccfit.thenemez.carfactory.storages.EngineStorage;


public class EngineProducer extends Producer<EngineStorage, Engine> {

    public EngineProducer(EngineStorage storage) {
        super(storage, 1.0, Engine.class);
    }

}
