package sgen;

public class Signal {
	public enum Shape { TREA, EXP, SIN, SIN2 };

	private int minA;
	private int maxA;
	private int tau;
	private Shape shape;
	
	public void setTau(int t) {
		tau = t;
	}

	public void setMinA(int a) {
		minA = a;
	}

	public void setMaxA(int a) {
		maxA = a;
	}
	
	public void setShape(Shape s) {
		shape = s;
	}
	
	public double[] getSignal() {
		double[] s = new double [256];
		if(shape == Shape.TREA) {
			for(int i=0; i < tau; i++) {
				s[i] = minA + (maxA-minA) * (double)i / (double)tau;
			}
			for(int i=tau; i < s.length; i++) {
				s[i] = maxA - (maxA-minA) * (double)(i-tau) / (double)(s.length-tau);
			}
		} else if(shape == Shape.SIN) {
			for(int i=0; i < tau; i++) {
				s[i] = minA + (maxA-minA) * (0.5+Math.sin(Math.PI * i / tau)/2.);
			}
			for(int i=tau; i < s.length; i++) {
				s[i] = minA+(maxA-minA) * (0.5+Math.sin(Math.PI+Math.PI * (i-tau) / (s.length-tau))/2.);
			}
		} else if(shape == Shape.SIN2) {
			for(int i=0; i < tau; i++) {
				s[i] = minA + (maxA-minA) * Math.sin(Math.PI/2 * i / tau);
			}
			for(int i=tau; i < s.length; i++) {
				s[i] = minA+(maxA-minA) * Math.sin(Math.PI/2+Math.PI/2 * (i-tau) / (s.length-tau));
			}
		} else if(shape == Shape.EXP) {
			for(int i=0; i < tau; i++) {
				s[i] = minA + (maxA-minA) * ((Math.exp((double)i/tau)-1)/(Math.E-1));
			}
			for(int i=tau; i < s.length; i++) {
				s[i] = maxA - (maxA-minA) * (Math.exp((double)(i-tau) / (s.length-tau))-1)/(Math.E-1);
			}
		}
		return s;
	}
}
