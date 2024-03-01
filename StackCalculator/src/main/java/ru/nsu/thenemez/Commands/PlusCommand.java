package ru.nsu.thenemez.Commands;

import ru.nsu.thenemez.Context.Context;

public class PlusCommand implements Command {
    @Override
    public void execute(Context context) {
        if (context.getStackSize() < 2) {
            throw new RuntimeException("Not enough elements on the stack to perform addition");
        }

        double operand2 = context.pop();
        double operand1 = context.pop();
        double result = operand1 + operand2;
        context.pushValue(result);
    }
}
