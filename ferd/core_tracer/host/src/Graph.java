

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.GridLayout;

import javax.swing.JPanel;


public class Graph extends JPanel {

	public static final int MODE_XT=0;
	public static final int MODE_XY=1;
	
	Signal sig1;
	Signal sig2;
	int mode = MODE_XT;
	int marker = -1;
	
	public Graph() {
		super();
	}

	public Graph(int m) {
		super();
		mode = m;
	}

	public void setSignal(Signal s1,Signal s2) {
		sig1 = s1;
		sig2 = s2;
	}
	
	@Override
	public Dimension getPreferredSize() {
		return new Dimension(300,300);
	}
	
	@Override
	public void paintComponent(Graphics g) {
		super.paintComponent(g);
		if(mode == MODE_XY) {
			paintComponentXY(g);
		} else {
			paintComponentXT(g);
		}
	}
	
	private void paintComponentXT(Graphics g) {
		int w = getWidth();
		int h = getHeight();
		double m = 0;
		double m2 = 0;
		if(sig1 != null) {
			m = sig1.getMax();
		}
		if(sig2 != null) {
			m2 = sig2.getMax();
//			if(m2 > m) m = m2;
		}
		if(sig1 != null) {
			double[] signal = sig1.getSignal();
			g.setColor(Color.RED);
			for(int i=0; i < signal.length-1; i++) {
				g.drawLine(5+(int)((double)i*((double)w-10)/signal.length),
						h-5-(int)((double)(h-10)*signal[i]/m),
						5+(int)(((double)i+1)*((double)w-10)/signal.length),
						h-5-(int)((double)(h-10)*signal[i+1]/m));
			}
		}
		if(sig2 != null) {
			double[] signal = sig2.getSignal();
			g.setColor(Color.BLUE);
			for(int i=0; i < signal.length-1; i++) {
				g.drawLine(5+i*(w-10)/signal.length,
						h-5-(int)((double)(h-10)*signal[i]/m2),
						5+(i+1)*(w-10)/signal.length,
						h-5-(int)((double)(h-10)*signal[i+1]/m2));
			}
		}
		g.setColor(Color.BLACK);
		g.drawLine(5, h-5, w-5, h-5);
		g.drawLine(5, h-5, 5, 5);
    }

	
	private void paintComponentXY(Graphics g) {
		int w = getWidth();
		int h = getHeight();
		if(sig1 != null && sig2 != null) {
			double m = sig1.getMax();;
			double m2 = sig2.getMax();
			double[] signal = sig1.getSignal();
			double[] signal2 = sig2.getSignal();

			g.setColor(Color.RED);
			for(int i=0; i < signal.length-1; i++) {
				int x =  (int)((double)(h-10)*signal[i]/m);
				int x2 = (int)((double)(h-10)*signal[i+1]/m);
				int y =  (int)((double)(h-10)*signal2[i]/m2);
				int y2 = (int)((double)(h-10)*signal2[i+1]/m2);
				g.drawLine(5+x,	h-5-y /*5+y*/, 5+x2, h-5-y2 /*5+y2*/);
				if(i == marker) g.setColor(Color.BLUE);
			}
		}
		g.setColor(Color.BLACK);
		g.drawLine(5, h-5, w-5, h-5);
		g.drawLine(5, h-5, 5, 5);
    }

	public Signal getSignal1() {
		return sig1;
	}
	
	public Signal getSignal2() {
		return sig2;
	}

	public void setMarker(int x) {
		marker = x;
		System.out.println("marker = "+x);
	}
}
