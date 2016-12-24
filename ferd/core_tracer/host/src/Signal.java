
import java.util.Arrays;



public class Signal {
	
	double[] signal;
	
	public Signal() {
	}

	public Signal(Signal s) {
		setSignal(s.signal,s.signal.length);
	}
	
	public double[] getSignal() {
		return signal;
	}

	public void setSignal(double[] s1,int len) {
		signal = new double[len];
		signal = Arrays.copyOf(s1, len);
	}

	public void clip(int ix) {
		for(int i=0; i < ix; i++)
			signal[i] = 0;
	}
	
	public double getMax() {
		double m = signal[0];
		for(int i=1; i < signal.length; i++) {
			if(signal[i] > m) {
				m = signal[i];
			}
		}
		return m;
	}

	
	public String toString() {
		StringBuffer b = new StringBuffer();
		for(int i=0; i < signal.length; i++) {
			b.append(""+signal[i]);
			if(i != signal.length-1) b.append(",");
		}
		return b.toString();
	}
	
	public void smooth() {
		for(int i=0; i < signal.length-3; i++) {
			signal[i] = (signal[i] + signal[i+1] + signal[i+2])/3.; 
		}
		
		signal[signal.length-2] = (signal[signal.length-2] + signal[signal.length-1] + signal[signal.length-3])/3.;
		signal[signal.length-1] = (signal[signal.length-1] + signal[signal.length-2] + signal[signal.length-3])/3.;
	}
	
	
	public int getMaxIndex() {
		double m = signal[0];
		int maxIndex = 0;
		for(int i=1; i < signal.length; i++) {
			if(signal[i] > m) {
				m = signal[i];
				maxIndex = i;
			}
		}
		return maxIndex;
	}

	
	public void scale(double k) {
		for(int i=0; i < signal.length; i++) {
			signal[i] *= k;
		}
	}

	
	public double intTrap(int start,int len) {
		double m = (signal[start]+signal[start+len-1])/2.;
		for(int i=start+1; i < start+len-1; i++) {
			m += signal[i];
		}
		return m;
	}

	public double intTrap2(int start,int len) {
		double m = (signal[start]*signal[start]+signal[start+len-1]*signal[start+len-1])/2.;
		for(int i=start+1; i < start+len-1; i++) {
			m += signal[i]*signal[i];
		}
		return m;
	}

}
