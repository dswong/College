
/**
 * CS 176B HW 2 audio streaming simulator 
 * 
 * Derek Wong, 5516117
 * UCSB, CS 176B, 2-12-14
 * 
 * Code adapted from docs.oracle.com/javase/tutorial/sound/converters.html
 * 
 * Description: Will open audio file into a stream, which will be read into a buffer, which acts as the 
 * packet size. With the given buffer of bytes, will then decide whether or not the packet will be lost
 * in the simulation with a calcualted loss chance. If the packet is lost, something alternate will be 
 * written to file. 
 * 
 * Variables to set:
 * pathName = Path of audio file to be read in. Will write file to working directory.
 * packetSize = Size of packet. Will be used to caculate 
 *              buffer size = packetSize*bytesPerFrame (default 1)
 * threshold = set the threshold to decide percentage of packets that will make it
		through. A threshold of 90 means 90% of packets will make it.
 * type = decide what to do with a missing packet:
 *      1 - play silence (all zeros)
 *      2 - replaying last sample value
 *      3 - replay the entire last packet
 * 
 */

import java.io.*;
import sun.audio.*;
import javax.sound.sampled.*;

public class simulator
{
    //------------------Set Test Values-------------------
    private String pathName = "simple.au"; //set path to input file 
    private int packetSize = 100; //Used to set size of buffer to read in as one "packet" before writing back out
    private double threshold = 90.0; //set threshold to decide whether a packet will be lost
    private int type = 2; //decide what to do for an unavailable packet
    //------------------Testbed End-----------------------
    
    private boolean notDoneReading = true; //keep track of whether or not we need to keep reading
    private AudioInputStream inputStream;
    private ByteArrayOutputStream outputStream = new ByteArrayOutputStream();
    private AudioFileFormat format;
    private File fileOut;
    private byte[] audioBuffer; //byte array buffer to hold audio
    private byte[] previous; //byte array buffer to hold last audio packet
    private byte lastByte = 0;
    private int bytesPerFrame; //bytes per frame, default 1
    private int totalFramesRead; //total frames read
    
    public static void main()
    {
        simulator test = new simulator();
    }
    
    public simulator()
    {
       openAudioIn(pathName);
       openAudioOut();
       audioBuffer = new byte[packetSize*bytesPerFrame];
       previous = new byte[packetSize*bytesPerFrame];
       for (int i=0;i<packetSize*bytesPerFrame;i++) //zero out buffers
       {
           audioBuffer[i] = 0;
           previous[i] = 0;
       }
       double loss;
       while (notDoneReading)
       {
           readToBuffer();
           loss = calculateLossProb();
           if (loss < threshold)
           {
               writeToStream();
           }
           else
           {
               writeAltToStream(type);
           }
           for (int i=0;i<packetSize*bytesPerFrame;i++) //store current buffer to previous for cycle
           {
               previous[i] = audioBuffer[i];
           }
           lastByte = previous[packetSize*bytesPerFrame-1]; //get the last byte for type 2
       }    
       writeToFile();    
       try
       {
           inputStream.close();
           outputStream.close();
       }
       catch (Exception e)
       {
            System.out.println("Stream close error.");
       }
    }
    
    public void openAudioIn(String path) //opens .au audio file 
    {
        try 
        {
            File fileIn = new File(path);
            inputStream = AudioSystem.getAudioInputStream(fileIn);
            format = AudioSystem.getAudioFileFormat(fileIn);
            bytesPerFrame = inputStream.getFormat().getFrameSize();
            if (bytesPerFrame == AudioSystem.NOT_SPECIFIED)
            {
                bytesPerFrame = 1;
            }
        } 
        catch (Exception e)
        {
            System.out.println("Audio Input Error. Cannot Get Stream.");
        }
    }
    
    public void openAudioOut()
    {
        try
        {
            fileOut = new File("output.au");
        }
        catch (Exception e)
        {
            System.out.println("Cannot make audio out file.");
        }      
    }
    
    public void readToBuffer()
    {
        int done;
        try
        {        
             done = inputStream.read(audioBuffer,0,packetSize*bytesPerFrame);
             if (done == -1)
             {
                  notDoneReading = false;
             }           
        }
        catch (Exception e)
        {
            System.out.println("Read to Buffer Error.");
        }
    }
    
    public double calculateLossProb()
    {
        //set value, max 99.9 possible
        double random = Math.random()*100;
        return random;
    }
    
    public void writeToStream()
    {
        try
        {
            outputStream.write(audioBuffer,0,packetSize*bytesPerFrame);
        }
        catch (Exception e)
        {
            System.out.println("Write to output stream error.");
        }
    }
    
    public void writeAltToStream(int t)
    {
        byte[] tempBuffer = new byte[packetSize*bytesPerFrame];
        switch (t)
        {
            case 1: //play silence, all zeros
                for (int i=0;i<tempBuffer.length;i++) //fill array with zeroes
                {
                    tempBuffer[i] = 0;
                }
                outputStream.write(tempBuffer,0,packetSize*bytesPerFrame);
                break;            
            case 2: //replay last sample value
                for (int i=0;i<tempBuffer.length;i++)
                {
                    tempBuffer[i] = lastByte;
                }
                outputStream.write(tempBuffer,0,packetSize*bytesPerFrame);
                break;                
            case 3: //replay entire last packet
                outputStream.write(previous,0,packetSize*bytesPerFrame); //rewrite what was already in buffer previous loop, if first packet, this is all zeroes
                break;
            default: break;
        }
    }
    
    public void writeToFile()
    {
        try
        {
            byte[] finalData = outputStream.toByteArray(); //change ouputStream to byte array
            ByteArrayInputStream byteInputStream = new ByteArrayInputStream(finalData); //change byte array to input stream
            AudioInputStream output = new AudioInputStream(byteInputStream,format.getFormat(),finalData.length/bytesPerFrame);
            AudioSystem.write(output,format.getType(),fileOut);
            byteInputStream.close();
            output.close();
        }
        catch (Exception e)
        {
            System.out.println("Write to file error.");
        }
    }
}
