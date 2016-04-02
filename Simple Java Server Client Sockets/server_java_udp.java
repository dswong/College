
/**
 * Derek Wong, 5516117
 * 
 * Server TCP ver 1.0, 10-14-2014
 * 
 * portions of socket code adapted from Java: How To Program by Harvey Deitel/Paul Deitel, pg 1131-1150
 * 
 * Usage: java server_java_udp <port>
 * 
 * The client and server communicate through packets using Java's DatagramSocket class. The max size for a packet
 * is capped at byte[1024]. Anything larger will have lost truncated data. Both the client and server mirror each 
 * other in methods to send packets. On the client side, cmdToServer handles one full message to the server, sending 
 * the command, while resultFromServer will receive the message from the server. Their server counterparts are 
 * resultToClient and cmdFromClient. One full communication cycle is therefore:
 * 
 * cmdToServer -> cmdFromClient -> resultToClient -> resultFromServer 
 * 
 * These methods are called in main after the commands are parsed. All other methods are helper.
 * 
 */

import java.util.*;
import java.net.*;
import java.io.*;
public class server_java_udp
{
    private DatagramSocket serverUDP;
    private InetAddress IPAddress;
    private String serverCounter = "0";
    private int serverPort;
    private int clientPort;

    //constructor to set up and start server
    public server_java_udp(int givenPort) throws Exception
    {
        try
        {
            serverPort = givenPort;
            serverUDP = new DatagramSocket(serverPort);
        }
        catch (IOException connectionError)
        {
            System.err.println("ERROR: Could not bind port. Terminating.");
            System.exit(0);
        }
    }

    //helper method to toggle the prefix counter
    private void toggleCounter()
    {
        if (serverCounter.equals("0"))
        {
            serverCounter = "1";           
        }
        else
        {
            serverCounter = "0";
        }
    }

    //sends command from server to client
    private void serverUDPSend(byte[] message) throws IOException
    {
        try
        {
            DatagramPacket sendPacket = new DatagramPacket(message,message.length,IPAddress,clientPort);
            serverUDP.send(sendPacket);
        }
        catch (IOException sendError)
        {
            System.err.println("Failed to send message. Terminating.");
            System.exit(1);
        }
    }

    //helper method to send ACK to client
    private void serverUDPACKSend() throws IOException
    {
        String outwardACK = serverCounter + "ACK";
        byte[] message = outwardACK.getBytes();
        try 
        {
            DatagramPacket sendPacket = new DatagramPacket(message,message.length,IPAddress,clientPort);
            serverUDP.send(sendPacket);
        }
        catch (IOException sendError)
        {
            System.err.println("Failed to send message. Terminating.");
            System.exit(1);
        }
    }

