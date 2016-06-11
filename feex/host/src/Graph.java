

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.GridLayout;

import javax.swing.JPanel;


public class Graph extends JPanel {

	Signal sig1;
	Signal sig2;
	
	public Graph() {
		super();
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
		int w = getWidth();
		int h = getHeight();
		double m = 0;
		if(sig1 != null) {
			m = sig1.getMax();
		}
		if(sig2 != null) {
			double m2 = sig2.getMax();
			if(m2 > m) m = m2;
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
						h-5-(int)((double)(h-10)*signal[i]/m),
						5+(i+1)*(w-10)/signal.length,
						h-5-(int)((double)(h-10)*signal[i+1]/m));
			}
		}
		g.setColor(Color.BLACK);
		g.drawLine(5, h-5, w-5, h-5);
		g.drawLine(5, h-5, 5, 5);
    }

}
