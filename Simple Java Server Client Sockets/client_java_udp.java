
/**
 * Derek Wong, 5516117
 * 
 * Client UDP ver 1.0, 10-12-2014
 * 
 * Portions of socket code adapted from Java: How To Program by Harvey Deitel/Paul Deitel, pg 1131-1150
 * 
 * Usage: java client_java_udp <address/hostname> <port>
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

import java.net.*;
import java.io.*;
import java.util.Scanner;
public class client_java_udp
{
    private DatagramSocket clientUDP;
    private InetAddress IPAddress;
    private String clientCounter = "0";
    private int port;

    //constructor to set IP address and port required by packets
    public client_java_udp(String serverAddress, int givenPort) throws Exception
    {
        try
        {
            port = givenPort;
            IPAddress = InetAddress.getByName(serverAddress);
            clientUDP = new DatagramSocket();
        }
        catch (Exception connectionError)
        {
            System.err.println("Could not connect to server. Terminating.");
            System.exit(1);
        }
    }

    //helper method to toggle the prefix counter
    private void toggleCounter()
    {
        if (clientCounter.equals("0"))
        {
            clientCounter = "1";           
        }
        else
        {
            clientCounter = "0";
        }
    }

    //sends command from client to server
    private void clientUDPSend(byte[] message) throws IOException
    {
        try
        {
            DatagramPacket sendPacket = new DatagramPacket(message,message.length,IPAddress,port);
            clientUDP.send(sendPacket);
        }
        catch (IOException sendError)
        {
            System.err.println("Failed to send message. Terminating.");
            System.exit(1);
        }
    }

    //helper method to send ACK to server
    private void clientUDPACKSend() throws IOException
    {
        String outwardACK = clientCounter + "ACK";
        byte[] message = outwardACK.getBytes();
        try 
        {
            DatagramPacket sendPacket = new DatagramPacket(message,message.length,IPAddress,port);
            clientUDP.send(sendPacket);
        }
        catch (IOException sendError)
        {
            System.err.println("Failed to send message. Terminating.");
            System.exit(1);
        }
    }

    //helper method to tell whether a good ACK has been recieved
    private boolean clientUDPACKReceive() throws IOException
    {
        try
        {
            byte[] data = new byte[1024];
            DatagramPacket receivePacket = new DatagramPacket(data,data.length);
            clientUDP.receive(receivePacket);
            String ack = new String(receivePacket.getData());
            if (ack.substring(0,1).equals(clientCounter))
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
            //let calling method handle timeout
        }
        return false;
    }

    //receive message from server
    private byte[] clientUDPReceive() throws IOException
    {
        byte[] data = new byte[1024];
        try
        {
            DatagramPacket receivePacket = new DatagramPacket(data,data.length);
            clientUDP.receive(receivePacket);
        }
        catch (IOException receiveError)
        {
            System.err.println("Failed to receive message. Terminating.");
            System.exit(1);
        }
        return data;
    }

    //method that handles one outgoing message to server
    private void cmdToServer(String cmd) throws Exception
    {
        int cmdLength = cmd.length(); //get length of cmd to send (string length)
        String cmdLengthMessage = clientCounter + Integer.toString(cmdLength); //prepend counter int to message
        clientUDP.setSoTimeout(500); //timeout at 500 ms

        int timesSent = 0;
        while (true)
        {
            try 
            {
                clientUDPSend(cmdLengthMessage.getBytes());
                timesSent++;
                if (clientUDPACKReceive()) //if we get the right ACK, break and start sending packets
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
        if (cmdLength > 500) 
        {
            int numPackets = (cmdLength/500) + 1; //how many packets we need to send based on 500 split, +1 for remainder
            for (int j=0;j<numPackets;j++)
            {
                int start = j*500;
                int end = start+500;
                String segment = "";
                if (j==numPackets-1) //if we are at the last packet, just send from the start of the segment to end of string
                {
                    segment = cmd.substring(start);
                }
                else
                {
                    segment = cmd.substring(start,end);
                }
                String packetMessage = clientCounter + segment; //prepend counter num

                int timesSentPacket = 0;
                while (true)
                {
                    try
                    {
                        clientUDPSend(packetMessage.getBytes()); //send command
                        timesSentPacket++;
                        if(clientUDPACKReceive())
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
            String cmdSinglePacket = clientCounter + cmd; //prepend counter num

            int timesSentOnePacket = 0;
            while (true)
            {               
                try
                {
                    clientUDPSend(cmdSinglePacket.getBytes()); //send command
                    timesSentOnePacket++;
                    if (clientUDPACKReceive())
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

    //method to handle one incoming response from server
    private String resultFromServer() throws Exception
    {
        String receivedString = "";
        int totalLength = 0;
        int receivedLength = 0;
        String receivedLengthPacket = "";

        int ackSends0 = 0;
        while (true)
        {
            receivedLengthPacket = new String(clientUDPReceive());
            if (receivedLengthPacket.substring(0,1).equals(clientCounter)) //valid length message recieved so we ACK
            {
                totalLength = Integer.parseInt(receivedLengthPacket.substring(1,2));
                clientUDPACKSend();
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

        //start receiving packets
        toggleCounter();
        String receivedPacket = "";
        clientUDP.setSoTimeout(2000); //timeout at 2000 ms
        while (receivedLength < totalLength) //while we have less than the total string length
        {
            try
            {
                int ackSends1 = 0;
                while (true)
                {
                    receivedPacket = new String(clientUDPReceive());
                    if (receivedPacket.substring(0,1).equals(clientCounter)) //if the bit is the same, we can save the packet
                    {
                        clientUDPACKSend();
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
        String input = "";
        String receivedServerMessage = "";
        if (args.length != 2)
        {
            System.err.println("ERROR: Invalid number of args. Terminating.");
            System.exit(0);
        }
        String serverName = args[0];
        int portNumber = Integer.parseInt(args[1]);
        if (portNumber > 65535)
        {
            System.err.println("ERROR: Invalid port. Terminating");
            System.exit(0);
        }
        //setup connection
        client_java_udp client = new client_java_udp(serverName,portNumber);
        System.out.println("Connected.");
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
                System.exit(0);
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
                //send and recieve message from server
                client.cmdToServer(input);
                receivedServerMessage = client.resultFromServer();
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
                //send and recieve message from server
                client.cmdToServer(input);
                receivedServerMessage = client.resultFromServer();
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
    }
}
