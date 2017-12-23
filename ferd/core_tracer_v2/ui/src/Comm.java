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
	
	
	double[] chX = new double[1024];
	double[] chY = new double[1024];
	
	private void parseWave(byte[] msg,double[] w,boolean bCenter) {
		int cs = 0;
		for(int i=0; i < 1024; i++) {
			int x = hex2int(msg,1+4*i,4);
			cs ^= x;
			w[i] = (double)x;
			if(bCenter) {
				w[i] = 4096. - w[i];
			}
		}
		cs ^= 0x1234;
		if(cs != hex2int(msg,1+4*1024,4)) {
			System.err.println("check sum mismatch "+cs+" vs "+hex2int(msg,1+4*1024,4));
		}
	}
	
	
	private int xor32(int x) {
		int a = x & 0xff;
		int b = (x >> 8) & 0xff;
		int c = (x >> 16) & 0xff;
		int d = (x >> 24) & 0xff;
		return a ^ b ^ c ^ d;
	}
	
	
	public void messageReceived(LCM lcm, String channel, LCMDataInputStream ins) {
		try {
			if (channel.equals("FEEX.MEAS")) {
				feex_t msg = new feex_t(ins);
				//print("dev->",msg.msg);
				
				if(msg.len == 4101 && msg.msg[0] == 'i') {
					parseWave(msg.msg,chX,false);
				} else if(msg.len == 4101 && msg.msg[0] == 'u') {
					parseWave(msg.msg,chY,true);
					ui.setWave(chX,chY);
				} else if(msg.len == 19 && msg.msg[0] == 'z') {
					int freq = hex2int(msg.msg,1,8);
					int pulse = hex2int(msg.msg,9,8);
					int cs = xor32(pulse) ^ xor32(freq);
					if(cs != hex2int(msg.msg,1+8+8,2)) {
						System.err.println("check sum mismatch "+cs+" vs "+hex2int(msg.msg,1+8+8,2));
					}
					freq /= 10000;
					pulse /= 10;
					System.out.println("freq = " + freq + " tpls = " + pulse);
					ui.refreshSettings(freq,pulse);
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
		int val = fq * 10000;
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
		int val = tpls * 10;
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
	
	
	private void parseAnsw(String answ,int len, double[] sig,int offset, boolean center) {
		int cs = 0;
		len /= 2;
		for(int i=0; i < len; i++) {
			String sA = answ.substring(i*8, i*8+4);
			String sB = answ.substring(i*8+4, i*8+4+4);
			int a = Integer.parseInt(sA,16);
			cs ^= a;
			int b = Integer.parseInt(sB,16);
			cs ^= b;
			sig[offset+2*i] = a;
			sig[offset+2*i+1] = b;
			if(center) {
				sig[offset+2*i]   = 4096 - sig[offset+2*i];	
				sig[offset+2*i+1] = 4096 - sig[offset+2*i+1];	
			}
		}
		cs ^= 0x1234;
		String css = answ.substring(len*8, len*8+4);
		if(cs == Integer.parseInt(css,16)) {
			System.out.println("cs ok");
		} else {
			System.out.println("cs="+css+" expected="+cs);
		}
	}

}

