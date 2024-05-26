package ru.nsu.ccfit.thenemez.carfactory.storages;

import ru.nsu.ccfit.thenemez.carfactory.FillListener;
import ru.nsu.ccfit.thenemez.carfactory.details.Auto;

public class AutoStorage extends Storage<Auto> {
    public AutoStorage(int storageCapacity, FillListener listener){
        super(storageCapacity,listener);
    }

}
