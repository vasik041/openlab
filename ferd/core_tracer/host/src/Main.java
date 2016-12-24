

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
	final static String VERSION = "0.1";
	final static String PROPS_PATH = "/com/lz/core_tracer";
	
	static class theLock extends Object {
	}
	
	static public theLock lockObject = new theLock();
	
	Port port = new Port("dev041","COM4",9600);
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

        JPanel cpanel1 = new JPanel(new FlowLayout(FlowLayout.RIGHT));
        JButton bSync = new JButton("Sync");
        bSync.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				syncSettings();
			}
        });
        cpanel1.add(bSync);

        JPanel cpanel = new JPanel(new BorderLayout());
        cpanel.add(cpanel0,BorderLayout.NORTH);
        cpanel.add(cpanel1,BorderLayout.SOUTH);

        //XT graph
        JPanel gpanel0 = new JPanel(new BorderLayout());
        g = new Graph();
        gpanel0.add(g,BorderLayout.CENTER);
        
        JPanel gpanel1 = new JPanel(new FlowLayout(FlowLayout.RIGHT));
        JButton bGetW = new JButton("Get Wave");
        bGetW.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				getWave();
			}
        });
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
        gpanel1.add(bGetW);        

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
            	p.port.exeCommand("c\n",false);
            	p.port.close();
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


	@Override
	public void syncHW(String param, int val) {
		synchronized (lockObject) {
			syncHWImpl(param,val);
		}
	}
	
	
	public void syncHWImpl(String param, int val) {
		String answ = null;
		if(param.equals("freq")) {
			val *= 10000; //ms -> 100ns
			int a = val & 0xff;
			int b = (val >> 8) & 0xff;
			int c = (val >> 16) & 0xff;
			int d = (val >> 24) & 0xff;
			int cs = (a ^ b ^ c ^ d) & 0xff;
			answ = port.sendData("f"+Port.toHex(a)+Port.toHex(b)+Port.toHex(c)+Port.toHex(d),cs);
		} else if(param.equals("tpls")) {
			val *= 10;	// us -> 100ns
			int a = val & 0xff;
			int b = (val >> 8) & 0xff;
			int c = (val >> 16) & 0xff;
			int d = (val >> 24) & 0xff;
			int cs = (a ^ b ^ c ^ d) & 0xff;
			answ = port.sendData("t"+Port.toHex(a)+Port.toHex(b)+Port.toHex(c)+Port.toHex(d),cs);
		}
		if(answ == null) answ = "null";
		System.out.println("answ = "+answ);
	}

	
	private void syncSettings() {
		String answ;
		synchronized (lockObject) {
			answ = port.sendData("s",0);
		}
		if(answ != null && answ.startsWith("z") && answ.length() == 16+1+2) {
			int fq = Integer.parseInt(answ.substring(1, 1+8),16);
			int tau = Integer.parseInt(answ.substring(1+8, 1+8+8),16);
			fq /= 10000;
			tau /= 10;
			System.out.println("freq = "+fq);
			System.out.println("tau = "+tau);
			freq.setValue(fq);
			tpls.setValue(tau);
		} else {
			System.out.println("com. error " + ((answ == null) ? "timeout" : "wrong answer: "+answ) );
		}
	}

	
	private void parseAnsw(String answ,int len, double[] sig,int offset, boolean center) {
		int cs = 0;
		len /= 2;
		for(int i=0; i < len; i++) {
			String sA = answ.substring(i*8, i*8+4);
			String sB = answ.substring(i*8+4, i*8+4+4);
			int a = Integer.parseInt(sA,16);
			cs ^= a;
			int b = Integer.parseInt(sB,16);
			cs ^= b;
			sig[offset+2*i] = a;
			sig[offset+2*i+1] = b;
			if(center) {
				sig[offset+2*i]   = 4096 - sig[offset+2*i];	
				sig[offset+2*i+1] = 4096 - sig[offset+2*i+1];	
			}
		}
		cs ^= 0x1234;
		String css = answ.substring(len*8, len*8+4);
		if(cs == Integer.parseInt(css,16)) {
			System.out.println("cs ok");
		} else {
			System.out.println("cs="+css+" expected="+cs);
		}
	}
	

	static final int waveLen = 1000;
	
	private void getWave() {
		double[] chX = new double[waveLen];
		double[] chY = new double[waveLen];
		String answX;
		String answY;
		synchronized (lockObject) {
			answX = port.sendData("q07"+Port.toHex(waveLen & 0xff)+Port.toHex((waveLen >> 8) & 0xff),0);
		}
		if(answX != null && answX.length() == 500*4+4) {
			parseAnsw(answX,500,chX,0,false);
		} else {
			if(answX == null) return;
			System.out.println("wrong length "+answX.length()+" expected "+ (500*4+4));
		}
		synchronized (lockObject) {
			answX = port.sendData("wf401f401",0); // 500 500
		}
		if(answX != null && answX.length() == 500*4+4) {
			parseAnsw(answX,500,chX,500,false);
		} else {
			if(answX == null) return;
			System.out.println("wrong length2 "+answX.length()+" expected "+ (500*4+4));
		}
		
		
		synchronized (lockObject) {
			answY = port.sendData("q06"+Port.toHex(waveLen & 0xff)+Port.toHex((waveLen >> 8) & 0xff),0);
		}
		if(answY != null && answY.length() == 500*4+4) {
			parseAnsw(answY,500,chY,0,true);
		} else {
			if(answY == null) return;
			System.out.println("wrong length3 "+answY.length()+" expected "+ (500*4+4));
		}
		synchronized (lockObject) {
			answY = port.sendData("wf401f401",0); // 500 500
		}
		if(answY != null && answY.length() == 500*4+4) {
			parseAnsw(answY,500,chY,500,true);
		} else {
			if(answY == null) return;
			System.out.println("wrong length4 "+answY.length()+" expected "+ (500*4+4));
		}
	
		Signal sX = new Signal();
		sX.setSignal(chX, chX.length);
		sX.clip(2);
		Signal sY = new Signal();
		sY.setSignal(chY, chY.length);
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
		    writer.write("len:  "+waveLen+"\n");
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

	
	static final double dT = 1.5e-6;
	
	private void printStats() {
		double Pout = 0;
		Signal sX = new Signal(g.getSignal1());
		sX.scale(3.3*2/4096.);
		int maxIndex = sX.getMaxIndex();
		System.out.println("maxIndex = "+maxIndex);
		double Ein = 12.77 * sX.intTrap(0, maxIndex) * dT;
		double Eout = 4.7 * sX.intTrap2(maxIndex,sX.signal.length-maxIndex) * dT;
		double COP = Eout * 100. / Ein;
		COP = (double)(((int)COP*10)/10);
		if(Eout > Ein) {
			Pout = (Eout-Ein)*1000.* (1000./(double)freq.getValue());
			Pout = (double)(((int)Pout*10)/10);
		}
		System.out.println("Ein = "+Ein+" Eout = "+Eout+" COP = "+COP+"%"+ (Pout > 0 ? " Pout = "+Pout+" mW" : ""));
	}
}


