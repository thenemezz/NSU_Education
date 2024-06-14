package ru.nsu.org.thenemez.model;

import java.util.Random;
import java.awt.Color;

import static org.apache.commons.lang3.ArrayUtils.contains;

public class TetrisShape {
    private Color color;
    private int x = 0;
    private int y = 0;
    private final int CNT_ROTATION_SHIP = 4;
    private int indexRotation = 0;
    private static int indexColor = 0;
    private static Random random = new Random();
    boolean[][][] rotateShapes;
    private boolean[][] shape;
    private static Color[] colors = new Color[]{
            Color.RED,
            Color.BLUE,
            Color.BLACK,
            Color.YELLOW,
            Color.PINK,
            Color.CYAN,
            Color.MAGENTA,
            Color.WHITE,
            Color.GREEN,
            Color.ORANGE
    };
    private static int[] numbers = new int[colors.length];

    static {
        fillColorArray();
    }

    public TetrisShape(boolean[][] shape) {
        this.shape = shape;
        generateRotateShapes();
    }

    private static void fillColorArray() {
        int number;
        for (int i = 0; i < 10; ++i) {
            numbers[i] = -1;
        }
        for (int i = 0; i < 10; i++) {
            do {
                number = random.nextInt(colors.length);
            } while (contains(numbers, number));
            numbers[i] = number;
        }
    }

    private void generateRotateShapes() {
        rotateShapes = new boolean[CNT_ROTATION_SHIP][][];
        for (int i = 0, h, w; i < CNT_ROTATION_SHIP; ++i) {
            h = getHeight();
            w = getWidth();
            rotateShapes[i] = new boolean[h][w];
            for (int y = 0; y < h; ++y) {
                for (int x = 0; x < w; ++x) {
                    rotateShapes[i][y][x] = shape[w - x - 1][y];
                }
            }
            shape = rotateShapes[i];
        }
    }

    public void setColor() {
        this.color = colors[numbers[indexColor % colors.length]];
        ++indexColor;
    }


    public void  shapeDropped() {
        y =  Context.getHEIGHT() / Context.getRATE_VALUE() - getHeight() - 1;
    }
    public int getWidth() {
        return shape.length;
    }

    public int getHeight() {
        return shape[0].length;
    }

    public boolean IsShape(int x, int y) {
        return shape[x][y];
    }

    public Color getColor() {
        return this.color;
    }

    public void rotate() {
        indexRotation = random.nextInt(rotateShapes.length);
        shape = rotateShapes[indexRotation];
    }

    public void spawn() {
        x = random.nextInt(10);
        y = 0;
    }

    public int getX() {
        return x;
    }

    public int getY() {
        return y;
    }

    public void setX(int x) {
        this.x = x;
    }

    public void setY(int y) {
        this.y = y;
    }

    public void moveDown() {
        ++y;
    }

    public void moveUp() {
        --y;
    }

    public void moveLeft() {
        --x;
    }

    public void moveRight() {
        ++x;
    }

    public void nextRotation() {
        shape = rotateShapes[(++indexRotation) % CNT_ROTATION_SHIP];
    }

    public void previousRotation() {
        shape = rotateShapes[(--indexRotation) % CNT_ROTATION_SHIP];
    }

    public int getLeftSide() {
        return x;
    }

    public int getRightSide() {
        return x + getWidth();
    }
}
