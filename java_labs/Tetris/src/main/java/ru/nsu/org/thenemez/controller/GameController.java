package ru.nsu.org.thenemez.controller;

import ru.nsu.org.thenemez.model.Context;
import ru.nsu.org.thenemez.model.GameArea;
import ru.nsu.org.thenemez.model.TetrisShape;
import ru.nsu.org.thenemez.view.tetris_area.PerformanceGameArea;


import java.awt.*;

public class GameController {
    private static boolean isPaused = false;
    private static int DELAY = Context.getMIDDLE();
    private GameArea gameArea;

    public GameController(GameArea gameArea) {
        this.gameArea = gameArea;
    }

    public void setPaused(boolean isPaused) {
        GameController.isPaused = isPaused;
    }

    public static boolean getPaused() { return isPaused; }

    public static void setDELAY(int curDELAY) {
        DELAY = curDELAY;
    }

    public void preparationNewGame() {
        gameArea.updateInitialValues();
    }

    public TetrisShape getTetrisShape() { return gameArea.getShape(); }
    public Color[][] getPlacedTetrisShape() { return gameArea.getPlacedShape(); }
    public Integer getPointPlayer() { return gameArea.getPointPlayer(); }

    public void actionButtonUp() { gameArea.moveShapeUp(); }

    public void actionButtonDown() { gameArea.moveShapeDown(); }

    public void actionButtonLeft() { gameArea.moveShapeLeft(); }

    public void actionButtonRight() { gameArea.moveShapeRight(); }

    public void actionButtonRotate() { gameArea.moveShapeRotate(); }

    public void run() {
        while(true) {
            while (gameArea.IsMoveShapeDown(isPaused)) {
                try {
                    Thread.sleep(DELAY);
                } catch (InterruptedException e) {
                    throw new RuntimeException(e);
                }
            }

            if (!isPaused && gameArea.isBlockOutOfBounds()) {
                System.out.println("Game Over");
                Thread.currentThread().interrupt();
                PerformanceGameArea.gameOver();
                isPaused = true;
            }
            if (!isPaused)
                gameArea.spawnShape();
        }
    }
}
