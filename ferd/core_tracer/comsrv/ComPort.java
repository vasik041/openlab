
import java.io.*;
import java.util.*;
import gnu.io.CommPortIdentifier;
import gnu.io.PortInUseException;
import gnu.io.SerialPort;
import gnu.io.SerialPortEvent;
import gnu.io.SerialPortEventListener;
import gnu.io.UnsupportedCommOperationException;


public class ComPort implements SerialPortEventListener
{
CommPortIdentifier portId;
Enumeration portList;
SerialPort serialPort;
InputStream inputStream;
OutputStream outputStream;

boolean opened;
boolean ready;
boolean timeout;

byte[] readBuffer;
int numBytes = 0;
int readPos = 0;

	public String[] getPorts() {
		ArrayList<String> list = new ArrayList<>();
	   	portList = CommPortIdentifier.getPortIdentifiers();
       	while(portList.hasMoreElements()) {
       		portId = (CommPortIdentifier) portList.nextElement();
       		if(portId.getName().startsWith("COM")) { 
       			list.add(portId.getName());
       		}
   		}
	   	return list.toArray(new String[list.size()]);
	}


	public void open(String p,int baudrate)	{
	   	portList = CommPortIdentifier.getPortIdentifiers();
       	while(portList.hasMoreElements()) {
       		portId = (CommPortIdentifier) portList.nextElement();
       		if(portId.getName().equalsIgnoreCase(p))
       			break;
   		}

       	readBuffer = new byte[4096];
        try {
			serialPort = (SerialPort) portId.open(this.getClass().getName(),0);
	    }
		catch(PortInUseException e) {
		 	System.out.println("Port "+p+" in use...");
			return;
		}
		opened = true;
		
        try {
       	    inputStream = serialPort.getInputStream();
            outputStream = serialPort.getOutputStream();
		}
		catch(IOException e) {
		 	System.out.println("IOException at openPort"+e);
			close();
			return;
		}

//		try {
//		    serialPort.addEventListener(this);
//		}
//		catch(TooManyListenersException e) {
//		 	System.out.println("too many listeners");
//		}
//      ready = false;
//		serialPort.notifyOnDataAvailable(true);

		try {
			serialPort.setFlowControlMode(SerialPort.FLOWCONTROL_NONE);
		    serialPort.setDTR(false);
            serialPort.setSerialPortParams(
            		baudrate,
            		SerialPort.DATABITS_8,
            		SerialPort.STOPBITS_1,
            		SerialPort.PARITY_NONE);
	    }
		catch(UnsupportedCommOperationException e) {
			System.out.println("Error seting port params");
			close();
			return;
		}
	}

	
	public void setDTR(boolean b) {
	    serialPort.setDTR(b);
	}

	
	@Override
    public void serialEvent(SerialPortEvent event) {
      	switch(event.getEventType()) {
	      	case SerialPortEvent.DATA_AVAILABLE:
				ready = true;
				break;
			
	      	case SerialPortEvent.BI:
	        case SerialPortEvent.OE:
		    case SerialPortEvent.FE:
		    case SerialPortEvent.PE:
	        case SerialPortEvent.CD:
		    case SerialPortEvent.CTS:
		    case SerialPortEvent.DSR:
	        case SerialPortEvent.RI:
		    case SerialPortEvent.OUTPUT_BUFFER_EMPTY:
        		break;
      	}
	}

	
	public void clear()	{
		numBytes = 0;
		readPos = 0;
    }

	
	public byte get(int timeLimit) {
		int f = 0;
		if(readPos < numBytes) {
			timeout = false;
			return readBuffer[readPos++];		
		}

		for(int t = 0; t < timeLimit; t++) {
			try {
				f = inputStream.available();
			}
			catch(Exception e) {
				System.out.println("Exception at available " + e);
			}
	        if(f == 0) {
				delay(1);
				continue;
			}

			try {
	        	numBytes = inputStream.read(readBuffer);
            }
			catch(IOException e) {
			 	System.out.println("IOException at getChar "+e);
			}
			readPos = 1;
			timeout = false;
			return readBuffer[0];
       	}
		timeout = true;
		return 0;
	}

	
    public boolean IsTimeout()	{
       	return timeout;
	}

	
    public void put(int b)	{
		put((byte)b);
	}


	public void put(byte b)	{
		try {
			outputStream.write(b);
			delay(1);
        }
		catch(IOException e) {
		 	System.out.println("IOException at put"+e);
		}
	}

	public void put(String s) {
		for(int i=0; i < s.length(); i++) {
			put(s.charAt(i));
		}
	}

	
	public void close()	{
		System.out.println("close port "+ (opened?"true":"false"));
		if(opened) {
			try {
				inputStream.close();
				outputStream.close();
			} catch(IOException e) {
				System.out.println("IOException at close"+e);
			}
			serialPort.close();
			opened = false;
		}
	}

	synchronized public boolean IsOpen()	{
		return opened;
	}

	
	public static void delay(int ms) {
		try {
			Thread.sleep(ms);
		}
		catch(Exception e) {
		 	System.out.println("Exception at delay " + e);
		}
	}
	
}
	
