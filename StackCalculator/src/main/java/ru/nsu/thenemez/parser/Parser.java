//package ru.nsu.thenemez.parser;
//import ru.nsu.thenemez.Commands.Command;
//import java.io.BufferedReader;
//import java.io.FileReader;
//import java.io.IOException;
//
//
//
//public class Parser {
//    private final CommandFactory commandFactory;
//
//    public Parser(CommandFactory commandFactory) {
//        this.commandFactory = commandFactory;
//    }
//
//    public void parseFile(String filename) {
//        try (BufferedReader reader = new BufferedReader(new FileReader(filename))) {
//            String line;
//            while ((line = reader.readLine()) != null) {
//                if (!line.startsWith("#")) {
//                    Command command = parseCommand(line);
//                    if (command != null) {
//                        command.execute();
//                    }
//                }
//            }
//        } catch (IOException e) {
//            e.printStackTrace();
//        }
//    }
//
//    private Command parseCommand(String line) {
//        String[] parts = line.split("\\s+");
//        if (parts.length == 0) {
//            return null;
//        }
//
//        String commandName = parts[0].toUpperCase();
//        String[] args = new String[parts.length - 1];
//        System.arraycopy(parts, 1, args, 0, args.length);
//
//        return commandFactory.createCommand(commandName, args);
//    }
//}
