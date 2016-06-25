

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.GridLayout;

import javax.swing.JPanel;


public class Graph extends JPanel {

	int fq;
	int tau1;
	int delay2;
	int tau2;
	
	public Graph() {
		super();
	}

	public void setVals(int freq,int t1,int dly2,int t2) {
		fq = freq;
		tau1 = t1;
		delay2 = dly2;
		tau2 = t2;
//		System.out.println("fq="+fq);
//		System.out.println("t1="+t1);
//		System.out.println("d2="+dly2);
//		System.out.println("t2="+t2);
	}
	
	@Override
	public Dimension getPreferredSize() {
		return new Dimension(200,200);
	}
	
	@Override
	public void paintComponent(Graphics g) {
		super.paintComponent(g);
		int w = getWidth();
		int h = getHeight();

		int hs = h/5;
		int w1 = w-10;
		int t1 = (int)((double)tau1 * (double)w1 / (double)fq); 
		int d2 = (int)((double)delay2 * (double)w1 / (double)fq); 
		int t2 = (int)((double)tau2 * (double)w1 / (double)fq); 
				
		g.setColor(Color.RED);
		g.drawLine(5,2*hs,5,hs);
		g.drawLine(5,hs,5+t1,hs);
		g.drawLine(5+t1,hs,5+t1,2*hs);
		g.drawLine(5+t1,2*hs,w-5,2*hs);

		g.setColor(Color.BLUE);
		g.drawLine(5,4*hs,5+d2,4*hs);
		g.drawLine(5+d2,4*hs,5+d2,3*hs);
		g.drawLine(5+d2,3*hs,5+d2+t2,3*hs);
		g.drawLine(5+d2+t2,3*hs,5+d2+t2,4*hs);
		g.drawLine(5+d2+t2,4*hs,w-5,4*hs);
		
		g.setColor(Color.LIGHT_GRAY);
		g.drawLine(2, h-5, w-5, h-5);
		g.drawLine(4, h-3, 4, 5);
    }

}
