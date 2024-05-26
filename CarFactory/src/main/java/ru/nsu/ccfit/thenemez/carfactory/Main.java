package ru.nsu.ccfit.thenemez.carfactory;

import javafx.application.Application;
import javafx.scene.Scene;
import javafx.stage.Stage;
import ru.nsu.ccfit.thenemez.carfactory.dealers.Dealer;
import ru.nsu.ccfit.thenemez.carfactory.producers.AccessoryProducer;
import ru.nsu.ccfit.thenemez.carfactory.producers.BodyProducer;
import ru.nsu.ccfit.thenemez.carfactory.producers.EngineProducer;
import ru.nsu.ccfit.thenemez.carfactory.storages.AccessoryStorage;
import ru.nsu.ccfit.thenemez.carfactory.storages.AutoStorage;
import ru.nsu.ccfit.thenemez.carfactory.storages.BodyStorage;
import ru.nsu.ccfit.thenemez.carfactory.storages.EngineStorage;

import java.io.IOException;

public class Main extends Application {
    @Override
    public void start(Stage stage) throws IOException {
        try {
            FactoryProperties.readInfo();
            View view = new View();


            EngineStorage engineStorage = new EngineStorage(FactoryProperties.ENGINE_STORAGE_SIZE, view);
            BodyStorage bodyStorage = new BodyStorage(FactoryProperties.BODY_STORAGE_SIZE, view);
            AccessoryStorage accessoryStorage = new AccessoryStorage(FactoryProperties.ACCESSORY_STORAGE_SIZE, view);
            AutoStorage autoStorage = new AutoStorage(FactoryProperties.AUTO_STORAGE_SIZE, view);

            EngineProducer engineProducer = new EngineProducer(engineStorage);
            BodyProducer bodyProducer = new BodyProducer(bodyStorage);
            AccessoryProducer accessoryProducer = new AccessoryProducer(accessoryStorage);
        //
            Factory factory = new Factory(engineStorage, bodyStorage, accessoryStorage, autoStorage, view);
            //контроллер склада (если на складе достаточно -> хватит делать машины)
            AutoStorageController autoStorageController = new AutoStorageController(factory, autoStorage);
            Dealer dealer = new Dealer(autoStorage);

            view.drawMenu(engineProducer, bodyProducer, accessoryProducer, dealer,autoStorageController);

            dealer.start();
            engineProducer.start();
            bodyProducer.start();
            accessoryProducer.start();
            autoStorageController.start();


            Scene scene = new Scene(view.getRoot());
            stage.setScene(scene);
            stage.show();

            stage.setOnHidden(event -> {
                engineProducer.interrupt();
                bodyProducer.interrupt();
                accessoryProducer.interrupt();
                factory.getWorkers().killThreadPool();
                autoStorageController.interrupt();
                dealer.interrupt();
            });

        }
        catch (RuntimeException e) {
            System.err.println(e.getMessage());
        }
    }

    public static void main(String[] args) {
        launch();
    }

}