package ru.nsu.thenemez.MyExceptions;

//import Logging.MyLogger;

import java.io.BufferedWriter;
import java.io.IOException;
import java.util.logging.Logger;

abstract public class MyExceptions extends Exception {
    protected final String expMessage;
    //protected final Logger LOGGER = MyLogger.getLogger();

    public MyExceptions(String expMessage) {
        this.expMessage = expMessage;
    }

    public String getMessage() {
        return expMessage;
    }

    abstract public void printException(BufferedWriter errorStream) throws IOException;
}

