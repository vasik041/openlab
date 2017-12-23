

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Container;
import java.awt.FlowLayout;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.security.Timestamp;
import java.sql.Date;
import java.text.DecimalFormat;
import java.text.NumberFormat;
import java.text.SimpleDateFormat;
import java.util.Random;

import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;



public class Main implements ParamChanged {
	final static String VERSION = "0.2";
	final static String PROPS_PATH = "/com/lz/core_tracer";
	
	static class theLock extends Object {
	}
	
	static public theLock lockObject = new theLock();
	
	Comm d = new Comm(this);
//	Preferences prefs = Preferences.systemRoot().node("/com/lz/feex");
	
	Param freq = new Param("Freq:", 1,100, 5, "ms", "freq", 20, this);
	Param tpls = new Param("Tpls:", 1,100, 5, "us", "tpls", 1,  this);
	
	Graph g;
	Graph gxy;
	JButton bStart;
	JLabel lInfo;
	
	public void addComponentToPane(Container pane) {
		// controls panel
		JPanel cpanel0 = new JPanel(new GridLayout(2,1));
        cpanel0.add(freq);
        cpanel0.add(tpls);
//        cpanel0.setBorder(BorderFactory.createMatteBorder(0,0,0,1,Color.GRAY));

        JPanel cpanel = new JPanel(new BorderLayout());
        cpanel.add(cpanel0,BorderLayout.NORTH);

        //XT graph
        JPanel gpanel0 = new JPanel(new BorderLayout());
        g = new Graph();
        gpanel0.add(g,BorderLayout.CENTER);
        
        JPanel gpanel1 = new JPanel(new FlowLayout(FlowLayout.RIGHT));
        JButton bSmoothX = new JButton("Smooth X");
        bSmoothX.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				smoothWave(true);
			}
        });
        JButton bSmoothY = new JButton("Smooth Y");
        bSmoothY.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				smoothWave(false);
			}
        });
        gpanel1.add(bSmoothX);        
        gpanel1.add(bSmoothY);        

        JPanel gpanel = new JPanel(new BorderLayout());
        gpanel.add(gpanel0,BorderLayout.CENTER);
        gpanel.add(gpanel1,BorderLayout.SOUTH);

        //XY graph
        JPanel hpanel0 = new JPanel(new BorderLayout());
        gxy = new Graph(Graph.MODE_XY);
        hpanel0.add(gxy,BorderLayout.CENTER);

        JPanel hpanel1 = new JPanel(new FlowLayout(FlowLayout.RIGHT));
        JButton bSave = new JButton("Save");
        bSave.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				saveWave();
			}
        });
        hpanel1.add(bSave);        
        
        JPanel hpanel = new JPanel(new BorderLayout());
        hpanel.add(hpanel0,BorderLayout.CENTER);
        hpanel.add(hpanel1,BorderLayout.SOUTH);
        
        // combine all together
        JPanel panel = new JPanel(new GridLayout(1,3));
        panel.add(cpanel);
        panel.add(gpanel);
        panel.add(hpanel);
        
        pane.add(panel);
    }

		
	private static void createAndShowGUI() {
        //Create and set up the window.
        JFrame frame = new JFrame("Core Tracer v. "+VERSION);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        
        //Create and set up the content pane.
        Main p = new Main();
        p.addComponentToPane(frame.getContentPane());

        frame.addWindowListener(new java.awt.event.WindowAdapter() {
            @Override
            public void windowClosing(java.awt.event.WindowEvent windowEvent) {
                Utils.storeSizeAndPosition(frame);
            }
        });
        
        //Display the window.
        frame.pack();
        Utils.restoreSizeAndPosition(frame);
        frame.setVisible(true);
    }

	
	public static void main(String[] args) {
        /* Use an appropriate Look and Feel */
        try {
            //UIManager.setLookAndFeel("com.sun.java.swing.plaf.windows.WindowsLookAndFeel");
            UIManager.setLookAndFeel("javax.swing.plaf.metal.MetalLookAndFeel");
        } catch (UnsupportedLookAndFeelException ex) {
            ex.printStackTrace();
        } catch (IllegalAccessException ex) {
            ex.printStackTrace();
        } catch (InstantiationException ex) {
            ex.printStackTrace();
        } catch (ClassNotFoundException ex) {
            ex.printStackTrace();
        }
        /* Turn off metal's use of bold fonts */
        UIManager.put("swing.boldMetal", Boolean.FALSE);
         
        //Schedule a job for the event dispatch thread:
        //creating and showing this application's GUI.
        javax.swing.SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                createAndShowGUI();
            }
        });
    }


	

	void setWave(double chX[],double chY[]) {
		Signal sX = new Signal();
		sX.setSignal(chX, chX.length);
		sX.clip(2);
		Signal sY = new Signal(2048);
		sY.setSignal(chY, chY.length);
		sY.clip(2);
		sX.smooth();
		sY.smooth();
		g.setSignal(sX, sY);
		gxy.setSignal(sX, sY);
		gxy.setMarker((int)((double)tpls.getValue()/1.5));
		g.updateUI();
		gxy.updateUI();
		printStats();
	}
	
	
	private void smoothWave(boolean bX) {
		Signal sX = g.getSignal1();
		Signal sY = g.getSignal2();
		if(bX) sX.smooth(); else sY.smooth();
		g.setSignal(sX, sY);
		gxy.setSignal(sX, sY);
		g.updateUI();
		gxy.updateUI();
		printStats();
	}


	private void saveWave() {
		Date date = new Date(System.currentTimeMillis());
		String ts = new SimpleDateFormat("ddMMyy-hhmmss").format(date);
		String fileName = "wave-"+ts+".txt";
		BufferedWriter writer = null;
		try {
		    writer = new BufferedWriter(new FileWriter(fileName));
		    writer.write("freq: "+freq.getValue()+"ms\n");
		    writer.write("tpls: "+tpls.getValue()+"us\n");
		    writer.write("len:  "+g.getSignal1().getSignal().length+"\n");
		    writer.write("sX:   "+g.getSignal1().toString()+"\n");
		    writer.write("sY:   "+g.getSignal2().toString()+"\n");
		} catch(IOException e) {
			e.printStackTrace();
		}

		try {
			if(writer != null) writer.close();
	    } catch ( IOException e) {
	    	e.printStackTrace();
	    }
	}

	
	private String r(double x) {
		return String.format("%3.2g",x);
	}

	private String d(double x) {
		return String.format("%3.2f",x);
	}
	
	static double Ris = 0.58;
	static double Ups = 13.72;
	static double Rload = 5.06;
	static double Igain = 4;
	
	
	private void printStats() {
		double dT = 1.5e-6;
		double Pout = 0;
		Signal sX = new Signal(g.getSignal1());
		sX.scale(3.3/Ris/Igain/4096.);
		int maxIndex = sX.getMaxIndex();
		if(maxIndex == 0) return;
		System.out.println("maxIndex = "+maxIndex);
		double Ein = Ups * sX.intTrap(0, maxIndex) * dT;
		double Eout = Rload * sX.intTrap2(maxIndex,sX.signal.length-maxIndex) * dT;
		double COP = Eout * 100. / Ein;
		if(Eout > Ein) {
			Pout = (Eout-Ein)*1000.* (1000./(double)freq.getValue());
		}
		System.out.println("Ein = "+r(Ein)+" Eout = "+r(Eout)+" COP = "+d(COP)+"%"+ (Pout > 0 ? " Pout = "+d(Pout)+" mW" : ""));
	}


	@Override
	public void syncHW(String param, int val) {
		if(param == "freq") {
			d.setFreq(val);
		} else if(param == "tpls") {
			d.setTpls(val);
		}
	}
	
	
	void refreshSettings(int fq,int pls) {
		freq.setValue(fq);
		tpls.setValue(pls);
	}
}


