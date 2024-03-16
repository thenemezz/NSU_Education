package ru.nsu.thenemez.Commands;
import ru.nsu.thenemez.Calculator.Calculator.Parameters;
//import Calculator.Calculator.Parameters;
//import Logging.MyLogger;

import java.io.BufferedWriter;
import java.io.IOException;
import java.util.logging.Logger;

import ru.nsu.thenemez.Logging.MyLogger;
public abstract class Command {

    protected final Logger LOGGER = MyLogger.getLogger();
    abstract public void action(String[] command_args, Parameters parameters, BufferedWriter errorStream) throws IOException;
}
