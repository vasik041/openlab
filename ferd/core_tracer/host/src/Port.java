
import java.io.*;
import java.net.*;



public class Port {

	private static final int mTimeout = 5000;

	private String host;
	private String port;
	private int baud;
	
	private Socket clientSocket;
	private OutputStream outToServer;
	private InputStream inFromServer;
	private boolean connected;
	
	
	public Port(String h,String p, int b) {
		host = h;
		port = p;
		baud = b;
	}
	
	
	private boolean send(String s,boolean bReconnect) {
		if(clientSocket != null && !clientSocket.isConnected()) connected = false;
		if(!connected && bReconnect) connect();
		if(!connected) return false;
		try {
			for(int i=0; i < s.length(); i++) {
				outToServer.write(s.charAt(i));
			}
		} catch (IOException e) {
			e.printStackTrace();
			return false;
		}
		System.out.println("sent "+s);
		return true;
	}

	
	private String recv(int timeout) {
		int t = 0;
		if(!connected) return null;
		try {
			while(inFromServer.available() == 0 && t < timeout) {
				try {
					Thread.sleep(1);
				} catch(InterruptedException e) {
					return null;
				}
				t++;
			}
		} catch (IOException e) {
			e.printStackTrace();
			return null;
		}
		if(t >= timeout) return null;
		
		try {
			StringBuffer sb = new StringBuffer(16*1024);
			for(;;) {
				int c = inFromServer.read();
				if(c == '\n' || c == -1) break;
				sb.append((char)c);
			}
			System.out.println("rcvd "+sb.toString());
			return sb.toString();
		} catch (IOException e) {
			e.printStackTrace();
		}
		return null;
	}
	
	
	private String sendCommand(String cmd,boolean bReconnect) {
		if(!send(cmd,bReconnect)) return null;
		return recv(mTimeout);
	}
	
	
	public boolean exeCommand(String cmd,boolean bReconnect) {
		String answ = sendCommand(cmd,bReconnect);
		if(answ == null) answ="null";
		System.out.println("Command: "+cmd+" answer: "+answ);
		if(answ.equals("ok")) {
			return true;
		} 
		close();
		return false;
	}
	
	
	private void connect() {
		System.out.println("connecting to "+host);
		try {
			clientSocket = new Socket(host, 8080);
		} catch (UnknownHostException e) {
			e.printStackTrace();
			return;
		} catch (IOException e) {
			e.printStackTrace();
			return;
		}

		try {
			inFromServer = clientSocket.getInputStream();
			outToServer = clientSocket.getOutputStream();
		} catch (IOException e) {
			e.printStackTrace();
			return;
		}
		connected = true;
		System.out.println("connected to "+host);
	
		if(!exeCommand("b"+baud+"\n",false)) {
			close();
			return;
		}
		
		if(!exeCommand("o"+port+"\n",false)) {
			System.out.println("error, trying re-connect...");
			if(!exeCommand("c\n",false)) {
				close();
				return;
			}
			if(!exeCommand("b"+baud+"\n",false)) {
				close();
				return;
			}
			if(!exeCommand("o"+port+"\n",false)) {
				close();
				return;
			}
		}
		try {
			Thread.sleep(50);
		} catch(InterruptedException e) {
		}
	 }
	
	
	public void close() {
		if(!connected) return;
		try {
			inFromServer.close();
			outToServer.close();
			clientSocket.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
		connected = false;
	}

	
	public static String toHex(int n) {
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

	// send command or data to board
	public String sendData(String d,int expectedCS) {
		StringBuffer sb = new StringBuffer();
		for(int i=0; i < d.length(); i++) {
			sb.append(toHex(d.charAt(i)));
		}
		if(!send("d"+sb.toString()+"\n",true)) {
			System.out.println("send failed");
			close();
			return null;
		}
		String answ = recv(mTimeout);
		if(answ != null && answ.charAt(0) == 'd') {
			String data = toStr(answ.substring(1, answ.length()));
			System.out.println("Data answer: "+data);
			if(expectedCS == Integer.parseInt(data.substring(2, 4),16)) {
				System.out.println("cs ok");
			}
			return data;
		}
		close();
		return null;
	}

}