    //helper method to tell whether a good ACK has been recieved
    private boolean serverUDPACKReceive() throws IOException
    {
        try
        {
            byte[] data = new byte[1024];
            DatagramPacket receivePacket = new DatagramPacket(data,data.length);
            serverUDP.receive(receivePacket);
            String ack = new String(receivePacket.getData());
            if (ack.substring(0,1).equals(serverCounter))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        catch (IOException receiveError)
        {

        }
        return false;
    }

    //receive message from client
    private byte[] serverUDPReceive() throws IOException
    {
        byte[] data = new byte[1024];
        try
        {
            DatagramPacket receivePacket = new DatagramPacket(data,data.length);
            serverUDP.receive(receivePacket);
            IPAddress = receivePacket.getAddress();
            clientPort = receivePacket.getPort();
        }
        catch (IOException receiveError)
        {
            System.err.println("Failed to receive message. Terminating.");
            System.exit(1);
        }
        return data;
    }

    //method that handles one outgoing message to client
    private void resultToClient(String result) throws Exception
    {
        int resultLength = result.length(); //get length of cmd to send (string length)
        String resultLengthMessage = serverCounter + Integer.toString(resultLength); //prepend counter int to message
        serverUDP.setSoTimeout(500);

        int timesSent = 0;
        while (true)
        {
            try 
            {
                serverUDPSend(resultLengthMessage.getBytes());
                timesSent++;
                if (serverUDPACKReceive()) //if we get the right ACK, break and start sending packets
                {
                    break;
                }
            }
            catch (SocketTimeoutException timeOut)
            {
                //if we time out, keep going until the 3rd time
                if (timesSent == 3)
                {
                    System.err.println("Failed to send message. Terminating.");
                    System.exit(1);
                }
            }
        }
        toggleCounter(); //toggle the counter, so now we can start sending the actual cmd to the server
        //if the string length is greater than 500, we are going to split it into seperate 500 length string segments
        //so we can send them through without exceeding the size for a 1024 byte packet
        if (resultLength > 500) 
        {
            int numPackets = (resultLength/500) + 1; //how many packets we need to send based on 500 split, +1 for remainder
            for (int j=0;j<numPackets;j++)
            {
                int start = j*500;
                int end = start+500;
                String segment = "";
                if (j==numPackets-1) //if we are at the last packet, just send from the start of the segment to end of string
                {
                    segment = result.substring(start);
                }
                else
                {
                    segment = result.substring(start,end);
                }
                String packetMessage = serverCounter + segment; //prepend counter num

                int timesSentPacket = 0;
                while (true)
                {
                    try
                    {
                        serverUDPSend(packetMessage.getBytes()); //send command
                        timesSentPacket++;
                        if (serverUDPACKReceive())
                        {
                            break;
                        }
                    }
                    catch (SocketTimeoutException timeOut)
                    {
                        if (timesSentPacket == 3)
                        {
                            System.err.println("Failed to send message. Terminating.");
                            System.exit(1);
                        }
                    }
                }
                toggleCounter(); //we sent the packet, so switch counter and prep to send next packet
            }
        }
        else //message is smaller than 500 length, so we only need to send one packet
        {
            String resultSinglePacket = serverCounter + result; //prepend counter num

            int timesSentOnePacket = 0;
            while (true)
            {               
                try
                {
                    serverUDPSend(resultSinglePacket.getBytes()); //send command
                    timesSentOnePacket++;
                    if (serverUDPACKReceive())
                    {
                        break;
                    }
                }
                catch (SocketTimeoutException timeOut)
                {
                    if (timesSentOnePacket==3)
                    {
                        System.err.println("Failed to send message. Terminating.");
                        System.exit(1);
                    }
                }
            }
            toggleCounter(); //packet sent, so switch counter
        }
    }

    //method to handle one incoming cmd from client
    private String cmdFromClient() throws Exception
    {
        String receivedString = "";
        int totalLength = 0;
        int receivedLength = 0;
        String receivedLengthPacket = ""; 

        int ackSends0 = 0;
        while (true)
        {
            receivedLengthPacket = new String(serverUDPReceive());
            if (receivedLengthPacket.substring(0,1).equals(serverCounter)) //valid length message recieved so we ACK
            {
                totalLength = Integer.parseInt(receivedLengthPacket.substring(1,2));
                serverUDPACKSend();
                ackSends0++;
            } 
            else
            {
                break; //if the first bit is not equal, then server has already got out ACK and is sending packets, so we move on
            }
            if (ackSends0 == 3)
            {
                break; //server is not going to send any more messages, so neither should we
            }
        }

        toggleCounter();
        String receivedPacket = "";
        serverUDP.setSoTimeout(2000); //timeout at 2000 ms
        while (receivedLength < totalLength) //while we have less than the total string length
        {
            try
            {
                int ackSends1 = 0;
                while (true)
                {
                    receivedPacket = new String(serverUDPReceive());
                    if (receivedPacket.substring(0,1).equals(serverCounter)) //if the bit is the same, we can save the packet
                    {
                        serverUDPACKSend();
                        receivedString = receivedString + receivedPacket.substring(1); //grab the string
                        receivedLength = receivedString.length(); //update the length we've gotten so far
                        ackSends1++;
                    }
                    else
                    {
                        break;
                    }
                    if (ackSends1 == 3)
                    {
                        break;
                    }
                }
                toggleCounter();
            }
            catch (SocketTimeoutException timedOut) //time out after not receiving packet
            {
                System.err.println("Failed to receive message. Terminating.");
                System.exit(1);
            }
        } 
        return receivedString;
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
            System.exit(0);
        }    
        int portNumber = Integer.parseInt(args[0]);
        if (portNumber > 65535)
        {
            System.err.println("ERROR: Invalid port. Terminating");
            System.exit(0);
        }       
        //setup sockets and streams
        server_java_udp server = new server_java_udp(portNumber);
        //loop to accept and parse messages
        while (true)
        {
            inputFromClient = new String(server.cmdFromClient());
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
                    System.out.println(outputToClient);
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
            server.resultToClient(outputToClient);
        }
    }
}

