package ru.nsu.thenemez.MyExceptions;

import java.io.BufferedWriter;
import java.io.IOException;

public class CommandExceptions extends MyExceptions {
    public CommandExceptions(String expMessage) {
        super(expMessage);
    }

    public void printException(BufferedWriter errorStream) throws IOException {
        errorStream.write(expMessage + '\n');
        errorStream.flush();
        //LOGGER.severe(expMessage + "\n");
    }
}
