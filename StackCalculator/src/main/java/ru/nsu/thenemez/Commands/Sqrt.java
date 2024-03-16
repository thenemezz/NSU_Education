package ru.nsu.thenemez.Commands;

import ru.nsu.thenemez.Calculator.Calculator.Parameters;
import ru.nsu.thenemez.MyExceptions.CommandExceptions;

import java.io.BufferedWriter;
import java.io.IOException;

public class Sqrt extends Command {
    @Override
    public void action(String[] commandArgs, Parameters parameters, BufferedWriter errorStream) throws IOException {
        try {
            if (commandArgs.length != 1) {
                throw new CommandExceptions("Invalid arguments for Sqrt command.");
            } else if (parameters.getStack().empty()) {
                throw new CommandExceptions("Stack must have at least one elements to perform the Sqrt operation.");
            } else {
                double doubleVal = parameters.getStack().pop();
                if (doubleVal < 0) {
                    throw new CommandExceptions("Square root from negative number.");
                }
                double res = Math.sqrt(doubleVal);
                LOGGER.info("Square root from " + doubleVal + ".\n");
                parameters.getStack().push(res);
            }
        } catch (CommandExceptions commExc) {
            commExc.printException(errorStream);
        }
    }
}
