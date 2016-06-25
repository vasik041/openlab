package sgen;

import java.awt.BorderLayout;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.util.Arrays;
import java.util.prefs.Preferences;

import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.JTextField;
import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import sgen.Signal.Shape;

public class Main implements ChangeListener, ActionListener {
	final static String VERSION = "0.2";
	final static int FREQ_MIN = 10;
	final static int FREQ_MAX = 550;
	final static int AMP_MIN = -2047;
	final static int AMP_MAX = 2047;
	
	Port port = new Port("dev041","COM7",9600);
	Signal sig = new Signal();
	
	Preferences prefs = Preferences.systemRoot().node("/com/lz/sgen");
	
	JSlider sliderTau;
	int nTau = prefs.getInt("tau",128);
	
	JSlider sliderMinA;
	JTextField tfMinA;
	int nMinA = prefs.getInt("minA",-100);
	
	JSlider sliderMaxA;
	JTextField tfMaxA;
	int nMaxA = prefs.getInt("maxA",100);
	
	Graph g;
	
	JTextField tfTau;
	JTextField tfFreq;
	int freq = prefs.getInt("freq",100);

	JButton bInc;
	JButton bDec;
	JButton bSet;

	JComboBox<String> shapesList;
    String[] shapeNames = { "Tre", "Exp", "Sin", "Sin2" };
    Shape[] shapes = { Shape.TREA, Shape.EXP, Shape.SIN, Shape.SIN2 };
    
