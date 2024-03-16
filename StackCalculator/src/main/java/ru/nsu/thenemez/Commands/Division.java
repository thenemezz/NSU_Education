package ru.nsu.thenemez.Commands;

import ru.nsu.thenemez.Calculator.Calculator.Parameters;
import ru.nsu.thenemez.MyExceptions.CommandExceptions;

import java.io.BufferedWriter;
import java.io.IOException;

public class Division extends Command {

    @Override
    public void action(String[] commandArgs, Parameters parameters, BufferedWriter errorStream) throws IOException {
        try {
            if (commandArgs.length != 1) {
                throw new CommandExceptions("Invalid arguments for Division command.");
            } else if (parameters.getStack().size() < 2) {
                throw new CommandExceptions("Stack must have at least two elements to perform the Division operation.");
            } else {
                double doubleVal1 = parameters.getStack().pop();
                double doubleVal2 = parameters.getStack().pop();
                if (doubleVal2 == 0) {
                    throw new CommandExceptions("Division by zero.");
                }
                double res = doubleVal1 / doubleVal2;
                if(Double.isInfinite(res)){
                    LOGGER.info("Overflow while division: " + doubleVal1 + " / " + doubleVal2 + ".\n");
                }
                LOGGER.info("Division: " + doubleVal1 + " / " + doubleVal2 + "\n");
                parameters.getStack().push(res);
            }
        } catch (CommandExceptions commExc) {
            commExc.printException(errorStream);
        }
    }
}