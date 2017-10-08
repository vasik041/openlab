import java.io.*;

import lcm.lcm.*;
import exlcm.*;

	
public class Comm implements LCMSubscriber {

	Main ui;
	LCM lcm;
    	
	public Comm(Main ui) {
		this.ui = ui;
	    try {
	    	lcm = new LCM();
	    }
	    catch (IOException ex) {
            System.out.println("Exception: " + ex);
        }
	    lcm.subscribe("FEEX.MEAS", this);
	}

	
	public static int h2d(int h) {
		if(h >= '0' && h <= '9') return h-'0';
		if(h >= 'a' && h <= 'f') return h-'a'+10;
		if(h >= 'A' && h <= 'F') return h-'A'+10;
		return 0;
	}
	
	public static int hex2int(byte[] msg,int offset,int len) {
		int n = 0;
		for(int i=0; i < len; i++) {
			n *= 16;
			n += h2d(msg[offset+i]);
		}
		return n;
	}
	
	
	public static int d2h(int b) {
		 b &= 0x0f; 
		return (b < 10) ? ('0'+b) : ('A'+b-10);
	}
	
	public static void print(String msg,byte[] buf) {
		System.out.print(msg+" ");
		for(int i=0; i < buf.length; i++) {
			System.out.print(""+(char)buf[i]);
		}
		System.out.println();
	}
	
	
	public void messageReceived(LCM lcm, String channel, LCMDataInputStream ins) {
		try {
			if (channel.equals("FEEX.MEAS")) {
				feex_t msg = new feex_t(ins);
//				print("dev->",msg.msg);
				if(msg.len == 19 && msg.msg[0] == 'p') {
					int Ui = hex2int(msg.msg,1,4);
					int Ii = hex2int(msg.msg,5,4);
					int Uo = hex2int(msg.msg,9,4);
					int Io = hex2int(msg.msg,13,4);
//					System.out.println("Ui = " + Ui + " Ii = " + Ii + " Uo = " + Uo + " Io = " + Io);
					ui.refreshMeasurements(Ui, Ii, Uo, Io);
				} else if(msg.len == 23 && msg.msg[0] == 'z') {
					int freq = hex2int(msg.msg,1,8) / 1000000;
					int pulse = hex2int(msg.msg,9,8) / 1000;
					int psu = Utils.convetPSUback(hex2int(msg.msg,17,2));
					int load = Utils.convetLoadback(hex2int(msg.msg,19,2));
					System.out.println("freq = " + freq + " tpls = " + pulse + " psu = " + psu + " load = " + load);
					ui.refreshSettings(freq,pulse,psu,load);
				} else {
					System.out.println("Wrong length: " + msg.len);
				}
			}
		} catch (IOException ex) {
			System.out.println("Exception: " + ex);
		}
	}
	

    public void send(byte[] msg) {
    	print("dev<-",msg);
        feex_t msgImpl = new feex_t();
        msgImpl.len = (byte)msg.length;
        msgImpl.msg = msg;
        lcm.publish ("FEEX.CTRL", msgImpl);
    }

	
	public void setFreq(int fq) {
		System.out.println("freq = "+fq);
		int val = fq * 1000000; //ms -> ns
		int a = val & 0xff;
		int b = (val >> 8) & 0xff;
		int c = (val >> 16) & 0xff;
		int d = (val >> 24) & 0xff;
		int cs = (a ^ b ^ c ^ d) & 0xff;
		
		byte[] msg = new byte[] { 'f',
								(byte)d2h(a>>4), (byte)d2h(a),
								(byte)d2h(b>>4), (byte)d2h(b),
								(byte)d2h(c>>4), (byte)d2h(c),
								(byte)d2h(d>>4), (byte)d2h(d),
								(byte)d2h(cs>>4),(byte)d2h(cs),
		};
		send(msg);
	}

	
	public void setTpls(int tpls) {
		System.out.println("tpls = "+tpls);
		int val = tpls * 1000;	// us -> ns
		int a = val & 0xff;
		int b = (val >> 8) & 0xff;
		int c = (val >> 16) & 0xff;
		int d = (val >> 24) & 0xff;
		int cs = (a ^ b ^ c ^ d) & 0xff;
		
		byte[] msg = new byte[] { 't',
								(byte)d2h(a>>4), (byte)d2h(a),
								(byte)d2h(b>>4), (byte)d2h(b),
								(byte)d2h(c>>4), (byte)d2h(c),
								(byte)d2h(d>>4), (byte)d2h(d),
								(byte)d2h(cs>>4), (byte)d2h(cs),
		};
		send(msg);
	}
	
	
	public void setPSU(int v) {
		System.out.println("psu = "+v);
		int val = Utils.convetPSU(v);
		int a = val & 0xff;
		int cs = (a ^ 0x55) & 0xff;
		byte[] msg = new byte[] { 'u',
								(byte)d2h(a>>4),  (byte)d2h(a),
								(byte)d2h(cs>>4), (byte)d2h(cs),
		};
		send(msg);
	}
	
	
	public void setLoad(int p) {
		System.out.println("load = "+p);
		int val = Utils.convetLoad(p);
		int a = val & 0xff;
		int cs = (a ^ 0x55) & 0xff;
		byte[] msg = new byte[] { 'l',
								(byte)d2h(a>>4),  (byte)d2h(a),
								(byte)d2h(cs>>4), (byte)d2h(cs)
		};
		send(msg);
	}
	
}

