package ru.nsu.org.thenemez.view.tetris_area;

import ru.nsu.org.thenemez.controller.GameController;
import ru.nsu.org.thenemez.model.GameArea;

public class TetrisAreaView implements Runnable {
    private final GameArea gameArea;
    private final GameController gameController;
    public TetrisAreaView() {
        gameArea = new GameArea();
        gameController = new GameController(gameArea);
        PerformanceGameArea performanceGameArea = new PerformanceGameArea(gameController);
        gameArea.registerObserver(performanceGameArea);
    }

    @Override
    public void run() {

        gameController.run();
    }
}
