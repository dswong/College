
/**
 * Derek Wong, 5516117
 * 
 * Client TCP ver 1.0, 10-12-2014
 * 
 * Portions of socket code adapted from Java: How To Program by Harvey Deitel/Paul Deitel, pg 1131-1150
 * 
 * Usage: java client_java_tcp <address/hostname> <port>
 * 
 * Both the server and client methods for TCP utilize the same method to send 
 * messages through Java Sockets and Object Input/Output streams. String messages 
 * are converted into byte arrays which are sent across the network. An integer 
 * is first sent through to inform the reciever of the size of the incoming message, 
 * and then the reciver reads the message into a byte array, and then converts it
 * back to a string to print or parse.
 *
 * Note that the server has no manual exit condition. It will exit after the client
 * does because it cannot recieve a message to parse, which triggers an exception 
 * that terminates the program serverside.
 * 
 */

import java.net.*;
import java.io.*;
import java.util.Scanner;
public class client_java_tcp
{
    private Socket clientTCP;
    private ObjectOutputStream clientOutputTCP;
    private ObjectInputStream clientInputTCP;

    //constructor to set up and start the connection
    public client_java_tcp(String serverAddress, int port) throws IOException
    {
        try
        {
            clientTCP = new Socket(serverAddress,port);
            clientOutputTCP = new ObjectOutputStream(clientTCP.getOutputStream());
            clientOutputTCP.flush();
            clientInputTCP = new ObjectInputStream(clientTCP.getInputStream());
            System.out.println("Connected.");
        }
        catch (IOException connectionError)
        {
            System.err.println("ERROR: Could not connect to server. Terminating.");
            System.exit(1);
        }
    }

    //method to close the client connection when done
    private void closeConnection()
    {
        try
        {
            clientOutputTCP.close();
            clientInputTCP.close();
            clientTCP.close();
        }
        catch (IOException exitError)
        {
            System.exit(1);
        }
    }

    //sends command from client to server
    private void clientTCPSend(byte[] byteArray, int start, int length) throws IOException
    {
        try
        {
            clientOutputTCP.writeInt(length);
            clientOutputTCP.flush();
            if (length > 0)
            {
                clientOutputTCP.write(byteArray,start,length);
                clientOutputTCP.flush();
            }
        }
        catch (IOException writeOutError)
        {
            System.err.println("ERROR: Failed to send message. Terminating.");
            System.exit(1);
        }
    }

    //receive message from server
    private byte[] clientTCPReceive() throws Exception
    {
        try
        {
            int length = clientInputTCP.readInt();
            byte[] data = new byte[length];
            if (length > 0)
            {
                clientInputTCP.readFully(data);
            }
            return data;
        }
        catch (IOException receiveError)
        {
            System.err.println("ERROR: Failed to receive message. Terminating.");
            System.exit(1);
        }
        return new byte[0];
    }

    public static void main(String[] args) throws Exception
    {
        String input = "";
        String receivedServerMessage = "";
        if (args.length != 2)
        {
            System.err.println("ERROR: Invalid number of args. Terminating.");
            System.exit(1);
        }
        String serverName = args[0];
        int portNumber = Integer.parseInt(args[1]);
        if (portNumber > 65535)
        {
            System.err.println("ERROR: Invalid port. Terminating");
            System.exit(1);
        }
        //setup connection
        client_java_tcp client = new client_java_tcp(serverName,portNumber);
        Scanner inFromUser = new Scanner(System.in);
        //continuous loop to maintain connection
        while (true)
        {
            //read input from cmd line and parse
            input = inFromUser.nextLine();
            if (input.equals("help"))
            {
                System.out.println("Avaliable commands to server: ");
                System.out.println("?key - responds with \"key=value\" or \"key=\" if not set");
                System.out.println("key=value - set key for value and return \"OK\"");
                System.out.println("list - return all key/value pairs each on their own line");
                System.out.println("listc num - returns the first num keys and values, along with a continuation key");
                System.out.println("listc num continuationkey - returns the first num keys and values after the last set of key/values, along with a new continuation key.");
            }
            else if (input.equals("exit"))
            {
                break;
            }
            else if ((input.contains("?")||input.contains("="))&&!(input.contains("\r"))&&!(input.contains("\n"))) //parse ?key, key=value
            {
                if (input.contains("?")&&(input.indexOf("?")==0)) //check key for a ?key command
                {
                    String testKey = input.substring(1);
                    if (testKey.contains("?")||testKey.contains("="))
                    {
                        System.err.println("ERROR: Invalid command.");
                        continue;
                    }
                }
                else 
                if (input.contains("=")) //check key for key=value command
                {
                    int equalsIndex = input.indexOf("="); //assume first equals sign is the assignment cmd
                    String testKey = input.substring(0,equalsIndex);
                    if (testKey.contains("?"))
                    {
                        System.err.println("ERROR: Invalid command.");
                        continue;
                    }
                }
                //send message to server
                byte[] array = input.getBytes();
                client.clientTCPSend(array,0,array.length);
                //recieve message from server
                receivedServerMessage = new String(client.clientTCPReceive());
                //print out server message
                System.out.printf(receivedServerMessage); 
            }
            else if (input.contains("list")) //parse list/listc commands
            {
                String[] tokens = input.split(" ");
                if (tokens.length == 2 && tokens[0].equals("listc"))
                {
                    try 
                    {
                        Integer.parseInt(tokens[1]);
                    }
                    catch (NumberFormatException nfe0)
                    {
                        System.err.println("ERROR: Invalid command.");
                        continue;
                    }
                }
                else
                if (tokens.length == 3 && tokens[0].equals("listc"))
                {
                    try 
                    {
                        Integer.parseInt(tokens[1]);
                        Integer.parseInt(tokens[2]);
                    }
                    catch (NumberFormatException nfe1)
                    {
                        System.err.println("ERROR: Invalid command.");
                        continue;
                    }
                }
                //send message to server
                byte[] array = input.getBytes();
                client.clientTCPSend(array,0,array.length);
                //recieve message from server
                receivedServerMessage = new String(client.clientTCPReceive());
                if (receivedServerMessage.equals("BAD KEY")) //handle if continuation key was bad
                {
                    System.err.println("ERROR: Invalid continuation key.");
                    continue;
                }
                System.out.printf(receivedServerMessage); //print out server message
            }
            else
            {
                System.err.println("ERROR: Invalid command.");
            }
        }
        //end connection
        client.closeConnection();
    }
}
