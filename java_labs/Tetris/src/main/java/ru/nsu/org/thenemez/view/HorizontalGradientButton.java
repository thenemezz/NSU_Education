package ru.nsu.org.thenemez.view;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class HorizontalGradientButton extends JButton {
    private Color startColor, endColor;
    private int x1 = 0;
    private int x2 = getWidth();
    private int step = 3;
    private int positionX, positionY;

    private void setPositionXY(int positionX, int positionY) {
        this.positionX = positionX;
        this.positionY = positionY;
    }

    private void setColor(Color startColor, Color endColor) {
        this.startColor = startColor;
        this.endColor = endColor;
    }

    public HorizontalGradientButton(String text, int positionX,  int positionY,  Color startColor,  Color endColor) {
        super(text);
        setPositionXY(positionX, positionY);
        setColor(startColor, endColor);
        setPreferredSize(new Dimension(225, 75));
        setContentAreaFilled(false);
        setFocusPainted(false);
        setBorderPainted(false);
        setOpaque(false);
        setLocation(positionX, positionY);
        Timer timer = new Timer(20, new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                x1 += step;
                x2 += step;
                if (x2 > getWidth()) {
                    x1 = -getWidth();
                    x2 = 0;
                }
                repaint();
            }
        });
        timer.start();
    }

    @Override
    protected void paintComponent(Graphics g) {
        Graphics2D g2d = (Graphics2D) g.create();
        g2d.setPaint(new GradientPaint(x1, 0, startColor, x2, 0, endColor));
        g2d.fillRoundRect (0, 0, getWidth(), getHeight(), 20, 20);
        g2d.dispose();
        AlphaComposite alpha = AlphaComposite.getInstance(AlphaComposite.SRC_OVER, 0.5f);
        setLocation(positionX, positionY);

        if (getModel().isRollover()) {
            g2d = (Graphics2D) g.create();
            g2d.setComposite(alpha);
            g2d.setColor(Color.WHITE);
            g2d.fillRoundRect(0, 0, getWidth(), getHeight(), 40, 40);
        }

        if (getModel().isArmed()) {
            setForeground(Color.yellow);
            g2d.dispose();
        } else {
            setForeground(Color.black);
        }
        super.paintComponent(g);
    }
}
