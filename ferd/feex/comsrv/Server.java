
import java.io.*;
import java.net.*;



public class Server extends Thread {

	
	class Rx extends Thread {
		ComPort port;
		Session session;
		byte buf[] = new byte[4096];
		int pos = 0;
		
		
		public Rx(ComPort p,Session s) {
			super();
			port = p;
			session = s;
		}

		
    	public void run() {
    		while(port.IsOpen()) {
    			byte b = port.get(10);
    			if(port.IsTimeout()) continue;
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
		DataOutputStream outToClient;
		BufferedReader inFromClient;
    	ComPort port = new ComPort();
    	int baud = 9600;
    	boolean bClosed = false;
		
		public Session(Socket s) {
			super();
			connectionSocket = s;
		}

		
    	public void run() {
    		try {
    			inFromClient = new BufferedReader(new InputStreamReader(connectionSocket.getInputStream()));
    			outToClient = new DataOutputStream(connectionSocket.getOutputStream());
    		} catch(IOException e) {
    			e.printStackTrace();
    			return;
    		}
    		String remote = connectionSocket.getRemoteSocketAddress().toString();
    		System.out.println("connected to "+remote);
			while(connectionSocket.isConnected() && !bClosed) {
				String  msg = null;
				try {
					msg = inFromClient.readLine();
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
    			outToClient.writeBytes(s);
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
    					(new Rx(port,this)).start();
    				}  else {
    					send("error\n");
    				}
    				break;
    				
    			case 'c':
    				System.out.println("close");
    				port.close();
    				bClosed = true;
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


