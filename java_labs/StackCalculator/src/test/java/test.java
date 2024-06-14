import org.junit.jupiter.api.Test;
import ru.nsu.thenemez.Calculator.Calculator;
import ru.nsu.thenemez.MyExceptions.CalculatorExceptions;
import ru.nsu.thenemez.MyExceptions.FabricExceptions;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.StringReader;
import java.io.StringWriter;

import static org.junit.jupiter.api.Assertions.*;


//import static org.junit.Assert.assertEquals;
//import org.junit.Test;
import java.io.*;

public class test {
    private final String inputFileName = "/Users/thenemez/Desktop/proga/java_labs/StackCalculator/src/test/CommandsTest";
    private final String outputFileName = "/Users/thenemez/Desktop/proga/java_labs/StackCalculator/src/test/OutputTest";
    private final String errorFileName = "/Users/thenemez/Desktop/proga/java_labs/StackCalculator/src/test/ErrorTest";

    private void runCalculator(String text) {
        try (BufferedWriter file = new BufferedWriter(new FileWriter(inputFileName))) {
            file.write(text);
        } catch (IOException ioExc) {
            throw new RuntimeException(ioExc);
        }

        try (BufferedReader inputStream = new BufferedReader(new FileReader(inputFileName));
             BufferedWriter outputStream = new BufferedWriter(new FileWriter(outputFileName));
             BufferedWriter errorStream = new BufferedWriter(new FileWriter(errorFileName))) {

            Calculator calculator = new Calculator(inputStream, outputStream, errorStream);
            calculator.doCalculating();

        } catch (IOException | CalculatorExceptions | FabricExceptions exc) {
            throw new RuntimeException(exc);
        }
    }


    @Test
    public void plusCheck() {
        String commands = "DEFINE a 4\nDEFINE b 8\nPUSH a\nPUSH b\n+\nPRINT";
        try {
            runCalculator(commands);
            BufferedReader checkOutStream = new BufferedReader(new FileReader(outputFileName));
            assertEquals(checkOutStream.readLine(), "12.0");
            checkOutStream.close();
        } catch (RuntimeException | IOException exc) {
            System.err.println(exc.getMessage());
        }
    }

    @Test
    public void minusCheck() {
        String commands = "DEFINE a 4\nDEFINE b 8\nPUSH a\nPUSH b\n-\nPRINT";
        try {
            runCalculator(commands);
            BufferedReader checkOutStream = new BufferedReader(new FileReader(outputFileName));
            assertEquals(checkOutStream.readLine(), "4.0");
            checkOutStream.close();
        } catch (RuntimeException | IOException exc) {
            System.err.println(exc.getMessage());
        }
    }

    @Test
    public void multCheck() {
        String commands = "DEFINE a 4\nDEFINE b 8\nPUSH a\nPUSH b\n*\nPRINT";
        try {
            runCalculator(commands);
            BufferedReader checkOutStream = new BufferedReader(new FileReader(outputFileName));
            assertEquals(checkOutStream.readLine(), "32.0");
            checkOutStream.close();
        } catch (RuntimeException | IOException exc) {
            System.err.println(exc.getMessage());
        }
    }
}

