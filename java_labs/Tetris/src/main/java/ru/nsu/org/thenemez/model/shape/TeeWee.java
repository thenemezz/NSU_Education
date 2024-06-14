package ru.nsu.org.thenemez.model.shape;

import ru.nsu.org.thenemez.model.TetrisShape;

//@AnnotationShape
public class TeeWee extends TetrisShape {
    public TeeWee() {
        super(new boolean[][] {
                {true, false},
                {true, true},
                {true, false}
        });
    }
}
