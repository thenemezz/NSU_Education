package thenemez.nsu.messages;

import java.io.StringReader;
import java.util.LinkedList;

import javax.xml.parsers.DocumentBuilderFactory;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.xml.sax.InputSource;

public class ServerMessage {
    public final ServerMessageStatus status;
    private String message;
    private String session;
    private String name;
    private LinkedList<String> users;

    public enum ServerMessageStatus {
        ERROR,
        SUCCESS_LOGOUT_OR_RECV_MESSAGE,
        SUCCESS_LOGIN,
        SUCCESS_USERS_LIST_REQUEST,
        SERVER_MESSAGE,
        NEW_USER,
        USER_LEFT,
        UNKNOWN
    }

    public ServerMessage(ServerMessageStatus status) {
        this.status = status;
    }

    public ServerMessageStatus getStatus() {
        return status;
    }

    public String getMessage() {
        return message;
    }

    public void setMessage(String message) {
        this.message = message;
    }

    public String getSession() {
        return session;
    }

    public void setSession(String session) {
        this.session = session;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public LinkedList<String> getUsers() {
        return users;
    }

    public void setUsers(LinkedList<String> users) {
        this.users = users;
    }
    
    public static ServerMessage parseMessage(String message) {
        ServerMessage serverMessage = null;
        ServerMessageStatus status = ServerMessageStatus.UNKNOWN;
        try {
            Document document 
             = 
            DocumentBuilderFactory.
            newInstance().
            newDocumentBuilder().
            parse(new InputSource(new StringReader(message)));

            Element root = document.getDocumentElement();

            if (root.getNodeName().equals("error")) {

                status = ServerMessageStatus.ERROR;

            } 
            else if (root.getNodeName().equals("success")) {
    
                if (
                    root.getElementsByTagName("session") != null && 
                    root.getElementsByTagName("session").getLength() > 0
                ) {
                    status = ServerMessageStatus.SUCCESS_LOGIN;
                } 

                else if (
                    root.getElementsByTagName("listusers") != null && 
                    root.getElementsByTagName("listusers").getLength() > 0
                ) {
                    status = ServerMessageStatus.SUCCESS_USERS_LIST_REQUEST;
                } 

                else {
                    status = ServerMessageStatus.SUCCESS_LOGOUT_OR_RECV_MESSAGE;
                }

            } 
            else if (root.getNodeName().equals("event")) {

                if (root.getAttribute("name").equals("message")) {
                    status = ServerMessageStatus.SERVER_MESSAGE;
                } 

                else if (root.getAttribute("name").equals("userlogin")) {
                    status = ServerMessageStatus.NEW_USER;
                } 

                else if (root.getAttribute("name").equals("userlogout")) {
                    status = ServerMessageStatus.USER_LEFT;
                }

            }

            serverMessage = new ServerMessage(status);

            if (
                root.getElementsByTagName("session") != null && 
                root.getElementsByTagName("session").getLength() > 0
            ) {
                serverMessage.
                setSession(
                    root.
                    getElementsByTagName("session").
                    item(0).
                    getTextContent()
                );
            }

            if (
                root.getElementsByTagName("listusers") != null && 
                root.getElementsByTagName("listusers").getLength() > 0
            ) {
                LinkedList<String> users = new LinkedList<String>();

                for (int i = 0; i < root.getElementsByTagName("user").getLength(); i++) {
                    users.add(root.getElementsByTagName("user").item(i).getTextContent());
                }

                serverMessage.setUsers(users);
            }

            if (
                root.getElementsByTagName("message")!= null && 
                root.getElementsByTagName("message").getLength() > 0
            ) {
                serverMessage.
                setMessage(
                    root.
                    getElementsByTagName("message").
                    item(0).
                    getTextContent()
                );
            }

            if (
                root.getElementsByTagName("name")!= null && 
                root.getElementsByTagName("name").getLength() > 0
            ) {
                serverMessage.
                setName(
                    root.
                    getElementsByTagName("name").
                    item(0).
                    getTextContent()
                );
            }
            
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
        return serverMessage;
    }

    @Override
    public String toString() {
        String result = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

        if (status == ServerMessageStatus.ERROR) {

            result += "<error>\n";
            result += "    <message>" + message + "</message>\n";
            result += "</error>\n";

        } 
        
        else if (status == ServerMessageStatus.NEW_USER || status == ServerMessageStatus.USER_LEFT) {

            if (status == ServerMessageStatus.NEW_USER) {
                result += "<event name=\"userlogin\">\n"; 
            } 
            
            else {
                result += "<event name=\"userlogout\">\n";
            }

            result += "    <name>" + name + "</name>\n";
            result += "</event>\n";

        } 
        
        else if (status == ServerMessageStatus.SUCCESS_LOGOUT_OR_RECV_MESSAGE) {

            result += "<success>\n" + 
                      "</success>\n";

        } 
        
        else if (status == ServerMessageStatus.SUCCESS_LOGIN) {

            result += "<success>\n";
            result += "    <session>" + session + "</session>\n";
            result += "</success>\n";

        } 
        
        else if (status == ServerMessageStatus.SERVER_MESSAGE) {

            result += "<event name=\"message\">\n";
            result += "    <message>" + message + "</message>\n";
            result += "    <name>" + name + "</name>\n";
            result += "</event>\n";

        } 
        
        else {

            result += "<success>\n";
            result += "    <listusers>\n";

            for (String user : users) {
                result += "        <user>\n";
                result += "            <name>" + user + "</name>\n";
                result += "        </user>\n";
            }
            
            result += "    </listusers>\n";
            result += "</success>\n";

        }

        return result;
    }
}
