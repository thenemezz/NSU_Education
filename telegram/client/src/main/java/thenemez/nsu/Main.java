package thenemez.nsu;

import thenemez.nsu.client.ClientWindow;

import javafx.application.Application;
import javafx.stage.Stage;

public class Main extends Application {
    public static void main(String[] args) {
        launch(args);
    }

    @Override
    public void start(Stage stage) {
        ClientWindow window = new ClientWindow(stage, "127.0.0.1", 8080);
        window.start();
    }
}