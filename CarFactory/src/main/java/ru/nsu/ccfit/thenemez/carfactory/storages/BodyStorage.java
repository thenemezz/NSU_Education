package ru.nsu.ccfit.thenemez.carfactory.storages;

import ru.nsu.ccfit.thenemez.carfactory.FillListener;
import ru.nsu.ccfit.thenemez.carfactory.details.Body;

public class BodyStorage extends Storage<Body>{

    public BodyStorage(int storageCapacity, FillListener listener){
        super(storageCapacity,listener);
    }
}
