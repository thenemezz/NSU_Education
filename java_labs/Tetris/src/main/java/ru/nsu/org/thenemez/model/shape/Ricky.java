package ru.nsu.org.thenemez.model.shape;

import ru.nsu.org.thenemez.model.TetrisShape;

//@AnnotationShape
public class Ricky extends TetrisShape {
    public Ricky() {
        super(new boolean[][] {
                {true, true},
                {true, false},
                {true, false},
        });
    }
}
