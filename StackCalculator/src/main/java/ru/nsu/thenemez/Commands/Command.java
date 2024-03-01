package ru.nsu.thenemez.Commands;

import ru.nsu.thenemez.Context.BasicContext;
import ru.nsu.thenemez.Context.Context;

public interface Command {
    void execute(Context context);
}