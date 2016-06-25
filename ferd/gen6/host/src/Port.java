



public class Port {

	ComPort p;
	
	public Port(String port, int baud) {
		p = new ComPort();
		p.open(port, baud);
	}
	
	private void set(int a,int d) {
		p.put(a & 0x7f);
		p.put(d | 0x80);
	}
	
	public void setFreq(int fq) {
		set(0x7f,fq);
	}

	public void setT(int t1,int d2,int t2) {
		int[] sig = new int[128];
		int i;
		for(i=0; i < 127; i++) {
			if(i < t1) sig[i] = 7-2; else sig[i] = 7;
			if(i >= d2 && i < d2+t2) sig[i] &= ~4;
		}
		for(i=0; i < 127; i++) {
			set(i,sig[i]);
		}
	}
	
	public void close() {
		p.close();
	}
}
