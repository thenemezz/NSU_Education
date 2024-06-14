package ru.nsu.thenemez.Commands;

import java.io.BufferedWriter;
import java.io.IOException;
//import ru.nsu.thenemez.Context.Context;
import ru.nsu.thenemez.Calculator.Calculator.Parameters;
import ru.nsu.thenemez.MyExceptions.CommandExceptions;
public class Pop extends Command {
    @Override
    public void action(String[] commandArgs, Parameters parameters, BufferedWriter errorStream) throws IOException {
        try {
            if (commandArgs.length != 1) {
                throw new CommandExceptions("Invalid arguments for Pop command.");
            } else if (parameters.getStack().empty()) {
                throw new CommandExceptions("Stack must have at least one elements to perform the Pop operation.");
            } else {
                parameters.getStack().pop();
                LOGGER.info("Pop top element.\n");
            }
        } catch (CommandExceptions commExc) {
            commExc.printException(errorStream);
        }
    }
}