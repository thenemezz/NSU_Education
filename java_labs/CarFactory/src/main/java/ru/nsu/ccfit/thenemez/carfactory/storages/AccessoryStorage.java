package ru.nsu.ccfit.thenemez.carfactory.storages;

import ru.nsu.ccfit.thenemez.carfactory.FillListener;
import ru.nsu.ccfit.thenemez.carfactory.details.Accessories;

public class AccessoryStorage extends Storage<Accessories>{
    public AccessoryStorage(int storageCapacity, FillListener listener) {
        super(storageCapacity, listener);
    }
}
