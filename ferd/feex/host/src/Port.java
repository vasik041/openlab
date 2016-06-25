
import java.io.*;
import java.net.*;



public class Port {

	String host;
	String port;
	int baud;
	
	Socket clientSocket;
	DataOutputStream outToServer;
	BufferedReader inFromServer;
	boolean connected;
	
	
	public Port(String h,String p, int b) {
		host = h;
		port = p;
		baud = b;
	}
	
	
	public void send(String s) {
		if(clientSocket != null && !clientSocket.isConnected()) connected = false;
		if(!connected) connect();
		if(!connected) return;
		try {
			outToServer.writeBytes(s);
		} catch (IOException e) {
			e.printStackTrace();
			connected = false;
			return;
		}
		System.out.println("sent "+s);
	}

	
	public String recv(int timeout) {
		if(!connected) return null;
		try {
			while(!inFromServer.ready()) {
				try {
					Thread.sleep(1);
				} catch(InterruptedException e) {
				}
			}
		} catch (IOException e) {
			e.printStackTrace();
			return null;
		}
		try {
			String s = inFromServer.readLine();
			System.out.println("rcvd "+s);
			return s;
		} catch (IOException e) {
			e.printStackTrace();
		}
		return null;
	}
	
	
	public String sendCommand(String cmd) {
		send(cmd);
		return recv(1000);
	}
	
	
	public void exeCommand(String cmd) {
		String answ = sendCommand(cmd);
		if(answ == null) answ="null";
		if(answ.equals("ok")) {
			System.out.println("Command: "+cmd+" answer: "+answ);
		}
	}
	
	
	public void connect() {
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
			inFromServer = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
			outToServer = new DataOutputStream(clientSocket.getOutputStream());
		} catch (IOException e) {
			e.printStackTrace();
			return;
		}
		connected = true;
		
		exeCommand("b"+baud+"\n");
		exeCommand("o"+port+"\n");
	 }
	
	
	public void close() {
		if(!connected) return;
		exeCommand("c\n");
		try {
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
	
	
	public String sendData(String d,int expectedCS) {
		StringBuffer sb = new StringBuffer();
		for(int i=0; i < d.length(); i++) {
			sb.append(toHex(d.charAt(i)));
		}
		send("d"+sb.toString()+"\n");
		String answ = recv(1000);
		if(answ != null && answ.charAt(0) == 'd') {
			String data = toStr(answ.substring(1, answ.length()));
			System.out.println("Data answer: "+data);
			if(expectedCS == Integer.parseInt(data.substring(2, 4),16)) {
				System.out.println("cs ok");
			}
			return data;
		}
		return null;
	}

}
