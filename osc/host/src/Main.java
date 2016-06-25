

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Container;
import java.awt.FlowLayout;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;
import java.util.prefs.Preferences;

import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;
import javax.swing.event.ChangeListener;
import javax.swing.filechooser.FileNameExtensionFilter;


public class Main implements ActionListener {
	static final String VERSION = "0.2";
	static JFrame frame;
	static Preferences prefs = Preferences.systemRoot().node("/com/lz/dso");
	Port port = new Port("dev041","COM2",115200);

	Signal sigA = new Signal();	// ch1, current 
	Signal sigB = new Signal();	// ch2, voltage
	
	Graph gA;
	Graph gB;

	Signal sigU = new Signal();
	Signal sigY = new Signal();
	
	Signal sigD = new Signal();
	Signal sigY2 = new Signal();
	Graph gXY;
	
	String[] gains = { "/2","x1","x2","x5", "x10","x20","x50","x100" };
	JComboBox cbGainA;
	JComboBox cbGainB;
	int nGainA = 0;
	int nGainB = 0;
	
	String[] divs = { "640K","320K","160K","80K", "40K","20K","10K" };
	int [] nDivs = { 1,2,3,4, 5,6,7 };
	int nDiv = nDivs[0];
	JComboBox cbDiv;
	
	JComboBox cbAdcMode;
	JButton bGet;
	
	Info info;
    		
