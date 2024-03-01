package ru.nsu.thenemez.UI;
import ru.nsu.thenemez.Commands.*;
import ru.nsu.thenemez.Context.Context;
import ru.nsu.thenemez.Context.BasicContext;
//import ru.nsu.thenemez.parser.Parser;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.io.IOException;

public class Main {
    public static void main(String[] args) {
        //Парсинг аргсов
        String filePath;
        BufferedReader reader = null;

        Context context = new BasicContext();

        context.pushValue(1);
        context.pushValue(2);
        context.pushValue(3);
        context.pushValue(4);
        context.pushValue(5);
        context.pushValue(6);
        context.pushValue(7);

        // Define
        Command defineCommand = new DefineCommand("a", 10);
        defineCommand.execute(context);

        // Pop
        Command popCommand = new PopCommand();
        popCommand.execute(context);

        // Addition
        Command additionCommand = new PlusCommand();
        additionCommand.execute(context);

        // Subtraction
        Command subtractionCommand = new MinusCommand();
        subtractionCommand.execute(context);

        // Multiplication
        Command multiplicationCommand = new MultiplyCommand();
        multiplicationCommand.execute(context);

        // Division
        Command divisionCommand = new DivideCommand();
        divisionCommand.execute(context);

        // Square root
        Command sqrtCommand = new SqrtCommand();
        sqrtCommand.execute(context);

        // Print
        Command printCommand = new PrintCommand();
        printCommand.execute(context);
    }
        //System.out.println("Top of the stack: " + context.getNumbersOnStack().get(context.getNumbersOnStack().size() - 1));


//        if (args.length == 1) {
//            filePath = args[0];
//            //reader = new BufferedReader(new FileReader(filePath));
//        } else if (args.length < 1) {
//             filePath = null;
//        } else {
//            System.out.println("Too many args");
//            return;
//        }


    }
//}

//Создание калькулятора
//CommandFactory factory = new CommandFactory();