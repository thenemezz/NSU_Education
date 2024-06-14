package ru.nsu.org.thenemez.model.shape;

import ru.nsu.org.thenemez.model.TetrisShape;

//@AnnotationShape
public class Cleveland extends TetrisShape {
    public Cleveland() {
        super(new boolean[][] {
                {false, true},
                {true, true},
                {true, false},
        });
    }
}