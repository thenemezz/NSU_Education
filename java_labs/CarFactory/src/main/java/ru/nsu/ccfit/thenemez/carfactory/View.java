package ru.nsu.ccfit.thenemez.carfactory;

import javafx.fxml.FXMLLoader;
import javafx.scene.control.Slider;
import javafx.scene.layout.AnchorPane;
import javafx.scene.text.Text;

import ru.nsu.ccfit.thenemez.carfactory.dealers.Dealer;
import ru.nsu.ccfit.thenemez.carfactory.producers.AccessoryProducer;
import ru.nsu.ccfit.thenemez.carfactory.producers.BodyProducer;
import ru.nsu.ccfit.thenemez.carfactory.producers.EngineProducer;

import java.io.IOException;
import java.util.HashMap;
import java.util.Objects;
import java.util.concurrent.atomic.AtomicInteger;

public class View implements FillListener {
    private final AnchorPane root;
    private final HashMap<String, Text> fills;

    public View() throws IOException {
        root = FXMLLoader.load(Objects.requireNonNull(View.class.getResource("menu.fxml")));
        fills = new HashMap<>();
        fills.put("EngineStorage", new Text(250, 330, "0"));
        fills.put("BodyStorage", new Text(250, 370, "0"));
        fills.put("AccessoryStorage", new Text(250, 410, "0"));
        fills.put("AutoStorage", new Text(250, 450, "0"));
        fills.put("Factory", new Text(250, 490, "0"));
        for (var pair : fills.entrySet()) {
            root.getChildren().add(pair.getValue());
        }

    }

    public void drawMenu(EngineProducer engineProducer, BodyProducer bodyProducer, AccessoryProducer accessoryProducer, Dealer dealer,AutoStorageController autoStorageController) {

        Slider engineSlider = (Slider) root.lookup("#EngineSlider");
        engineSlider.setValue(FactoryProperties.ENGINE_PRODUCER_SLEEP_TIME);
        engineSlider.valueProperty().addListener((changed, oldValue, newValue) -> engineProducer.setSleepTime((Double) newValue));

        Slider bodySlider = (Slider) root.lookup("#BodySlider");
        bodySlider.valueProperty().addListener((changed, oldValue, newValue) -> bodyProducer.setSleepTime((Double) newValue));
        bodySlider.setValue(FactoryProperties.BODY_PRODUCER_SLEEP_TIME);

        Slider accSlider = (Slider) root.lookup("#AccessoiresSlider");
        accSlider.valueProperty().addListener((changed, oldValue, newValue) -> accessoryProducer.setSleepTime((Double) newValue));
        accSlider.setValue(FactoryProperties.ACCESSORY_PRODUCER_SLEEP_TIME);

        Slider dealerSlider = (Slider) root.lookup("#DealerSlider");
        dealerSlider.valueProperty().addListener((changed, oldValue, newValue) -> dealer.setSleepTime((Double) newValue));
        dealerSlider.setValue(FactoryProperties.DEALER_SLEEP_TIME);

        Slider autoCreateSlider = (Slider) root.lookup("#AutoCreateSlider");
        autoCreateSlider.valueProperty().addListener((changed, oldValue, newValue) -> autoStorageController.setSleepTime((Double) newValue));
        autoCreateSlider.setValue(FactoryProperties.AUTO_STORAGE_CONTROLLER_SLEEP_TIME);

    }

    @Override
    public void updateFill(AtomicInteger fill, String storageName) {
        fills.get(storageName).setText(String.valueOf(fill.intValue()));
    }

    public AnchorPane getRoot() {
        return root;
    }
}



