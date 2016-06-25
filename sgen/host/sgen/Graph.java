package sgen;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import javax.swing.JPanel;


public class Graph extends JPanel {

	Signal sig;
	
	public Graph(Signal s) {
		super();
		sig = s;
	}
	
	@Override
	public Dimension getPreferredSize() {
		return new Dimension(300,300);
	}
	
	@Override
	public void paintComponent(Graphics g)
    {
		super.paintComponent(g);
		int w = getWidth();
		int h = getHeight();
		double[] signal = sig.getSignal();
		g.setColor(Color.RED);
		for(int i=0; i < signal.length-1; i++) {
			g.drawLine(i*w/signal.length,
						h*(128-(int)signal[i])/256,
						(i+1)*w/signal.length,
						h*(128-(int)signal[i+1])/256);
		}
		g.setColor(Color.GRAY);
		g.drawLine(0, h/2, w-1, h/2);
		g.setColor(Color.WHITE);
		g.drawRect(0, 0, w-1, h-1);
    }
}
