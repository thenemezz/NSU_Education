package ru.nsu.org.thenemez.view.menu_game;

import ru.nsu.org.thenemez.model.Context;

import javax.swing.*;
import javax.swing.table.DefaultTableModel;
import java.awt.*;
import java.io.*;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.List;
import java.util.stream.Collectors;

public class LeaderBoard extends JFrame {
    private JTable table;
    private DefaultTableModel model;
    private List<String[]> players;
    private String playerStatistics = "PlayerStatistics.txt";

    public LeaderBoard() {
        super.setBounds(550, 200, 400, 300);
        setTitle("Таблица результатов");
        model = new DefaultTableModel();
        model.addColumn("Имя");
        model.addColumn("Очки");
        players = getPreviousPlayers();
        fillTablePlayer();
    }

    private List<String[]> getPreviousPlayers() {
        try (BufferedReader reader = new BufferedReader(
                new FileReader(Context.getPATH_RESOURCES() + playerStatistics))) {
            String line;
            players = new ArrayList<>();
            while ((line = reader.readLine()) != null) {
                String[] playerData = line.split(";")[0].split(",");
                players.add(playerData);
            }
        } catch (IOException ex) {
            System.out.println(ex.getMessage());
        }
        return players;
    }

    public void fillTablePlayer() {
        model.setRowCount(0); // Очистка таблицы перед заполнением

        players = players.stream()
                .sorted(Comparator.comparingInt((String[] player) -> Integer.parseInt(player[1])).reversed())
                .collect(Collectors.toList());

        for (String[] player : players) {
            model.addRow(player);
        }
        table = new JTable(model);
        JScrollPane scrollPane = new JScrollPane(table);//покрутка таблицы
        getContentPane().add(scrollPane, BorderLayout.CENTER);
        setVisible(true);
    }

    public void addPlayer(String name, int score) {
        String[] player = new String[]{name, Integer.toString(score)};

        players.add(player);
        fillTablePlayer();

        try (FileWriter fileWriter = new FileWriter(playerStatistics, true)) {
            fileWriter.write(String.join(",", player) + ";\n"); // Добавляем перевод строки после каждой записи
        } catch (IOException e) {
            System.err.println("Result table not found");
        }
    }

}