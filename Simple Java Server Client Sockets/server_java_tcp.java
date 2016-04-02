
/**
 * Derek Wong, 5516117
 * 
 * Server TCP ver 1.0, 10-14-2014
 * 
 * portions of socket code adapted from Java: How To Program by Harvey Deitel/Paul Deitel, pg 1131-1150
 * 
 * Usage: java server_java_tcp <port>
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

import java.util.*;
import java.net.*;
import java.io.*;
public class server_java_tcp
{
    private ServerSocket serverTCP;
    private Socket connection;
    private ObjectOutputStream serverOutputTCP;
    private ObjectInputStream serverInputTCP;

    //constructor to set up and start server
    public server_java_tcp(int port) throws IOException
    {
        try
        {
            serverTCP = new ServerSocket(port);
            connection = serverTCP.accept();
            serverOutputTCP = new ObjectOutputStream(connection.getOutputStream());
            serverOutputTCP.flush();
            serverInputTCP = new ObjectInputStream(connection.getInputStream());
        }
        catch (IOException connectionError)
        {
            System.err.println("ERROR: Could not bind port. Terminating.");
            System.exit(1);
        }
    }

    //sends message to client
    private void serverTCPSend(byte[] byteArray, int start, int length) throws IOException
    {
        try
        {
            serverOutputTCP.writeInt(length);
            serverOutputTCP.flush();
            if (length > 0)
            {
                serverOutputTCP.write(byteArray,start,length);
                serverOutputTCP.flush();
            }
        }
        catch (IOException writeOutError)
        {
            System.err.println("ERROR: Failed to send message. Terminating.");
            System.exit(1);
        }
    }

    //recieve command from client
    private byte[] serverTCPReceive() throws Exception
    {
        try
        {
            int length = serverInputTCP.readInt();
            byte[] data = new byte[length];
            if (length > 0)
            {
                serverInputTCP.readFully(data);
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
        int serverContKey = 0;
        String inputFromClient = "";
        String outputToClient = "";
        LinkedHashMap<String,String> keyValueMap = new LinkedHashMap<String,String>();
        if (args.length != 1)
        {
            System.err.println("ERROR: Invalid number of args. Terminating.");
            System.exit(1);
        }    
        int portNumber = Integer.parseInt(args[0]);
        if (portNumber > 65535)
        {
            System.err.println("ERROR: Invalid port. Terminating");
            System.exit(1);
        }       
        //setup sockets and streams
        server_java_tcp server = new server_java_tcp(portNumber);
        //loop to accept and parse messages
        while (true)
        {
            inputFromClient = new String(server.serverTCPReceive());
            //process what command is being read in and act appropiately
            if (inputFromClient.substring(0,1).equals("?")) //case 1: ?key
            {
                String key = inputFromClient.substring(1);
                if (keyValueMap.containsKey(key))
                {
                    outputToClient = key + "=" + keyValueMap.get(key) + "\n";
                }
                else
                {
                    outputToClient = key + "=\n";
                }
            }
            else
            if (inputFromClient.contains("=")) //case 2: set key=value
            {
                int equalsIndex = inputFromClient.indexOf("=");
                String setKey = inputFromClient.substring(0,equalsIndex);
                String setValue = inputFromClient.substring(equalsIndex+1);
                keyValueMap.put(setKey,setValue);
                outputToClient = "OK\n";
            }
            else
            {
                String[] tokens = inputFromClient.split(" ");
                if (tokens[0].equals("list")) //case 3: list
                {
                    outputToClient = "";
                    Set listSet = keyValueMap.entrySet();
                    Iterator listIterator = listSet.iterator();
                    while (listIterator.hasNext())
                    {
                        Map.Entry pair = (Map.Entry)listIterator.next();
                        outputToClient = outputToClient + pair.getKey() + "=" + pair.getValue() + "\n";
                    }
                }
                else
                if (tokens.length==2 && tokens[0].equals("listc")) //case 4: listc num
                {
                    int size = keyValueMap.size();
                    int itemsReq = Integer.parseInt(tokens[1]);
                    outputToClient = "";
                    Set listSet = keyValueMap.entrySet();
                    Iterator listIterator = listSet.iterator();
                    if (itemsReq < size) //can return the whole list
                    {
                        for (int i=0;i<itemsReq;i++)
                        {
                            Map.Entry pair = (Map.Entry)listIterator.next();
                            outputToClient = outputToClient + pair.getKey() + "=" + pair.getValue() + "\n";
                            serverContKey++;
                        }
                        outputToClient = outputToClient + serverContKey + "\n";
                    }
                    else
                    if (itemsReq >= size) //hit end of the list
                    {                       
                        while (listIterator.hasNext())
                        {
                            Map.Entry pair = (Map.Entry)listIterator.next();
                            outputToClient = outputToClient + pair.getKey() + "=" + pair.getValue() + "\n";
                        }
                        outputToClient = outputToClient + "END\n";
                    }
                }
                else 
                if (tokens.length==3 && tokens[0].equals("listc")) //case 5: listc num contKey
                {
                    int size = keyValueMap.size();
                    int itemsReq = Integer.parseInt(tokens[1]);
                    int clientContKey = Integer.parseInt(tokens[2]);
                    outputToClient = "";
                    Set listSet = keyValueMap.entrySet();
                    Iterator listIterator = listSet.iterator();
                    if (clientContKey != serverContKey)
                    {
                        outputToClient = outputToClient + "BAD KEY";
                    }
                    else
                    if ((itemsReq+clientContKey) < size) //can return the whole list
                    {
                        for (int j=0;j<clientContKey;j++) //push iterator to where it left off
                        {                            
                            Map.Entry pair = (Map.Entry)listIterator.next();
                        }
                        for (int k=0;k<itemsReq;k++) //add the requested items to outputToClient
                        {
                            Map.Entry pair = (Map.Entry)listIterator.next();
                            outputToClient = outputToClient + pair.getKey() + "=" + pair.getValue() + "\n";
                            serverContKey++;
                        }
                        outputToClient = outputToClient + serverContKey + "\n";
                    }
                    else 
                    if ((itemsReq+clientContKey) >= size) //will hit the end of the list                 
                    {
                        for (int l=0;l<clientContKey;l++) //push iterator to where it left off
                        {
                            Map.Entry pair = (Map.Entry)listIterator.next();
                        }
                        while (listIterator.hasNext())
                        {
                            Map.Entry pair = (Map.Entry)listIterator.next();
                            outputToClient = outputToClient + pair.getKey() + "=" + pair.getValue() + "\n";
                        }
                        outputToClient = outputToClient + "END\n";
                    }                    
                }
            }            
            //write to client
            byte[] array = outputToClient.getBytes();
            server.serverTCPSend(array,0,array.length);
        }
    }
}

