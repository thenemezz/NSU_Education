package ru.nsu.thenemez.Calculator;

import ru.nsu.thenemez.Fabric.CommandCreator;
import ru.nsu.thenemez.Commands.Command;
import ru.nsu.thenemez.MyExceptions.CalculatorExceptions;
import ru.nsu.thenemez.MyExceptions.FabricExceptions;
import ru.nsu.thenemez.Logging.MyLogger;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;

import java.util.HashMap;
import java.util.Map;
import java.util.Stack;
import java.util.logging.Logger;



public class Calculator {
    public static class Parameters {
        private final Stack<Double> stack;
        private final Map<String, Double> variablesMap;
        private final BufferedWriter outputStream;

        public Parameters(BufferedWriter outputStream) {
            this.stack = new Stack<>();
            this.variablesMap = new HashMap<>();
            this.outputStream = outputStream;
        }

        public Stack<Double> getStack() {
            return stack;
        }

        public Map<String, Double> getVariablesMap() {
            return variablesMap;
        }

        public BufferedWriter getOutputStream() {
            return outputStream;
        }
    }

    private final BufferedReader inputStream;
    private final BufferedWriter errorStream;
    private final Parameters parameters;
    private final Logger LOGGER;

    public Calculator(BufferedReader inputStream, BufferedWriter outputStream, BufferedWriter errorStream) {
        this.inputStream = inputStream;
        this.errorStream = errorStream;
        parameters = new Parameters(outputStream);
        LOGGER = MyLogger.getLogger();
        LOGGER.info("Calculator constructor success done.\n");
    }

    public void doCalculating() throws FabricExceptions, CalculatorExceptions {
        try {

            CommandCreator commandCreator = new CommandCreator();

            String commandLine;

            while ((commandLine = inputStream.readLine()) != null) {

                if (commandLine.contains("#")) {
                    int hashCharInd = commandLine.indexOf('#');
                    LOGGER.info("Find comments \"" + commandLine.substring(hashCharInd) + "\".\n");
                    commandLine = commandLine.substring(0, hashCharInd);
                    if (commandLine.equals("")) continue;
                }

                String[] commandParts = commandLine.split(" ");
                if (commandParts.length == 0 || commandLine.equals("")) {
                    LOGGER.severe("Unknown command: '" + commandLine + "'.\n");
                    errorStream.write("Unknown command: '" + commandLine + "'.\n");
                    continue;
                }

                Command command = commandCreator.createCommands(commandParts[0], errorStream);
                if (command != null) {
                    command.action(commandParts, parameters, errorStream);
                }

            }
        } catch (IOException ioExc) {
            throw new CalculatorExceptions(ioExc.getMessage());
        }
    }

}
