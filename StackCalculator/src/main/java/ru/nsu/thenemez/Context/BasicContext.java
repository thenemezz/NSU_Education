package ru.nsu.thenemez.Context;


import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class BasicContext implements Context {
    private final List<Double> stack;
    private final Map<String, Double> variables;
    private boolean exitFlag;

    public BasicContext() {
        this.stack = new ArrayList<>();
        this.variables = new HashMap<>();
        this.exitFlag = false;
    }

    @Override
    public List<Double> getNumbersOnStack() {
        return stack;
    }

    @Override
    public Map<String, Double> getVariables() {
        return variables;
    }

    @Override
    public void pushValue(double value) {
        stack.add(value);
    }

    @Override
    public void pushVariable(String name) {
        Double value = variables.get(name);
        if (value != null) {
            stack.add(value);
        } else {
            throw new IllegalArgumentException("Undefined variable: " + name);
        }
    }

    @Override
    public double pop() {
        if (stack.isEmpty()) {
            throw new IllegalStateException("Stack is empty");
        }
        return stack.remove(stack.size() - 1);
    }
    public double peek() {
        if (stack.isEmpty()) {
            throw new IllegalStateException("Stack is empty");
        }
        return stack.get(stack.size() - 1); // Возвращаем верхний элемент стека без удаления
    }

    @Override
    public boolean isVariableName(String string) {
        return variables.containsKey(string);
    }

    @Override
    public void defineVariable(String name, double value) {
        variables.put(name, value);
    }

    @Override
    public int getStackSize() {
        return stack.size();
    }

    @Override
    public void setExitFlag() {
        exitFlag = true;
    }

    @Override
    public boolean getExitFlag() {
        return exitFlag;
    }
}

