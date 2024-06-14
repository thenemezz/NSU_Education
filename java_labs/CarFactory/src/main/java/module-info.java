module ru.nsu.ccfit.thenemez.carfactory {
    requires javafx.controls;
    requires javafx.fxml;
    requires java.logging;
    opens ru.nsu.ccfit.thenemez.carfactory to javafx.fxml;
    exports ru.nsu.ccfit.thenemez.carfactory;
    exports ru.nsu.ccfit.thenemez.carfactory.details;
    exports ru.nsu.ccfit.thenemez.carfactory.storages;
    exports ru.nsu.ccfit.thenemez.carfactory.producers;
    exports ru.nsu.ccfit.thenemez.carfactory.dealers;
    exports ru.nsu.ccfit.thenemez.carfactory.threadPool;
    opens ru.nsu.ccfit.thenemez.carfactory.details to javafx.fxml;
}