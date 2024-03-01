package ru.nsu.thenemez.Commands;

import ru.nsu.thenemez.Context.Context;
public class PopCommand implements Command {
    @Override
    public void execute(Context context) {
        context.pop();
    }
}