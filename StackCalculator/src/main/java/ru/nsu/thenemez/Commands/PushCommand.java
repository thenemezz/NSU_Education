package ru.nsu.thenemez.Commands;

import ru.nsu.thenemez.Context.Context;

public class PushCommand implements Command {
    private final double value;

    public PushCommand(double value) {
        this.value = value;
    }

    @Override
    public void execute(Context context) {
        context.pushValue(value);
    }
}
