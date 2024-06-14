package ru.nsu.ccfit.thenemez.carfactory.producers;

import ru.nsu.ccfit.thenemez.carfactory.details.Accessories;
import ru.nsu.ccfit.thenemez.carfactory.storages.AccessoryStorage;

public class AccessoryProducer extends Producer<AccessoryStorage, Accessories> {
    public AccessoryProducer(AccessoryStorage accessoryStorage) {
        super(accessoryStorage, 1.0, Accessories.class);
    }
}