//    @Test
//    public void sqrtCheck() {
//        String commands = "DEFINE a 4\nPUSH a\nSQRT\nPRINT\nPUSH -1\nSQRT";
//        try {
//            runCalculator(commands);
//            BufferedReader checkOutStream = new BufferedReader(new FileReader(outputFileName));
//            assertEquals(checkOutStream.readLine(), "2.0");
//            checkOutStream.close();
//
//            BufferedReader checkErrStream = new BufferedReader(new FileReader(errorFileName));
//            assertEquals(checkErrStream.readLine(), "Square root from negative number.");
//            checkErrStream.close();
//        } catch (RuntimeException | IOException exc) {
//            System.err.println(exc.getMessage());
//        }
//    }
//
//    @Test
//    public void divCheck() {
//        String commands = "DEFINE a 4\nDEFINE b 8\nPUSH a\nPUSH b\n/\nPRINT";
//        try {
//            runCalculator(commands);
//            BufferedReader checkOutStream = new BufferedReader(new FileReader(outputFileName));
//            assertEquals(checkOutStream.readLine(), "2.0");
//            checkOutStream.close();
//        } catch (RuntimeException | IOException exc) {
//            System.err.println(exc.getMessage());
//        }
//    }
//
//    @Test
//    public void divByZeroCheck() {
//        String commands = "DEFINE a 0\nDEFINE b 8\nPUSH a\nPUSH b\n/\nPRINT";
//        try {
//            runCalculator(commands);
//            BufferedReader checkErrStream = new BufferedReader(new FileReader(errorFileName));
//            assertEquals(checkErrStream.readLine(), "Division by zero.");
//            assertEquals(checkErrStream.readLine(), "Stack must have at least one elements to perform the Print operation.");
//            checkErrStream.close();
//        } catch (RuntimeException | IOException exc) {
//            System.err.println(exc.getMessage());
//        }
//    }
//
//    @Test
//    public void pushAndPrintAndPopCheck() {
//        String commands = "PUSH 4 5 6 #7 8\nPRINT\nPOP\nPRINT\nPOP\nPRINT\nPOP\nPRINT";
//        try {
//            runCalculator(commands);
//            BufferedReader checkOutStream = new BufferedReader(new FileReader(outputFileName));
//            assertEquals(checkOutStream.readLine(), "6.0");
//            assertEquals(checkOutStream.readLine(), "5.0");
//            assertEquals(checkOutStream.readLine(), "4.0");
//            checkOutStream.close();
//
//            BufferedReader checkErrStream = new BufferedReader(new FileReader(errorFileName));
//            assertEquals(checkErrStream.readLine(), "Stack must have at least one elements to perform the Print operation.");
//            checkErrStream.close();
//        } catch (RuntimeException | IOException exc) {
//            System.err.println(exc.getMessage());
//        }
//    }
//
//
//    @Test
//    public void defineCheck() {
//        String commands = "DEFINE a 4\nDEFINE b 8\nDEFINE c f\nDEFINE 2 5\nPUSH a\nPUSH b\nPRINT\nPOP\nPRINT";
//        try {
//            runCalculator(commands);
//            BufferedReader checkOutStream = new BufferedReader(new FileReader(outputFileName));
//            assertEquals(checkOutStream.readLine(), "8.0");
//            assertEquals(checkOutStream.readLine(), "4.0");
//            checkOutStream.close();
//
//            BufferedReader checkErrStream = new BufferedReader(new FileReader(errorFileName));
//            assertEquals(checkErrStream.readLine(), "Wrong type of second definitive value.");
//            assertEquals(checkErrStream.readLine(), "Wrong type of first definitive value.");
//            checkErrStream.close();
//
//        } catch (RuntimeException | IOException exc) {
//            System.err.println(exc.getMessage());
//        }
//    }
//
//    @Test
//    public void doubleDefineCheck() {
//        String commands = "DEFINE a 4\nDEFINE a 8\nPUSH a\nPRINT";
//        try {
//            runCalculator(commands);
//            BufferedReader checkOutStream = new BufferedReader(new FileReader(outputFileName));
//            assertEquals(checkOutStream.readLine(), "4.0");
//            checkOutStream.close();
//
//            BufferedReader checkErrStream = new BufferedReader(new FileReader(errorFileName));
//            assertEquals(checkErrStream.readLine(), "The variable \"a\" has been defined earlier.");
//            checkErrStream.close();
//
//        } catch (RuntimeException | IOException exc) {
//            System.err.println(exc.getMessage());
//        }
//    }
//
//    @Test
//    public void unknownCommandCheck() {
//        String commands = "PUSH 1 2 3\nUNKNOWN\nPRINT\nPOP\nPRINT\nPOP\nPRINT";
//        try {
//            runCalculator(commands);
//            BufferedReader checkOutStream = new BufferedReader(new FileReader(outputFileName));
//            assertEquals(checkOutStream.readLine(), "3.0");
//            assertEquals(checkOutStream.readLine(), "2.0");
//            assertEquals(checkOutStream.readLine(), "1.0");
//            checkOutStream.close();
//
//            BufferedReader checkErrStream = new BufferedReader(new FileReader(errorFileName));
//            assertEquals(checkErrStream.readLine(), "Unknown command:\"UNKNOWN\".");
//            checkErrStream.close();
//
//        } catch (RuntimeException | IOException exc) {
//            System.err.println(exc.getMessage());
//        }
//    }
//
//    @Test
//    public void emptyStackCheck() {
//        String commands = "POP\nPRINT\nPOP\nPRINT";
//        try {
//            runCalculator(commands);
//            BufferedReader checkErrStream = new BufferedReader(new FileReader(errorFileName));
//            assertEquals(checkErrStream.readLine(), "Stack must have at least one elements to perform the Pop operation.");
//            assertEquals(checkErrStream.readLine(), "Stack must have at least one elements to perform the Print operation.");
//            assertEquals(checkErrStream.readLine(), "Stack must have at least one elements to perform the Pop operation.");
//            assertEquals(checkErrStream.readLine(), "Stack must have at least one elements to perform the Print operation.");
//            checkErrStream.close();
//        } catch (RuntimeException | IOException exc) {
//            System.err.println(exc.getMessage());
//        }
//    }
//
//    @Test
//    public void commentsCheck() {
//        String commands = "PUSH 1#23\n#Hi im comment\n     #And me too# # ## \nPRINT";
//        try {
//            runCalculator(commands);
//            BufferedReader checkOutStream = new BufferedReader(new FileReader(outputFileName));
//            assertEquals(checkOutStream.readLine(), "1.0");
//            checkOutStream.close();
//        } catch (RuntimeException | IOException exc) {
//            System.err.println(exc.getMessage());
//        }
//    }
//
//}

