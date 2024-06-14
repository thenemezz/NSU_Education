package ru.nsu.thenemez.Commands;

import ru.nsu.thenemez.Calculator.Calculator.Parameters;
import ru.nsu.thenemez.MyExceptions.CommandExceptions;

import java.io.BufferedWriter;
import java.io.IOException;

public class Push extends Command {
    @Override
    public void action(String[] commandArgs, Parameters parameters, BufferedWriter errorStream) throws IOException {
        try {
            if (commandArgs.length < 2) {
                throw new CommandExceptions("Invalid arguments for Push command.");
            }

            for (int i = 1; i < commandArgs.length; i++) {
                if (parameters.getVariablesMap().containsKey(commandArgs[i])) {
                    double pushValue = parameters.getVariablesMap().get(commandArgs[i]);
                    parameters.getStack().push(pushValue);
                    LOGGER.info("Push variable on stack: " + commandArgs[i] + "\n");
                } else {
                    try {
                        double num = Double.parseDouble(commandArgs[i]);
                        parameters.getStack().push(num);
                        LOGGER.info("Push number on stack: " + num + "\n");
                    } catch (NumberFormatException e) {     // Parse double will throw NumberFormatException
                        throw new CommandExceptions("Pushing variable hasn't been defined.");
                    }
                }
            }
        } catch (CommandExceptions commExc) {
            commExc.printException(errorStream);
        }
    }
}
