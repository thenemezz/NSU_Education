package ru.nsu.thenemez.Commands;

import ru.nsu.thenemez.Context.Context;

public class SqrtCommand implements Command {
    @Override
    public void execute(Context context) {
        if (context.getStackSize() < 1) {
            throw new IllegalStateException("Not enough elements on the stack to perform square root operation");
        }

        double value = context.pop();
        if (value < 0) {
            throw new ArithmeticException("Square root of a negative number");
        }

        double result = Math.sqrt(value);
        context.pushValue(result);
    }
}
