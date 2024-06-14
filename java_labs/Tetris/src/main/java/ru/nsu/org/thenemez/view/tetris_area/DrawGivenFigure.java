package ru.nsu.org.thenemez.view.tetris_area;

import ru.nsu.org.thenemez.model.Context;

import java.awt.*;

public class DrawGivenFigure {
    public static void draw(Graphics graphics, Color color, int x, int y) {
        graphics.setColor(color);
        graphics.fillRect(x * Context.getOFFSET_SHARED() + Context.getOFFSET_TABLE_X(),
                y * Context.getOFFSET_SHARED() + Context.getOFFSET_TABLE_Y(),
                Context.getOFFSET_SHARED(),
                Context.getOFFSET_SHARED());

        graphics.setColor(Color.BLACK);
        graphics.drawRect(x * Context.getOFFSET_SHARED() + Context.getOFFSET_TABLE_X(),
                y * Context.getOFFSET_SHARED() + Context.getOFFSET_TABLE_Y(),
                Context.getOFFSET_SHARED(),
                Context.getOFFSET_SHARED());
    }
}