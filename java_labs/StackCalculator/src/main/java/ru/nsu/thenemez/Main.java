package ru.nsu.thenemez;

//import static org.junit.Assert.assertEquals;
//import org.junit.Test;

import ru.nsu.thenemez.CreateStreams.*;
import ru.nsu.thenemez.Logging.MyLogger;
import ru.nsu.thenemez.MyExceptions.*;
import ru.nsu.thenemez.Calculator.Calculator;
import java.io.*;
import java.io.IOException;
import java.util.logging.Logger;



public class Main {
    private static final Logger LOGGER = MyLogger.getLogger();

    public static void main(final String[] args) throws IOException {
        BufferedWriter outputStream = null;
        BufferedWriter errorStream = null;
        BufferedReader inputStream = null;

        try {
            outputStream = CreateStreams.getOutputStream(args);
            errorStream = CreateStreams.getErrorStream(args);
            inputStream = CreateStreams.getInputStream(args);

            Calculator calculator = new Calculator(inputStream, outputStream, errorStream);

            calculator.doCalculating();

            LOGGER.info("End of all calculating.");

        } catch (FileNotFoundException  | FabricExceptions | CalculatorExceptions e) {
            e.printStackTrace();

        }


//        catch (FabricExceptions | CalculatorExceptions exc) {
//            exc.printException(errorStream);
//            errorStream.write(Arrays.toString(exc.getStackTrace()) );
//            errorStream.flush();

//        } finally {
//            try {
//                if (inputStream != null) inputStream.close();
//                if (outputStream != null) outputStream.close();
//                if (errorStream != null) errorStream.close();
//            } catch (IOException e) {
//                System.err.println("Error closing stream: " + e.getMessage());
//            }

    }
}








//        if (args.length == 1) {
//            filePath = args[0];
//            //reader = new BufferedReader(new FileReader(filePath));
//        } else if (args.length < 1) {
//             filePath = null;
//        } else {
//            System.out.println("Too many args");
//            return;
//        }



//}