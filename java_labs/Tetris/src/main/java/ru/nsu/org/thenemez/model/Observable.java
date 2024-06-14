package ru.nsu.org.thenemez.model;

import ru.nsu.org.thenemez.view.tetris_area.Observer;

public interface Observable {
    void registerObserver(Observer o);
    void notifyObservers();
}