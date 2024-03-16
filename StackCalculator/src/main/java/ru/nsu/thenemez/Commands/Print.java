package ru.nsu.thenemez.Commands;

import java.io.BufferedWriter;
import java.io.IOException;

import ru.nsu.thenemez.Calculator.Calculator.Parameters;
import ru.nsu.thenemez.MyExceptions.CommandExceptions;

public class Print extends Command {
    @Override
    public void action(String[] commandArgs, Parameters parameters, BufferedWriter errorStream) throws IOException {
        try {
            if (commandArgs.length != 1) {
                throw new CommandExceptions("Invalid arguments for Print command.");
            } else if (parameters.getStack().empty()) {
                throw new CommandExceptions("Stack must have at least one elements to perform the Print operation.");
            } else {
                double printValue = parameters.getStack().peek();
                parameters.getOutputStream().write(Double.toString(printValue)+'\n');
                System.out.println(printValue);
                LOGGER.info( "Print top element of stack: " + printValue + "\n");
            }
        } catch (CommandExceptions commExc) {
            commExc.printException(errorStream);
        }
    }
}