	public void addComponentToPane(Container pane) {

		gA = new Graph(sigA,"1",Color.BLUE);
		gB = new Graph(sigB,"2",Color.CYAN);
		gXY = new Graph(sigU,sigY,sigD,sigY2);
		
        JPanel p = new JPanel(new BorderLayout());
        JPanel pG = new JPanel(new GridLayout(1,3));
        pG.add(gA);
        pG.add(gB);
        pG.add(gXY);
        p.add(pG,BorderLayout.CENTER);
        info = new Info(sigA,sigB,this);
        p.add(info,BorderLayout.EAST);
                
        JPanel downPanel = new JPanel(new FlowLayout());

        cbGainA = new JComboBox(gains);
        cbGainA.addActionListener(this);
        cbGainA.setSelectedItem(gains[prefs.getInt("gainAix", 0)]);
        downPanel.add(new JLabel("Gain 1:"));
        downPanel.add(cbGainA);

        cbGainB = new JComboBox(gains);
        cbGainB.addActionListener(this);
        cbGainB.setSelectedItem(gains[prefs.getInt("gainBix", 0)]);
        downPanel.add(new JLabel("Gain 2:"));
        downPanel.add(cbGainB);
        
        cbDiv = new JComboBox(divs);
        cbDiv.addActionListener(this);
        cbDiv.setSelectedItem(divs[prefs.getInt("divix", 0)]);
        downPanel.add(new JLabel("Sps:"));
        downPanel.add(cbDiv);

        downPanel.add(new JLabel("ADC Mode:"));
        cbAdcMode = new JComboBox(new String[] { "1212", "1122" } );
        cbAdcMode.addActionListener(this);
        cbAdcMode.setSelectedIndex(prefs.getInt("adcmodeix", 0));
        downPanel.add(cbAdcMode);

        bGet = new JButton("Get");
        bGet.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				syncHW();
			}
        });
        downPanel.add(bGet);

        p.add(downPanel,BorderLayout.SOUTH);
        pane.add(p);
    }

	
	// shapes combobox
	@Override
	public void actionPerformed(ActionEvent e) {
		JComboBox cb = (JComboBox)e.getSource();
		int ix = cb.getSelectedIndex();
		if(cb.equals(cbDiv)) {
			nDiv = nDivs[ix];
			prefs.putInt("divix", ix);
		} else if(cb.equals(cbGainA)) {
			nGainA = ix;
			prefs.putInt("gainAix", ix);
		} else if(cb.equals(cbGainB)) {
			nGainB = ix;
			prefs.putInt("gainBix", ix);
		} else if(cb.equals(cbAdcMode)) {
			prefs.putInt("adcmodeix", ix);
		}
	}

	private static void createAndShowGUI() {
        //Create and set up the window.
        frame = new JFrame("DSO v. "+VERSION);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        
        //Create and set up the content pane.
        Main p = new Main();
        p.addComponentToPane(frame.getContentPane());

        frame.addWindowListener(new java.awt.event.WindowAdapter() {
            @Override
            public void windowClosing(java.awt.event.WindowEvent windowEvent) {
                p.port.close();
            }
        });
        
        //Display the window.
        frame.pack();
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


	private void parseAnsw(String answ,int len,double[] sig) {
		int cs = 0;
		for(int i=0; i < len; i++) {
			String sA = answ.substring(i*8, i*8+4);
			String sB = answ.substring(i*8+4, i*8+4+4);
			int a = Integer.parseInt(sA,16);
			cs ^= a;
			int b = Integer.parseInt(sB,16);
			cs ^= b;
			sig[2*i] = 512.-a;
			sig[2*i+1] = 512.-b;
		}
		cs ^= 0x1234;
		String css = answ.substring(len*8, len*8+4);
		if(cs == Integer.parseInt(css,16)) {
			System.out.println("cs ok");
		} else {
			System.out.println("cs="+css+" expected="+cs);
		}
	}
	
	
	
	private void syncHW() {
		int nMode = cbAdcMode.getSelectedIndex();
		System.out.println("div="+nDiv+" gainA="+nGainA+" gainB="+nGainB+" mode="+nMode);

		if(nMode == 0) {
			double[] chAB = new double[Signal.SIG_LEN];
			double[] chA = new double[Signal.SIG_LEN/2];
			double[] chB = new double[Signal.SIG_LEN/2];
			String answ = port.sendData("g"+Port.toHex(nDiv)+Port.toHex(16*nGainB+2*nGainA));
			parseAnsw(answ,Signal.SIG_LEN/2,chAB);
			for(int i=0; i < Signal.SIG_LEN/2; i++) {
				chA[i] = chAB[2*i];
				chB[i] = chAB[2*i+1];
			}
			sigA.setSignal(chA);
			sigB.setSignal(chB);
		} else {
			double[] chA = new double[Signal.SIG_LEN];
			double[] chB = new double[Signal.SIG_LEN];
			String answ1 = port.sendData("q"+Port.toHex(nDiv)+Port.toHex(16*nGainB+2*nGainA)+Port.toHex(0));
			String answ2 = port.sendData("q"+Port.toHex(nDiv)+Port.toHex(16*nGainB+2*nGainA)+Port.toHex(1));
			parseAnsw(answ1,Signal.SIG_LEN/2,chA);
			parseAnsw(answ2,Signal.SIG_LEN/2,chB);
			sigA.setSignal(chA);
			sigB.setSignal(chB);
		}
		refresh();
	}

	
	public void refresh() {
		trace();
		gA.updateUI();
		gB.updateUI();
		gXY.updateUI();
		info.updateValues();
	}

	private double getDeltaT() {
		String div = divs[nDiv].substring(0,divs[nDiv].length()-1);
		double dT = 1./Double.parseDouble(div);
		return dT;
	}
	
	// calculate integral sY(sX)
	private double [] integ(double[] sX,double[] sY,double meanY) {
		double dT = getDeltaT();
		double[] sI = new double[sY.length];
		for(int i=0; i < sY.length-1; i++) {
			double dX = sX[i+1]-sX[i];
			sI[i] = (sY[i]+sY[i+1]-2*meanY)*dT/2.;
		}
		sI[sY.length-1] = (sY[sY.length-1]-meanY)*dT; //(sX[sY.length-1]-sX[sY.length-2]);
		
		double[] sI2 = new double[sY.length];
		sI2[0] = sI[0];
		for(int i=1; i < sY.length; i++) {
			sI2[i] = sI2[i-1] + sI[i];
		}
		return sI2;
	}
	
	
	// calculate BH curve
	private void trace() {
		double meanB = sigB.getMean();
		if(!sigA.getSync(sigU,sigD)) return;
				
		double [] sB = integ(sigU.getSignal(),
							sigB.getSignal(sigU.getStart(),sigU.getLength()),
							meanB);
		double [] sB2 = integ(sigD.getSignal(),
							sigB.getSignal(sigD.getStart(),sigD.getLength()),
							meanB);
		sigY.setSignal(sB);
		sigY2.setSignal(sB2);
	
		// cheating
		if(false) {
			double k = sigY.getPkPk() / sigY2.getPkPk();
			double k2 = (double)sigY2.getLength() / (double)sigY.getLength();
			System.out.println("k="+k+" k2="+k2);
			for(int i=0; i < sB2.length; i++)
				sB2[i] *= k;
			sigY2.setSignal(sB2);
		}
		
//		Signal sigI = new Signal();
//		sigI.setSignal(sigB.getSignal(sigU.getStart(),sigU.getLength()));
//		System.out.println("sigI="+sigI.toString());
//		sigI.setSignal(sigB.getSignal(sigD.getStart(),sigD.getLength()));
//		System.out.println("sigI2="+sigI.toString());
//		System.out.println("sigY="+sigY.toString());
//		System.out.println("sigY2="+sigY2.toString());
				
		sigA.markers.clear();
		sigB.markers.clear();
		sigA.addMarker(sigU.getStart());
		sigA.addMarker(sigU.getStart()+sigU.getLength());
		sigA.addMarker(sigD.getStart()+sigD.getLength());
		sigB.addMarker(sigU.getStart());
		sigB.addMarker(sigU.getStart()+sigU.getLength());
		sigB.addMarker(sigD.getStart()+sigD.getLength());

	}
}
