package ru.nsu.ccfit.thenemez.carfactory.storages;

import ru.nsu.ccfit.thenemez.carfactory.FillListener;
import ru.nsu.ccfit.thenemez.carfactory.details.Engine;


public class EngineStorage extends Storage<Engine> {
    public EngineStorage(int storageCapacity, FillListener listener){
        super(storageCapacity,listener);
    }
}
