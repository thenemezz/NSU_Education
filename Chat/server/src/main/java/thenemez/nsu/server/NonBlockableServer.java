package thenemez.nsu.server;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.ServerSocketChannel;
import java.nio.channels.SocketChannel;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.Set;

import thenemez.nsu.messages.ClientMessage;
import thenemez.nsu.messages.ServerMessage;

public class NonBlockableServer {
    private ServerSocketChannel serverSocket;
    private InetSocketAddress serverAddress;

    private int maxConnections = 10;
    private int connectionsCount = 0;

    private SocketChannel channels[] = new SocketChannel[maxConnections];
    private String users[] = new String[maxConnections];
    
    private LinkedList<ServerMessage> lastEvents = new LinkedList<>();

    public NonBlockableServer(String address, int port) {
        try {
            serverAddress = new InetSocketAddress(address, port);
            serverSocket = ServerSocketChannel.open();
            serverSocket.socket().bind(serverAddress);
            serverSocket.configureBlocking(false);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public ServerMessage createServerAnswer(ClientMessage message) {
        ServerMessage answer = null;

        if (message.status == ClientMessage.ClientMessageStatus.LOGIN_REQUEST) {
            if (connectionsCount < maxConnections) {
                for (int i = 0; i < maxConnections; ++i) {
                    if (users[i] == null) {
                        users[i] = message.getName();
                        answer = new ServerMessage(ServerMessage.ServerMessageStatus.SUCCESS_LOGIN);
                        answer.setSession(String.valueOf(i));
                        
                        ServerMessage event = new ServerMessage(ServerMessage.ServerMessageStatus.NEW_USER);
                        event.setName(message.getName());
                        lastEvents.addLast(event);

                        ++connectionsCount;

                        break;
                    } else if (users[i].equals(message.getName())) {
                        answer = new ServerMessage(ServerMessage.ServerMessageStatus.ERROR);
                        answer.setMessage("User already exists");
                        break;
                    }
                }
            } else {
                answer = new ServerMessage(ServerMessage.ServerMessageStatus.ERROR);
                answer.setMessage("Server is full");
            }
        }

        else if (message.status == ClientMessage.ClientMessageStatus.LOGOUT_REQUEST) {
            if (connectionsCount > 0) {

                for (int i = 0; i < maxConnections; ++i) {

                    if (users[i].equals(message.getName())) {
                        answer = new ServerMessage(ServerMessage.ServerMessageStatus.SUCCESS_LOGOUT_OR_RECV_MESSAGE);
                        
                        users[i] = null;

                        ServerMessage event = new ServerMessage(ServerMessage.ServerMessageStatus.USER_LEFT);
                        event.setName(message.getName());
                        lastEvents.addLast(event);

                        --connectionsCount;

                        break;
                    }
                }

                if (answer == null) {
                    answer = new ServerMessage(ServerMessage.ServerMessageStatus.ERROR);
                    answer.setMessage("You are not logged in");
                }

            } else {
                answer = new ServerMessage(ServerMessage.ServerMessageStatus.ERROR);
                answer.setMessage("Server is empty");
            }
        }

        else if (message.status == ClientMessage.ClientMessageStatus.USERS_LIST_REQUEST) {
            answer = new ServerMessage(ServerMessage.ServerMessageStatus.SUCCESS_USERS_LIST_REQUEST);
            LinkedList<String> usersList = new LinkedList<>();
            for (int i = 0; i < maxConnections; ++i) {
                if (users[i] != null) {
                    usersList.add(users[i]);
                }
            }
            answer.setUsers(usersList);
        }

        else if (message.status == ClientMessage.ClientMessageStatus.USER_MESSAGE) {
            answer = new ServerMessage(ServerMessage.ServerMessageStatus.SUCCESS_LOGOUT_OR_RECV_MESSAGE);
            
            ServerMessage event = new ServerMessage(ServerMessage.ServerMessageStatus.SERVER_MESSAGE);

            System.out.println(message.getName());

            event.setName(message.getName());
            event.setMessage(message.getMessage());

            lastEvents.addLast(event);
        }

        return answer;
    }

    public void start() {
        try (Selector selector = Selector.open()) {
            runSender();

            serverSocket.register(selector, SelectionKey.OP_ACCEPT);

            while (true) {
                if (selector.select() == 0) {
                    continue;
                }

                Set<SelectionKey> selectedKeys = selector.selectedKeys();
                Iterator<SelectionKey> iterator = selectedKeys.iterator();

                while (iterator.hasNext()) {
                    SelectionKey key = iterator.next();
                    
                    if (key.isAcceptable()) {
                        SocketChannel client = serverSocket.accept();
                        client.configureBlocking(false);
                        client.register(selector, SelectionKey.OP_READ);
                    }
                    
                    if (key.isReadable()) {
                        SocketChannel client = (SocketChannel) key.channel();

                        ByteBuffer recvBuffer = ByteBuffer.allocate(8192);
                        try {
                            client.read(recvBuffer);
                        } catch (Exception e) {
                            for (int i = 0; i < maxConnections; ++i) {
                                if (client.equals(channels[i])) {
                                    channels[i] = null;

                                    ClientMessage defendMessage = new ClientMessage(ClientMessage.ClientMessageStatus.LOGOUT_REQUEST);
                                    defendMessage.setName(users[i]);
                                    createServerAnswer(defendMessage);

                                    client.close();
                                    break;
                                }
                            }
                            break;
                        }

                        ClientMessage clientMessage = ClientMessage.parseMessage(new String(recvBuffer.array()).trim());

                        ServerMessage answer = createServerAnswer(clientMessage);

                        if (answer.status == ServerMessage.ServerMessageStatus.SUCCESS_LOGIN) {
                            for (int i = 0; i < maxConnections; ++i) {
                                if (users[i].equals(clientMessage.getName())) {
                                    channels[i] = client;
                                    break;
                                }
                            }
                        }

                        client.write(wrapMessage(answer));

                        client.register(selector, SelectionKey.OP_WRITE);
                    }
                    
                    if (key.isWritable()) {
                        key.channel().register(selector, SelectionKey.OP_READ);
                    }

                    iterator.remove();
                }
                
            }

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void runSender() {
        Thread thread = new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    while (true) {
                        Thread.sleep(10);
                        if (lastEvents.size() > 0) {
                            for (SocketChannel channel : channels) {
                                try {
                                    if (channel == null) {
                                        continue;
                                    }
                                    channel.write(wrapMessage(lastEvents.getFirst()));
                                } catch (IOException e) {
                                    e.printStackTrace();
                                }
                            }
                            lastEvents.removeFirst();
                        }
                    }
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        });
        thread.setDaemon(true);
        thread.start();
    }

    private ByteBuffer wrapMessage(ServerMessage message) {
        ByteBuffer buffer;

        Integer length = message.toString().getBytes().length;

        buffer = ByteBuffer.allocate(length + 4);
        buffer.putInt(length.intValue());
        buffer.put(message.toString().getBytes());

        buffer.position(0);

        return buffer;
    }

    
}