    JComboBox<String> attList;
    String[] attNames = { "x1","/2","/5","/10" };
    double[] attVals = { 1., 0.5, 0.2, 0.1 };
    double atten = attVals[prefs.getInt("attenIx", 0)];
    
    
	public void addComponentToPane(Container pane) {

		sliderTau = new JSlider(JSlider.HORIZONTAL, 1, 254, nTau);
		sliderTau.addChangeListener(this);
		sliderTau.setMajorTickSpacing(10);
		sliderTau.setPaintTicks(true);

		sliderMinA = new JSlider(JSlider.VERTICAL, -127, 127, nMinA);
		sliderMinA.addChangeListener(this);
		sliderMinA.setMajorTickSpacing(10);
		sliderMinA.setPaintTicks(true);

		sliderMaxA = new JSlider(JSlider.VERTICAL, -127, 127, nMaxA);
		sliderMaxA.addChangeListener(this);
		sliderMaxA.setMajorTickSpacing(10);
		sliderMaxA.setPaintTicks(true);

        sig.setTau(nTau);
        sig.setMinA(nMinA);
        sig.setMaxA(nMaxA);
        sig.setShape(shapes[prefs.getInt("shapesIx", 0)]);
        
		g = new Graph(sig);
		
        JPanel p = new JPanel();
        BoxLayout boxL = new BoxLayout(p,BoxLayout.X_AXIS);
        p.setLayout(boxL);
        JLabel l1 = new JLabel("");
        l1.setPreferredSize(new Dimension(25, 10));
        p.add(l1);
        p.add(sliderTau);
        JLabel l2 = new JLabel("");
        l2.setPreferredSize(new Dimension(25, 10));
        p.add(l2);
        
        JPanel p2 = new JPanel(new BorderLayout());
        p2.add(sliderMinA,BorderLayout.WEST);
        p2.add(p,BorderLayout.PAGE_START);
        p2.add(g,BorderLayout.CENTER);
        p2.add(sliderMaxA,BorderLayout.EAST);
                
        shapesList = new JComboBox(shapeNames);
        shapesList.addActionListener(this);
        shapesList.setSelectedItem(shapeNames[prefs.getInt("shapesIx", 0)]);
        tfMinA = new JTextField(4);
        tfMinA.setText(""+nMinA);
        tfMaxA = new JTextField(4);
        tfMaxA.setText(""+nMaxA);
        tfTau = new JTextField(4);
        tfTau.setText(""+nTau);
        
        JPanel downPanel = new JPanel(new FlowLayout());
        downPanel.add(new JLabel("Amin:"));
        downPanel.add(tfMinA);
        
        downPanel.add(new JLabel("Amax:"));
        downPanel.add(tfMaxA);
        
        downPanel.add(new JLabel("Att:"));
        attList = new JComboBox(attNames);
        attList.addActionListener(this);
        attList.setSelectedItem(attNames[prefs.getInt("attenIx", 0)]);
        downPanel.add(attList);
        
        downPanel.add(new JLabel("Tau:"));
        downPanel.add(tfTau);
        
        downPanel.add(new JLabel("Shape:"));
        downPanel.add(shapesList);

        downPanel.add(new JLabel("Freq:"));
        tfFreq = new JTextField(4);
        tfFreq.setText(""+freq);
        downPanel.add(tfFreq);
        
        bInc = new JButton("+");
        bInc.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				if(freq < FREQ_MAX) {
					freq++;
					tfFreq.setText(""+freq);
					prefs.putInt("freq", freq);
					syncHW();
				}
			}
        });
        downPanel.add(bInc);
        
        bDec = new JButton("-");
        bDec.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				if(freq > FREQ_MIN) {
					freq--;
					tfFreq.setText(""+freq);
					prefs.putInt("freq", freq);
					syncHW();
				}
			}
        });
        downPanel.add(bDec);
        
        bSet = new JButton("Set");
        bSet.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				freq = Integer.parseInt(tfFreq.getText());
				if(freq < FREQ_MIN) {
					freq = FREQ_MIN;
					tfFreq.setText(""+freq);
					prefs.putInt("freq", freq);
				}
				if(freq > FREQ_MAX) {
					freq = FREQ_MAX;
					tfFreq.setText(""+freq);
					prefs.putInt("freq", freq);
				}
				
				int m = Integer.parseInt(tfMinA.getText());
				if(m != nMinA) {
					if(m < -127) m = -127;
					if(m > 127) m = 127;
					sliderMinA.setValue(m);
					prefs.putInt("minA", m);
				}

				m = Integer.parseInt(tfMaxA.getText());
				if(m != nMaxA) {
					if(m < -127) m = -127;
					if(m > 127) m = 127;
					sliderMaxA.setValue(m);
					prefs.putInt("maxA", m);
				}
				
				m = Integer.parseInt(tfTau.getText());
				if(m != nTau) {
					if(m < 1) m = 1;
					if(m > 254) m = 254;
					sliderTau.setValue(m);
					prefs.putInt("tau", m);
				}
				
				syncHW();
			}
        });
        downPanel.add(bSet);
        
        p2.add(downPanel,BorderLayout.SOUTH);
        
        pane.add(p2);
    }

	
	// sliders
	@Override
	public void stateChanged(ChangeEvent e) {
		JSlider source = (JSlider)e.getSource();
	    if (!source.getValueIsAdjusting()) {
	        int v = (int)source.getValue();
	        if(source.equals(sliderTau)) {
	        	sig.setTau(v);
	        	tfTau.setText(""+v);
	        	nTau = v;
	        	prefs.putInt("tau", v);
	        }
	        else if(source.equals(sliderMinA)) {
	        	sig.setMinA(v);
	        	tfMinA.setText(""+v);
	        	nMinA = v;
	        	prefs.putInt("minA", v);
	        }
	        else if(source.equals(sliderMaxA)) {
	        	sig.setMaxA(v);
	        	tfMaxA.setText(""+v);
	        	nMaxA = v;
	        	prefs.putInt("maxA", v);
	        }
	        g.updateUI();
	    }
	}


	// shapes and scales combo boxes
	@Override
	public void actionPerformed(ActionEvent e) {
		JComboBox cb = (JComboBox)e.getSource();
		if(cb.equals(shapesList)) {
			int ix = cb.getSelectedIndex();
			sig.setShape(shapes[ix]);
			prefs.putInt("shapesIx", ix);
			g.updateUI();
		} else if(cb.equals(attList)) {
			int ix = cb.getSelectedIndex();
			atten = attVals[ix];
			prefs.putInt("attenIx", ix);
		}
	}

	
	private static void createAndShowGUI() {
        //Create and set up the window.
        JFrame frame = new JFrame("Sample Gen v. "+VERSION);
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


	
	
	double[] signal0;
	int freq0;
	double att0;
	
	private void syncHW() {
		double[] signal = sig.getSignal();
		
		if(signal0 == null || !Arrays.equals(signal0,signal) || att0 != atten) {
			StringBuffer sb = new StringBuffer();
			sb.append("s");
			int cs = 0;
			for(int i=0; i < signal.length; i++) {
				int s = (int)(signal[i] * AMP_MAX / 128. * atten);
				int sl = s & 0xff;
				int sh = (s >> 8) & 0xff;
				sb.append(Port.toHex(sh));
				sb.append(Port.toHex(sl));
				cs ^= sl;
				cs ^= sh;
			}
			port.sendData(sb.toString(),cs & 0xff);
			signal0 = Arrays.copyOf(signal, signal.length);
			att0 = atten;
		}
		
		if(freq != freq0) {
			int h = freq/256;
			int l = freq%256;
			int cs = (l ^ h) & 0xff;
			port.sendData("t"+Port.toHex(h)+Port.toHex(l),cs);
			freq0 = freq;
		}
	}

}
