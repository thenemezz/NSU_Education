package ru.nsu.org.thenemez.view.tetris_area;

import ru.nsu.org.thenemez.controller.GameController;
import ru.nsu.org.thenemez.model.Context;
import ru.nsu.org.thenemez.view.CreateFrame;
import ru.nsu.org.thenemez.view.HorizontalGradientButton;
import ru.nsu.org.thenemez.view.SetColor;
import ru.nsu.org.thenemez.view.menu_game.GenerateMenu;
import ru.nsu.org.thenemez.view.menu_game.LeaderBoard;

import javax.swing.*;
import java.awt.*;

public class PerformanceGameArea implements Observer {
    CreateFrame frame;
    private JButton buttonMenu, buttonPause, buttonPlay, buttonRestart;
    private final String PANEL_GAME_AREA = "gameattributes/PanelGameArea.jpg";
    private static GameController gameController;
    private static JPanel panel;

    public PerformanceGameArea(GameController gameController) {
        this.gameController = gameController;
        frame = new CreateFrame();

        panel = new JPanel () {
            @Override
            protected void paintComponent(Graphics g) {
                super.paintComponent(g);
                Image image = new ImageIcon(Context.getPATH_RESOURCES() + PANEL_GAME_AREA).getImage();
                g.drawImage(image, 0, 0, getWidth(), getHeight(), this);
                FieldPanel.drawGridSquare(g);

                Font font = new Font("Comic", Font.BOLD, 80);
                g.setFont(font);
                g.setColor(new Color(88, 114, 140));
                g.drawString("POINT", 1050, 80);

                g.drawString(gameController.getPointPlayer().toString(), 1200, 160);

                DrawDetails.drawDetails(g, gameController.getTetrisShape());
                FieldPanel.drawBackGround(g, gameController.getPlacedTetrisShape());
            }
        };

        panel.setOpaque(false);
        frame.setContentPane(panel);
        new DetailSwitchButtons(frame, gameController);
        createButtonMenu();
        createButtonPause();
        createButtonRestart();
        createButtonPlay();
    }

    public static void Repaint() {
        panel.repaint();
    }

    private void createButtonMenu() {
        buttonMenu = new HorizontalGradientButton("Menu!",
                1100,
                500,
                SetColor.GREEN_START.get(),
                SetColor.GREEN_END.get());
        buttonMenu.addKeyListener(null);
        buttonMenu.addActionListener(e -> {
            frame.dispose();
            new GenerateMenu();
        });

        frame.add(buttonMenu);
    }
    private void createButtonPlay() {
        buttonPlay = new HorizontalGradientButton("Play",
                100,
                600,
                SetColor.GREEN_START.get(),
                SetColor.GREEN_END.get());
        buttonPlay.setFocusable(false);
        buttonPlay.addActionListener(e -> {
            gameController.setPaused(false);
        });

        frame.add (buttonPlay);
    }

    private void createButtonPause() {
        buttonPause = new HorizontalGradientButton("Pause",
                100,
                500,
                SetColor.GREEN_START.get(),
                SetColor.GREEN_END.get());
        buttonPause.setFocusable(false);

        buttonPause.addActionListener(e -> {
            gameController.setPaused(true);
        });

        frame.add(buttonPause);
    }

    private void createButtonRestart() {
        buttonRestart = new HorizontalGradientButton("Restart",
                1100,
                600,
                SetColor.GREEN_START.get(),
                SetColor.GREEN_END.get());

        buttonRestart.setFocusable(false);

        buttonRestart.addActionListener(e -> {
            gameController.preparationNewGame();
            gameController.setPaused(false);
        });

        frame.add(buttonRestart);
    }

    private static void createLeaderBoard(String playerName) {
        LeaderBoard leaderBoard = new LeaderBoard();
        leaderBoard.addPlayer(playerName, gameController.getPointPlayer());
        leaderBoard.fillTablePlayer();
    }

    public static void gameOver() {
        String playerName = JOptionPane.showInputDialog("Game Over\n Please, input your name.");
        createLeaderBoard(playerName);
    }

    @Override
    public void notification() {
        panel.repaint();
    }
}
