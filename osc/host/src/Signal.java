import java.util.ArrayList;
import java.util.Arrays;



public class Signal {
	public static final int SIG_LEN = 1000;
	public static final double SIG_MAX = 1024.;
	
	double[] signal = new double [SIG_LEN];
	int start = 0;
	ArrayList<Integer> markers = new ArrayList<>();
	
	public Signal() {
		for(int i=0; i < signal.length; i++) {
			signal[i] = (double)i * SIG_MAX / (double)signal.length - SIG_MAX/2.;
		}
	}
	
	public double[] getSignal() {
		return signal;
	}

	public double[] getSignal(int start,int len) {
		double[] s1 = new double [len];
		for(int i=0; i < len; i++) {
			s1[i] = signal[i+start];
		}
		return s1;
	}

	public void setSignal(double[] s1) {
		signal = Arrays.copyOf(s1, s1.length);
	}

	
	public String toString() {
		StringBuffer sb = new StringBuffer();
		for(int i=0; i < signal.length; i++) {
			sb.append(((i==0)?"":",")+signal[i]);
		}
		return sb.toString();
	}

	
	public double getMax() {
		double m = signal[0];
		for(int i=1; i < signal.length; i++) {
			if(signal[i] > m) m = signal[i];
		}
		return m;
	}

	
	public double getMin() {
		double m = signal[0];
		for(int i=1; i < signal.length; i++) {
			if(signal[i] < m) m = signal[i];
		}
		return m;
	}
	
	
	public double getPkPk() {
		return Math.abs(getMax()-getMin());
	}
	
	
	public double getMean() {
		double m = 0.;
		for(int i=0; i < signal.length; i++) {
			m += signal[i];
		}
		return m / (double)signal.length;
	}
	
	
	public void addMarker(int x) {
		markers.add(new Integer(x));
	}
	
	public int[] getMarkers() {
		int[] m = new int[markers.size()];
		for(int i=0; i < markers.size(); i++) {
			m[i] = markers.get(i).intValue();
		}
		return m;
	}
	
	public int getStart() {
		return start;
	}

	public void setStart(int s) {
		start = s;
	}
	
	public int getLength() {
		return signal.length;
	}
	
	
	public boolean getSync(Signal syncU,Signal syncD) {
		int level = 0;
		int treshold = 5;
		int i = 0;
		
		if(signal[0] < level) {
			while(i < signal.length && signal[i] <= level) i++;
			i+=treshold;
			while(i < signal.length && signal[i] >= level) i++;
			i+=treshold;
		}
		
		while(i < signal.length && signal[i] >= level) i++;
		int z1 = i;
		i+=treshold;
		while(i < signal.length && signal[i] <= level) i++;
		int z2 = i;
		i+=treshold;
		while(i < signal.length && signal[i] >= level) i++;
		int z3 = i;
		i+=treshold;
		while(i < signal.length && signal[i] <= level) i++;
		int z4 = i;

		if(z1 >= signal.length || z2 >= signal.length) return false;
		if(z3 >= signal.length || z4 >= signal.length) return false;
		System.out.println("z1="+z1+" z2="+z2+" z3="+z3+" z4="+z4);
				
		double min12 = signal[z1];
		int start = z1;
		for(i=z1+1; i < z2; i++) {
			if(signal[i] < min12) {
				min12 = signal[i];
				start = i;
			}
		}
		
		double max23 = signal[z2];
		int end = z2;
		for(i=z2+1; i < z3; i++) {
			if(signal[i] > max23) {
				max23 = signal[i];
				end = i;
			}
		}
		
		double min34 = signal[z3];
		int end2 = z3;
		for(i=z3+1; i < z4; i++) {
			if(signal[i] < min34) {
				min34 = signal[i];
				end2 = i;
			}
		}
		
		syncU.setSignal(getSignal(start,end-start+1));
		syncU.setStart(start);
		System.out.println("start="+start+" end="+end+" len="+(end-start)+" min="+syncU.getMin()+" max="+syncU.getMax());
//		System.out.println("syncU="+syncU.toString());
		
		syncD.setSignal(getSignal(end,end2-end+1));
		syncD.setStart(end);
		System.out.println("end2="+end2+" len2="+(end2-end)+" min="+syncD.getMin()+" max="+syncD.getMax());
//		System.out.println("syncD="+syncD.toString());
		return true;
	}
}
