

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import javax.swing.JPanel;


public class Graph extends JPanel {

	Signal sig;
	String label;
	Color col;

	Signal sigX;
	Signal sigY;
	Signal sigX2;
	Signal sigY2;
	
	public Graph(Signal s,String l,Color c) {	//x(t)
		super();
		sig = s;
		label = l;
		col = c;
	}

	public Graph(Signal x,Signal y,Signal x2,Signal y2) {	//y(x)
		super();
		sigX = x;
		sigY = y;
		sigX2 = x2;
		sigY2 = y2;
	}
	
	@Override
	public Dimension getPreferredSize() {
		return new Dimension(300,300);
	}


	private void drawXY(Graphics g,Signal sigX,Signal sigY,int w, int h,Color c,double ampX,double ampY,boolean revX) {
		double[] sX = sigX.getSignal();
		double minX = sigX.getMin();
	
		double[] sY = sigY.getSignal();
		double minY = sigY.getMin();

//		System.out.println("ampX="+ampX+" ampY="+ampY+" minX="+minX+" minY="+minY);
		
		for(int i=0; i < sX.length-1; i++) {
			g.setColor(c);
			int x1 = (int)((double)w * (sX[i]-minX) / ampX);
			int x2 = (int)((double)w * (sX[i+1]-minX) / ampX);
			int y1 = (int)((double)h * (1. - (sY[i]-minY)/ampY));
			int y2 = (int)((double)h * (1. - (sY[i+1]-minY)/ampY));
			if(!revX) g.drawLine(x1,y1,x2,y2); else g.drawLine(w-x1,y1,w-x2,y2); 
			//System.out.println("x1="+x1+" y1="+y1+" x2="+x2+" y1="+y2);
		}
	}
	
	@Override
	public void paintComponent(Graphics g)
    {
		super.paintComponent(g);
		int w = getWidth()-1;
		int h = getHeight()-1;
		
		if(sig != null) {
			double[] signal = sig.getSignal();
			g.setColor(col);
			for(int i=0; i < signal.length-1; i++) {
				g.drawLine((int)((double)w*(double)i/(double)signal.length),
						(int)(h*(0.5-signal[i]/Signal.SIG_MAX)),
						(int)((double)w*(double)(i+1)/(double)signal.length),
						(int)(h*(0.5-signal[i+1]/Signal.SIG_MAX)));
			}
			int markers[] = sig.getMarkers();
			if(markers.length > 0) {
				g.setColor(Color.LIGHT_GRAY);
				for(int i=0; i < markers.length; i++) {
					int x = (int)((double)w*(double)markers[i]/(double)signal.length);
					g.drawLine(x, 0, x, h-1);
				}
			}
		} else {
			double ampX = Math.max(sigX.getPkPk(),sigX2.getPkPk());
			double ampY = Math.max(sigY.getPkPk(),sigY2.getPkPk());
			drawXY(g,sigX,sigY,w,h,Color.RED,ampX,ampY,false);
			drawXY(g,sigX2,sigY2,w,h,Color.GREEN,ampX,ampY,false);
			g.setColor(Color.GRAY);
			g.drawLine(w/2, 0, w/2, h-1);
		}
		
		g.setColor(Color.GRAY);
		g.drawLine(0, h/2, w-1, h/2);
		if(label != null) g.drawString(label, 1, 10);
		
		g.setColor(Color.WHITE);
		g.drawRect(0, 0, w-1, h-1);
    }
}
