package ru.nsu.org.thenemez.model.shape;

import ru.nsu.org.thenemez.model.TetrisShape;

//@AnnotationShape
public class Hero extends TetrisShape {
    public Hero() {
        super(new boolean[][]{
                {true},
                {true},
                {true},
                {true},
        });
    }

    @Override
    public void rotate() {
        super.rotate();
        if(this.getWidth() == 1) {
            this.setX(this.getX() + 1);
            this.setY(this.getY() - 1);
        } else {
            this.setX(this.getX() - 1);
            this.setY(this.getY() + 1);
        }

    }
}