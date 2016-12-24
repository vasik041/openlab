
import java.io.*;
import java.net.*;



public class Server extends Thread {

	public static ComPort port = new ComPort();
	
	
	class Rx extends Thread {
		Session session;
		byte buf[] = new byte[16*1024];
		int pos = 0;
		boolean bClose = false;
		
		public Rx(Session s) {
			super();
			session = s;
		}

		public void run() {
    		System.out.println("rx starting");
    		while(port.IsOpen() && !bClose) {
    			byte b = port.get(10);
    			if(port.IsTimeout()) {
    				ComPort.delay(1);
    				continue;
    			}
    			if(b != '\n' && pos < buf.length-1) {
    				buf[pos] = b;
    				pos++;
    				continue;
    			}
    			StringBuffer sb = new StringBuffer();
    			StringBuffer sbA = new StringBuffer();
    			sb.append("d");
    			for(int i=0; i < pos; i++) {
    				sb.append(toHex(buf[i]));
    				sbA.append((char)buf[i]);
    			}
    			sb.append("\n");
    			System.out.println("from port: "+sbA.toString());
    			session.send(sb.toString());
    			pos = 0;
    		}
    		System.out.println("rx stopped");
    	}		
	}


	
	class Session extends Thread {
		Socket connectionSocket;
		OutputStream outToClient;
		InputStream inFromClient;
    	int baud = 9600;
    	Rx rx;
    	boolean bClose = false;
    	
		public Session(Socket s) {
			super();
			connectionSocket = s;
		}

		
    	public void run() {
    		String remote = connectionSocket.getRemoteSocketAddress().toString();
    		System.out.println("connected to "+remote);
    		
    		try {
    			inFromClient = connectionSocket.getInputStream();
    			outToClient = connectionSocket.getOutputStream();
    		} catch(IOException e) {
    			e.printStackTrace();
    			return;
    		}

    		while(connectionSocket.isConnected() && !bClose) {
				String  msg = null;
				try {
					StringBuffer sb = new StringBuffer(16*1024);
					for(;;) {
						int c = inFromClient.read();
						if(c == '\n' || c == -1) break;
						sb.append((char)c);
					}
					msg = sb.toString();
				} catch (IOException e) {
					//e.printStackTrace();
					break;
				}
				processMsg(msg);
			}
			port.close();
			System.out.println("disconnected from "+remote);
    	}		

    	
    	public void send(String s) {
    		try {
    			for(int i=0; i < s.length(); i++) {
    				outToClient.write(s.charAt(i));
    			}
    			System.out.println("sent to client: "+s+"\n");
    		} catch (IOException e) {
    			e.printStackTrace();
    		}
    	}
    	
    	
    	private void processMsg(String d) {
    		if(d == null) return;
    		if(d.length() == 0) return;
    		System.out.println("message "+d);
    		switch(d.charAt(0)) {
    			case 'b':
    				baud = Integer.parseInt(d.substring(1,d.length()));
    				System.out.println("baudrate "+baud);
    				send("ok\n");
    				break;
    				
    			case 'o':
    				String p = d.substring(1,d.length());
    				System.out.println("port "+p);
    				port.open(p,baud);
    				if(port.IsOpen()) {
    					send("ok\n");
    					rx = new Rx(this);
    					rx.start();
    				}  else {
    					send("error\n");
    				}
    				break;
    				
    			case 'c':
    				System.out.println("close");
    				bClose = true;
    				rx.bClose = true;
    				port.close();
    				send("ok\n");
    				break;
    				
    			case 'd':
    				String data = toStr(d.substring(1,d.length()));
    				System.out.println("data "+data);
    				port.put(data);
    				break;
    		}
    	}

	}

	
	private static String toHex(int n) {
		String s1 = Integer.toHexString(n);
		if(s1.length() == 1) return "0".concat(s1);
		return s1;
	}

	
	public static String toStr(String s) {
		StringBuffer sb = new StringBuffer();
		for(int i=0; i < s.length()/2; i++) {
			String b = s.substring(2*i, 2*i+2);
			sb.append((char)Integer.parseInt(b, 16));
		}
		return sb.toString();
	}
	
    ServerSocket welcomeSocket = null;
	
	public void terminate()  {
		try {
			if(welcomeSocket != null) welcomeSocket.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	
	public void run() {
        System.out.println("Server started");

		try {
			welcomeSocket = new ServerSocket(8080);
			while(true) {
				Socket connectionSocket = welcomeSocket.accept();
				(new Session(connectionSocket)).start();
			}
		} catch (IOException e) {
			//e.printStackTrace();
		}
				
		if(welcomeSocket != null) {
			try {
				welcomeSocket.close();
			} catch (IOException e) {
				//e.printStackTrace();
			}
		}
		System.out.println("Server stoped");
    }

}


