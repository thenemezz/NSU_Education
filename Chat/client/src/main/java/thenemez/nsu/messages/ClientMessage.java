package thenemez.nsu.messages;

import java.io.StringReader;

import javax.xml.parsers.DocumentBuilderFactory;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.xml.sax.InputSource;

public class ClientMessage {
    public final ClientMessageStatus status;
    private String name = "";
    private String message = "";
    private String sessionId = "";

    public enum ClientMessageStatus {
        LOGIN_REQUEST ("login", "name"),
        LOGOUT_REQUEST ("logout", "session"),
        USERS_LIST_REQUEST ("list", "session"),
        USER_MESSAGE ("message", "message,name"),
        UNKNOWN ("", "");

        private String command;
        private String subSections;

        private ClientMessageStatus(String command, String subSections) {
            this.command = command;
            this.subSections = subSections;
        }

        public String getCommand() {
            return command;
        }

        public String getSubSections() {
            return subSections;
        }

        public static ClientMessageStatus getStatus(String command) {
            for (ClientMessageStatus status : ClientMessageStatus.values()) {
                if (status.getCommand().equals(command)) {
                    return status;
                }
            }
            return UNKNOWN;
        }
    }

    public ClientMessage(ClientMessageStatus status) {
        this.status = status;
    }

    public void setName(String name) {
        this.name = name;
    }

    public void setMessage(String message) {
        this.message = message;
    }

    public void setSession(String sessionId) {
        this.sessionId = sessionId;
    }

    public String getName() {
        return name;
    }

    public String getMessage() {
        return message;
    }

    public String getSession() {
        return sessionId;
    }

    public static ClientMessage parseMessage(String message) {
        ClientMessage clientMessage = null;
        try {
            Document document = DocumentBuilderFactory.newInstance().newDocumentBuilder().parse(new InputSource(new StringReader(message)));
            Element root = document.getDocumentElement();
            clientMessage = new ClientMessage(ClientMessageStatus.getStatus(root.getAttribute("name")));
            if (clientMessage.status == ClientMessageStatus.UNKNOWN) {
                return null;
            }
            if (root.getElementsByTagName("message") != null) {
                clientMessage.setMessage(root.getElementsByTagName("message").item(0).getTextContent());
            } 
            if (root.getElementsByTagName("name") != null) {
                clientMessage.setName(root.getElementsByTagName("name").item(0).getTextContent());
            }
            if (root.getElementsByTagName("session") != null) {
                clientMessage.setSession(root.getElementsByTagName("session").item(0).getTextContent());
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return clientMessage;
    }

    @Override
    public String toString() {
        String result;
        String[] subSections = status.getSubSections().split(",");

        result = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" + 
                 "<command name=\"" + status.getCommand() + "\">\n";

        for (String section : subSections) {
            if (section.equals("name")) {
                result += "    <" + section + ">" + name + "</" + section + ">\n";
            } else if (section.equals("message")) {
                result += "    <" + section + ">" + message + "</" + section + ">\n";
            } else {
                result += "    <" + section + ">" + sessionId + "</" + section + ">\n";
            }
        }

        result += "</command>\n";
    
        return result;
    }
}
