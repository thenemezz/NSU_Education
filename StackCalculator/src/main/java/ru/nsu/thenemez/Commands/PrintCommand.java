package ru.nsu.thenemez.Commands;

import ru.nsu.thenemez.Context.Context;

public class PrintCommand implements Command {
    @Override
    public void execute(Context context) {
        if (context.getStackSize() < 1) {
            throw new IllegalStateException("Not enough elements on the stack to print");
        }

        double topElement = context.peek();
        System.out.println("Top element of the stack: " + topElement);
    }
}
