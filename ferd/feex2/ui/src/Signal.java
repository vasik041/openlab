
import java.util.Arrays;



public class Signal {
	
	double[] signal;
	
	public Signal() {
	}
	
	public double[] getSignal() {
		return signal;
	}

	public void setSignal(double[] s1,int len) {
		signal = new double[len];
		signal = Arrays.copyOf(s1, len);
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

	
}
