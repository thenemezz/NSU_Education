package ru.nsu.thenemez.CreateStreams;


import ru.nsu.thenemez.Logging.MyLogger;

import java.io.*;
import java.util.logging.Logger;

public class CreateStreams {
    private static final Logger LOGGER = MyLogger.getLogger();

    public static BufferedReader getInputStream(String[] args) throws FileNotFoundException {
        if (args.length > 0) {
            LOGGER.info("Commands will be read from file: " + args[0] + " \n");
            return new BufferedReader(new FileReader(args[0]));
        } else {
            LOGGER.info("Commands will be read from System.in\n");
            return new BufferedReader(new InputStreamReader(System.in));
        }
    }

    public static BufferedWriter getOutputStream(String[] args) throws IOException {
        if (args.length > 1) {
            LOGGER.info("Results will be writen to file: " + args[1] + " \n");
            return new BufferedWriter(new FileWriter(args[1]));
        } else {
            LOGGER.info("Results will be write to System.out\n");
            return new BufferedWriter(new OutputStreamWriter(System.out));
        }
    }

    public static BufferedWriter getErrorStream(String[] args) throws IOException {
        if (args.length > 2) {
            LOGGER.info("Errors will be writen to file: " + args[2] + " \n");
            return new BufferedWriter(new FileWriter(args[2]));
        } else {
            LOGGER.info("Errors will be writen to System.err\n");
            return new BufferedWriter(new OutputStreamWriter(System.err));
        }
    }
}

