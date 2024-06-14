package ru.nsu.org.thenemez.view.menu_game;

import ru.nsu.org.thenemez.model.Context;
import ru.nsu.org.thenemez.view.*;
import ru.nsu.org.thenemez.view.tetris_area.TetrisAreaView;

import javax.swing.*;
import java.awt.*;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class GenerateMenu {
    private String MAIN_MENU = "MainMenu.jpg";
    private JButton
            buttonStart,
            buttonLevel,
            buttonScore,
            buttonRules;
    private CreateFrame frame;

    public GenerateMenu()  {
        frame = new CreateFrame();

        JPanel panel = new JPanel () {
            @Override
            protected void paintComponent(Graphics g) {
                super.paintComponent(g);
                Image image = new ImageIcon(Context.getPATH_RESOURCES() + MAIN_MENU).getImage();
                g.drawImage(image, 0, 0, getWidth(), getHeight(), this);
            }
        };

        panel.setOpaque(false);
        frame.setContentPane(panel);

        createButtonStart();
        createButtonLevel();
        createButtonScore();
        createButtonRules();
    }

    private void createButtonStart() {
        buttonStart = new HorizontalGradientButton("Start game!",
                1050,
                300,
                SetColor.GREEN_START.get(),
                SetColor.GREEN_END.get());
        buttonStart.addActionListener(e -> {
            frame.dispose();//https://habr.com/ru/articles/554608/
            ExecutorService newExecutor = Executors.newSingleThreadExecutor();
            newExecutor.execute( new TetrisAreaView());
            newExecutor.shutdown();
        });
        frame.getContentPane().add(buttonStart);
    }

    private void  createButtonLevel() {
        buttonLevel = new HorizontalGradientButton("Difficulty level",
                250,
                300,
                SetColor.GOLD_START.get(),
                SetColor.GOLD_END.get());
        buttonLevel.addActionListener(e -> new GameLevelPanel());
        frame.getContentPane().add(buttonLevel);
    }
    private void createButtonScore() {
        buttonScore = new HorizontalGradientButton("Leaderboard",
                1050,
                450,
                SetColor.GREEN_START.get(),
                SetColor.GREEN_END.get());
        buttonScore.addActionListener(e -> {
            LeaderBoard leaderBoard = new LeaderBoard();
            leaderBoard.fillTablePlayer();
        });
        frame.getContentPane().add(buttonScore);
    }
    private void createButtonRules() {
        buttonRules = new HorizontalGradientButton("Rules!",
                250,
                450,
                SetColor.GOLD_START.get(),
                SetColor.GOLD_END.get());
        buttonRules.addActionListener(e -> new RulesGame());
        frame.getContentPane().add(buttonRules);
    }
}
