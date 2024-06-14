package ru.nsu.org.thenemez.view;

import javax.swing.*;

//    JFrame.HIDE_ON_CLOSE - окно будет скрыто, но приложение продолжит работать в фоновом режиме
//    JFrame.DISPOSE_ON_CLOSE - окно будет закрыто, но приложение продолжит работать.

public class CreateFrame extends JFrame {
    public CreateFrame() {
        setExtendedState(JFrame.MAXIMIZED_BOTH);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setVisible(true);
    }
}
