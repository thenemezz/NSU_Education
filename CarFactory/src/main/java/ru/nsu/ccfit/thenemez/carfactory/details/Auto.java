package ru.nsu.ccfit.thenemez.carfactory.details;


import java.util.*;
@SuppressWarnings({"FieldCanBeLocal", "MismatchedQueryAndUpdateOfCollection"})
public class Auto extends Detail {
    private final HashMap<String, Detail> details;
    private final String autoInfo;
    public Auto(LinkedList<Detail> detail) {
        super();
        details = new HashMap<>();
        StringBuilder stringBuilder = new StringBuilder("Auto: " + this.getID());
        for (Detail det : detail) {
            this.details.put(det.getClass().getSimpleName(), det);
            stringBuilder.append(", ").append(det.getClass().getSimpleName()).append(" ").append(det.getID());
        }

        autoInfo = String.valueOf(stringBuilder);

    }
    public String getAutoInfo() {
        return autoInfo;
    }

}
