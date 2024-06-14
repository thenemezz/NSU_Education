package ru.nsu.org.thenemez.view.tetris_area;

import ru.nsu.org.thenemez.controller.GameController;

import javax.swing.*;
import java.awt.event.ActionEvent;

public class DetailSwitchButtons {
    public DetailSwitchButtons(JFrame frame, GameController gameController) {
        InputMap inputMap = frame.getRootPane().getInputMap();
        ActionMap actionMap = frame.getRootPane().getActionMap();
        inputMap.put(KeyStroke.getKeyStroke("UP"), "up");
        inputMap.put(KeyStroke.getKeyStroke("DOWN"), "down");
        inputMap.put(KeyStroke.getKeyStroke("LEFT"), "left");
        inputMap.put(KeyStroke.getKeyStroke("RIGHT"), "right");
        inputMap.put(KeyStroke.getKeyStroke("SPACE"), "space");
        actionMap.put("up", new AbstractAction () {
            @Override
            public void actionPerformed(ActionEvent e) {
                if (GameController.getPaused()) return;
                gameController.actionButtonUp();
            }
        });
        actionMap.put("down",  new AbstractAction() {
            @Override
            public void actionPerformed(ActionEvent e) {
                if (GameController.getPaused()) return;
                gameController.actionButtonDown();
            }
        });
        actionMap.put("left",  new AbstractAction() {
            @Override
            public void actionPerformed(ActionEvent e) {
                if (GameController.getPaused()) return;
                gameController.actionButtonLeft();
            }
        });
        actionMap.put("right",  new AbstractAction () {
            @Override
            public void actionPerformed(ActionEvent e) {
                if (GameController.getPaused()) return;
                gameController.actionButtonRight();
            }
        });
        actionMap.put("space",  new AbstractAction () {
            @Override
            public void actionPerformed(ActionEvent e) {
                if (GameController.getPaused()) return;
                gameController.actionButtonRotate();
            }
        });
    }
}

