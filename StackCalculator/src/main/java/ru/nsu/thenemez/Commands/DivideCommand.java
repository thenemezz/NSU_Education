package ru.nsu.thenemez.Commands;

import ru.nsu.thenemez.Context.Context;

public class DivideCommand implements Command {
    @Override
    public void execute(Context context) {
        if (context.getStackSize() < 2) {
            throw new IllegalStateException("Not enough elements on the stack to perform division");
        }

        double divisor = context.pop();
        if (divisor == 0) {
            throw new ArithmeticException("Division by zero");
        }

        double dividend = context.pop();
        double result = dividend / divisor;
        context.pushValue(result);
    }
}

