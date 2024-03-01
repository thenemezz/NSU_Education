package ru.nsu.thenemez.Commands;

import ru.nsu.thenemez.Context.Context;

public class DefineCommand implements Command {
    private final String name;
    private final double value;

    public DefineCommand(String name, double value) {
        this.name = name;
        this.value = value;
    }

    @Override
    public void execute(Context context) {
        context.defineVariable(name, value);
    }
}
