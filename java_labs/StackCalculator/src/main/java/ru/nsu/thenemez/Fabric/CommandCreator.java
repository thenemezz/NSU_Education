package ru.nsu.thenemez.Fabric;


import java.io.*;
import java.util.*;
import java.lang.reflect.InvocationTargetException;
import ru.nsu.thenemez.Commands.Command;
import ru.nsu.thenemez.Commands.*;
import ru.nsu.thenemez.MyExceptions.FabricExceptions;
import ru.nsu.thenemez.MyExceptions.CreateCommandsExceptions;
import ru.nsu.thenemez.Logging.MyLogger;
import java.util.logging.Logger;

public class CommandCreator {
    private final Map<String, Class<? extends ru.nsu.thenemez.Commands.Command>> commandClassesMap;
    private static final String CONFIG_FILE_NAME = "/Users/thenemez/Desktop/proga/java_labs/StackCalculator/src/main/java/ru/nsu/thenemez/Fabric/ConfigFabric.txt";
    private final Logger LOGGER = MyLogger.getLogger();

    public CommandCreator() throws FabricExceptions {
        commandClassesMap = new HashMap<>();
        loadCommands();
    }

    private void loadCommands() throws FabricExceptions {
        try (InputStream inputStream = new FileInputStream(CONFIG_FILE_NAME);
             BufferedReader configFile = new BufferedReader(new InputStreamReader(inputStream)))  {
            String line;
            while ((line = configFile.readLine()) != null) {
                String[] fabricConfigParts = line.split(" ");
                if (fabricConfigParts.length != 2) {
                    throw new FabricExceptions("Invalid fabric config line: " + line + ".");
                }

                Class<? extends Command> curClass = loadCommandClass(fabricConfigParts[1]);
                commandClassesMap.put(fabricConfigParts[0], curClass);
            }
            LOGGER.info("Command creator has created commands.\n");

        } catch (IOException ioExc) {
            throw new FabricExceptions(ioExc.getMessage());
        } catch (NullPointerException nullPtrExc) {
            throw new FabricExceptions("Fabric configuration file not found");
        }
    }

    private Class<? extends Command> loadCommandClass(String className) throws FabricExceptions {
        try {
            Class<?> curClass = Class.forName(className);
            if (!Command.class.isAssignableFrom(curClass)) {
                throw new FabricExceptions(className + " does not implement Command abstract class.");
            }
            return curClass.asSubclass(Command.class);
        } catch (ClassNotFoundException e) {
            throw new FabricExceptions(className + " hasn't been found.");
        }
    }

    public ru.nsu.thenemez.Commands.Command createCommands(String commandName, BufferedWriter errorStream) throws IOException {
        try {
            Class<? extends Command> commandClass = commandClassesMap.get(commandName);
            if (commandClass == null) {
                throw new CreateCommandsExceptions("Unknown command:\"" + commandName + "\".");
            }

            try {
                return commandClass.getDeclaredConstructor().newInstance();
            } catch (InstantiationException | IllegalAccessException | NoSuchMethodException | InvocationTargetException e) {
                throw new CreateCommandsExceptions("Failed to create command: " + commandName + "' while creating command.");
            }

        } catch (CreateCommandsExceptions crCommExc) {
            crCommExc.printException(errorStream);
            return null;
        }
    }
}