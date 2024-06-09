package thenemez.nsu.client;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.SocketChannel;
import java.util.Iterator;
import java.util.Set;

import thenemez.nsu.messages.ClientMessage;
import thenemez.nsu.messages.ServerMessage;

import javafx.application.Platform;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.event.EventHandler;
import javafx.geometry.Pos;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.ScrollPane;
import javafx.scene.control.TextArea;
import javafx.scene.control.TextField;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyEvent;
import javafx.scene.layout.AnchorPane;
import javafx.scene.layout.VBox;
import javafx.stage.Stage;

public class ClientWindow {
    private InetSocketAddress serverAddress;
    private SocketChannel serverSocket;
    private String session = null;
    private String userName = "Alex";

    private Stage window;
    private Parent root;
    private TextArea textArea;
    private VBox usersListView;
    private VBox messagesView;

    public ClientWindow(Stage stage, String serverAddress, int port) {
        this.serverAddress = new InetSocketAddress(serverAddress, port);
        try {
            serverSocket = SocketChannel.open();
            serverSocket.configureBlocking(true);
        } catch (IOException e) {
            e.printStackTrace();
        }

        window = stage;
        window.setResizable(true);
        window.setMinHeight(700);
        window.setMinWidth(700);
        window.setHeight(700);
        window.setWidth(700);
    

        Button sendButton = new Button("Send");
        sendButton.setMinHeight(70);
        sendButton.setMinWidth(70);
        sendButton.setMaxHeight(70);
        sendButton.setMaxWidth(70);
        sendButton.setStyle("-fx-background-radius: 15px;");
        AnchorPane.setBottomAnchor(sendButton, 0.0);
        AnchorPane.setRightAnchor(sendButton, 0.0);
        sendButton.setOnAction(event -> {
            sendMessage();
        });

        usersListView = new VBox(10);
        ScrollPane scrollPane = new ScrollPane(usersListView);
        scrollPane.setMinWidth(150);
        scrollPane.setStyle("-fx-background-color: #F2F2F2; -fx-background-radius: 15px;");
        AnchorPane.setLeftAnchor(scrollPane, 0.0);
        AnchorPane.setTopAnchor(scrollPane, 0.0);
        AnchorPane.setBottomAnchor(scrollPane, 0.0);

        textArea = new TextArea();
        textArea.setMaxHeight(70);
        textArea.setMinWidth(500);
        textArea.setStyle("-fx-background-color: #F2F2F2; -fx-background-radius: 15px;");
        AnchorPane.setBottomAnchor(textArea, 0.0);
        AnchorPane.setLeftAnchor(textArea, 155.0);
        AnchorPane.setRightAnchor(textArea, 75.0);
        textArea.textProperty().addListener(new ChangeListener<String>() {
            @Override
            public void changed(ObservableValue<? extends String> observableValue, String oldValue, String newValue) {
                if (textArea.getText().length() > 1024) {
                    String str = textArea.getText().substring(0, 1024);
                    textArea.setText(str);
                }
            }
        });

        messagesView = new VBox(10);
        ScrollPane messagesScrollPane = new ScrollPane(messagesView);
        AnchorPane.setLeftAnchor(messagesScrollPane, 155.0);
        AnchorPane.setTopAnchor(messagesScrollPane, 0.0);
        AnchorPane.setBottomAnchor(messagesScrollPane, 75.0);
        AnchorPane.setRightAnchor(messagesScrollPane, 0.0);

        root = new AnchorPane(sendButton, scrollPane, textArea, messagesScrollPane);
    }

    public void start() {
        try {

            serverSocket.connect(serverAddress);

            showConnectionMenu();

            runRecieverThread();

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void showConnectionMenu() {
        VBox menu = new VBox(20);
        TextField textField = new TextField();
        textField.setMaxWidth(200);
        textField.textProperty().addListener(new ChangeListener<String>() {
            @Override
            public void changed(ObservableValue<? extends String> observableValue, String oldValue, String newValue) {
                if (textField.getText().length() > 10) {
                    String str = textField.getText().substring(0, 10);
                    textField.setText(str);
                }
            }
        });

        Button loginButton = new Button("Login");
        loginButton.setMinWidth(200);
        loginButton.setOnAction(event -> {
            userName = textField.getText();
            sendLoginRequest(userName);
        });

        menu.getChildren().addAll(textField, loginButton);
        menu.setAlignment(Pos.CENTER);
        window.setScene(new Scene(menu));
        window.show();
    }

    private void runRecieverThread() {
        Thread thread = new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    serverSocket.configureBlocking(false);
                    Selector selector = Selector.open();
                    serverSocket.register(selector, SelectionKey.OP_READ);
                    while (true) {
                        if (selector.select() == 0) {
                            continue;
                        }

                        Set<SelectionKey> selectionKeys = selector.selectedKeys();
                        Iterator<SelectionKey> iterator = selectionKeys.iterator();

                        while (iterator.hasNext()) {
                            SelectionKey key = iterator.next();

                            if (key.isReadable()) {
                                ServerMessage serverMessage = recv();
                                Platform.runLater(new Runnable() {
                                    @Override
                                    public void run() {
                                        processServerMessage(serverMessage);
                                    }
                                });
                            }

                            iterator.remove();
                        }
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }

            }
        });

