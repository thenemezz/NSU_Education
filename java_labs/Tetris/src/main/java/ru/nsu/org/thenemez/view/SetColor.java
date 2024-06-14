package ru.nsu.org.thenemez.view;

import java.awt.*;

public enum SetColor {
    GREEN_START(new Color(143, 255, 59)),
    GREEN_END(new Color(78, 149, 118)),
    GOLD_START(new Color(212, 232, 124)),
    GOLD_END (new Color(120, 123, 90));
    private Color color;

    SetColor(Color color) {
        this.color = color;
    }

    public Color get() {
        return color;
    }
}