        thread.setDaemon(true);
        thread.start();
    }

    private void processServerMessage(ServerMessage serverMessage) {
        if (serverMessage == null) {
            return;
        }

        if (serverMessage.status == ServerMessage.ServerMessageStatus.ERROR) {
            showErrorMessage(serverMessage);
        } 

        else if (
            serverMessage.status == ServerMessage.ServerMessageStatus.NEW_USER ||
            serverMessage.status == ServerMessage.ServerMessageStatus.USER_LEFT
        ) {
            sendUsersListRequest();
        } 
        
        else if (serverMessage.status == ServerMessage.ServerMessageStatus.SERVER_MESSAGE) {
            drawNewMessage(serverMessage);
        } 
        
        else if (serverMessage.status == ServerMessage.ServerMessageStatus.SUCCESS_LOGOUT_OR_RECV_MESSAGE) {
            //
        } 
        
        else if (serverMessage.status == ServerMessage.ServerMessageStatus.SUCCESS_LOGIN) {
            session = serverMessage.getSession();
            Scene scene = new Scene(root);
            scene.addEventHandler(KeyEvent.KEY_PRESSED, new EventHandler<KeyEvent>() {
                @Override
                public void handle(KeyEvent keyEvent) {
                    if (keyEvent.getCode() == KeyCode.ESCAPE) {
                        sendLogoutRequest();
                        Platform.exit();
                    }
                }
            });
            window.setScene(scene);
            window.show();
        }
        
        else if (serverMessage.status == ServerMessage.ServerMessageStatus.SUCCESS_USERS_LIST_REQUEST) {
            redrawUsersList(serverMessage);
        }
    }

    private void showErrorMessage(ServerMessage serverMessage) {
        Stage errorWindow = new Stage();
        errorWindow.initOwner(window);
        errorWindow.setMinHeight(300);
        errorWindow.setMinWidth(300);
        errorWindow.setMaxHeight(300);
        errorWindow.setMaxWidth(300);

        Label errorLabel = new Label(serverMessage.getMessage());
        
        errorWindow.setScene(new Scene(errorLabel));
        errorWindow.show();
    }

    private void redrawUsersList(ServerMessage serverMessage) {
        usersListView.getChildren().clear();
        for (String user: serverMessage.getUsers()) {
            Label userLabel;

            if (user.equals(userName)) {
                userLabel = new Label(user + " (YOU)");
            } else {
                userLabel = new Label(user);
            }

            userLabel.setStyle
            (   
                "-fx-text-fill: white;" +
                "-fx-font-weight: bold;" +
                "-fx-background-color: #333333;" +
                "-fx-background-radius: 10px;" +
                "-fx-padding: 10px;"
            );

            usersListView.getChildren().add(userLabel);
        }
    }

    private void drawNewMessage(ServerMessage serverMessage) {
        int maxLineLength = 65;

        String message = serverMessage.getMessage();
        String[] tokens = message.split(" ");
        message = serverMessage.getName() + ":\n";

        String line = "";
        for (int i = 0; i < tokens.length; ++i) {
            if (tokens[i].length() < maxLineLength) {
                if (line.length() + tokens[i].length() > maxLineLength) {
                    message += line + "\n";
                    line = tokens[i] + " ";
                } else {
                    line += tokens[i] + " ";
                }
            } else {
                message += line + "\n";
                line = "";
                for (int j = 0; j < tokens[i].length();) {
                    int length;
                    if (j + maxLineLength < tokens[i].length()) {
                        length = maxLineLength;
                    } else {
                        length = tokens[i].length() - j;
                    }
                    message += tokens[i].substring(j, j + length - 1) + "\n";
                    j += length;
                }
            }
        }
        message += line;

        Label label = new Label(message);

        label.setStyle
        (
            "-fx-text-fill: white;" +
            "-fx-font-weight: bold;" +
            "-fx-background-color: #333333;" +
            "-fx-background-radius: 10px;" +
            "-fx-padding: 10px;"
        );

        label.setMaxWidth(500);
        
        messagesView.getChildren().add(label);
    }

    private void sendMessage() {
        ClientMessage message = new ClientMessage(ClientMessage.ClientMessageStatus.USER_MESSAGE);
        if (!textArea.getText().isEmpty()) {
            message.setMessage(textArea.getText());
            message.setName(userName);
            send(message);
            textArea.clear();
        }
    }

    private void sendUsersListRequest() {
        ClientMessage clientMessage = new ClientMessage(ClientMessage.ClientMessageStatus.USERS_LIST_REQUEST);
        clientMessage.setSession(session);
        send(clientMessage);
    }

    private void sendLoginRequest(String userName) {
        ClientMessage clientMessage = new ClientMessage(ClientMessage.ClientMessageStatus.LOGIN_REQUEST);
        clientMessage.setName(userName);
        send(clientMessage);
    }

    private void sendLogoutRequest() {
        ClientMessage clientMessage = new ClientMessage(ClientMessage.ClientMessageStatus.LOGOUT_REQUEST);
        clientMessage.setSession(session);
        send(clientMessage);
    }

    private ServerMessage recv() {
        ServerMessage message = null;
        try {

            ByteBuffer buffer = ByteBuffer.allocate(4);
            serverSocket.read(buffer);
            buffer.position(0);
            buffer = ByteBuffer.allocate(buffer.getInt());
            serverSocket.read(buffer);
            
            String text = new String(buffer.array()).trim();

            System.out.println(text);
            System.out.println();

            if (!text.isEmpty()) {
                message = ServerMessage.parseMessage(text);
            }

        } catch (IOException e) {

            e.printStackTrace();
        }
        return message;
    }

    private void send(ClientMessage message) {
        try {

            ByteBuffer buffer = ByteBuffer.wrap(message.toString().getBytes());
            serverSocket.write(buffer);

        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
