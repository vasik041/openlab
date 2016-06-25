

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Container;
import java.awt.FlowLayout;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.text.DecimalFormat;
import java.text.NumberFormat;
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
	final static String PROPS_PATH = "/com/lz/feex";
	
	static class theLock extends Object {
	}
	static public theLock lockObject = new theLock();
	
	Port port = new Port("dev041","COM7",9600);
//	Preferences prefs = Preferences.systemRoot().node("/com/lz/feex");
	
	Param freq = new Param("Freq:", 1,100, 1, "ms", "freq", 20, this);
	Param tpls = new Param("Tpls:", 1,200,1, "us", "tpls", 1,  this);
	Param up   = new Param("Up:",  10,30, 1, "v",  "up",   10, this);
	Param load = new Param("Load:", 1,50, 1, "%",  "load", 10, this);
	
	ParamView u_in = new ParamView("Uin","v");
	ParamView i_in = new ParamView("Iin","mA");
	ParamView p_in = new ParamView("Pin","mW");
	
	ParamView u_out = new ParamView("Uout","v");
	ParamView i_out = new ParamView("Iout","mA");
	ParamView p_out = new ParamView("Pout","mW");

	ParamView max_i = new ParamView("maxI","mA");
	ParamView cop = new ParamView("COP","%");
	
	Graph g;
	JButton bStart;
	JLabel lInfo;
	
	public void addComponentToPane(Container pane) {
		// controls panel
		JPanel cpanel0 = new JPanel(new GridLayout(4,1));
        cpanel0.add(freq);
        cpanel0.add(tpls);
        cpanel0.add(up);
        cpanel0.add(load);
        cpanel0.setBorder(BorderFactory.createMatteBorder(0,0,0,1,Color.GRAY));

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
        cpanel.add(cpanel0,BorderLayout.CENTER);
        cpanel.add(cpanel1,BorderLayout.SOUTH);

        // measurements panel
        JPanel ipanel = new JPanel(new GridLayout(1,3));
        ipanel.add(u_in);
        ipanel.add(i_in);
        ipanel.add(p_in);
        
        JPanel opanel = new JPanel(new GridLayout(1,3));
        opanel.add(u_out);
        opanel.add(i_out);
        opanel.add(p_out);

        JPanel epanel = new JPanel(new GridLayout(1,1));
        epanel.add(max_i);
        epanel.add(cop);

        JPanel ipanel1 = new JPanel(new FlowLayout(FlowLayout.CENTER));
        lInfo = new JLabel("");
        ipanel1.add(lInfo);
        
        JPanel spanel = new JPanel(new FlowLayout(FlowLayout.RIGHT));
        JCheckBox bAuto = new JCheckBox("Auto");
        bAuto.addItemListener(new ItemListener() {
        	public void itemStateChanged(ItemEvent e) {
        	    //Object source = e.getItemSelectable();
        	    if(e.getStateChange() == ItemEvent.SELECTED) {
        	    	startPolling();
        	    } else {
        	    	stopPolling();
        	    }
        	}
        });
        spanel.add(bAuto);
        
        JButton bRefresh = new JButton("Refresh");
        bRefresh.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				refreshMeas();
			}
        });
        spanel.add(bRefresh);
        
        JPanel mpanel0 = new JPanel(new GridLayout(5,1));
        mpanel0.add(ipanel);
        mpanel0.add(opanel);
        mpanel0.add(epanel);
        mpanel0.add(spanel);
        mpanel0.add(ipanel1);
        mpanel0.setBorder(BorderFactory.createMatteBorder(0,0,0,1,Color.GRAY));
        
        JPanel mpanel = new JPanel(new BorderLayout());
        mpanel.add(mpanel0,BorderLayout.CENTER);
        mpanel.add(spanel,BorderLayout.SOUTH);
        
        //MPP panel
        JPanel powerpanel = new JPanel(new BorderLayout());
        g = new Graph();
        powerpanel.add(g,BorderLayout.CENTER);
        
        bStart = new JButton("MPP");
        bStart.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				startMPP();
			}
        });
        
        JButton bStop = new JButton("Stop");
        bStop.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				stopMPP();
			}
        });

        JPanel bpanel = new JPanel(new FlowLayout(FlowLayout.RIGHT));
        bpanel.add(bStart);
        bpanel.add(bStop);
        powerpanel.add(bpanel,BorderLayout.SOUTH);
        
        // combine all together
        JPanel panel = new JPanel(new GridLayout(1,3));
        panel.add(cpanel);
        panel.add(mpanel);
        panel.add(powerpanel);
        pane.add(panel);
    }

		
	private static void createAndShowGUI() {
        //Create and set up the window.
        JFrame frame = new JFrame("FEEx v. "+VERSION);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        
        //Create and set up the content pane.
        Main p = new Main();
        p.addComponentToPane(frame.getContentPane());

        frame.addWindowListener(new java.awt.event.WindowAdapter() {
            @Override
            public void windowClosing(java.awt.event.WindowEvent windowEvent) {
            	stopPolling();
            	stopMPP();
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
		if(param.equals("freq")) {
			val *= 1000000; //ms -> ns
			int a = val & 0xff;
			int b = (val >> 8) & 0xff;
			int c = (val >> 16) & 0xff;
			int d = (val >> 24) & 0xff;
			int cs = (a ^ b ^ c ^ d) & 0xff;
			port.sendData("f"+Port.toHex(a)+Port.toHex(b)+Port.toHex(c)+Port.toHex(d),cs);
		} else if(param.equals("tpls")) {
			val *= 1000;	// us -> ns
			int a = val & 0xff;
			int b = (val >> 8) & 0xff;
			int c = (val >> 16) & 0xff;
			int d = (val >> 24) & 0xff;
			int cs = (a ^ b ^ c ^ d) & 0xff;
			port.sendData("t"+Port.toHex(a)+Port.toHex(b)+Port.toHex(c)+Port.toHex(d),cs);
		} else if(param.equals("up")) {
			val = Utils.convetPSU(val);
			int a = val & 0xff;
			int cs = (a ^ 0x55) & 0xff;
			port.sendData("u"+Port.toHex(a),cs);
		} else if(param.equals("load")) {
			val = Utils.convetLoad(val);
			int a = val & 0xff;
			int cs = (a ^ 0x55) & 0xff;
			port.sendData("l"+Port.toHex(a),cs);
		}
	}


	private void refreshMeas() {
		synchronized (lockObject) {
			refreshMeasImpl();
		}
	}

	
	double pin = 0;
	double pout = 0;
	
   	private void refreshMeasImpl() {
		NumberFormat formatter = new DecimalFormat("#0.000");
		String answ = port.sendData("p",0);
		if(answ != null) {
			int v = Integer.parseInt(answ.substring(1, 1+4),16);
			int i = Integer.parseInt(answ.substring(1+4, 1+4+4),16);
			double dv = Utils.convetLoadU(v);
			double di = Utils.convetLoadI(i);
			u_out.setValue(""+formatter.format(dv));
			i_out.setValue(""+formatter.format(di));
			p_out.setValue(""+formatter.format(dv*di));
			pout = dv*di;
		}
		answ = port.sendData("e",0);
		if(answ != null) {
			int v = Integer.parseInt(answ.substring(1, 1+4),16);
			int i = Integer.parseInt(answ.substring(1+4, 1+4+4),16);
			double dv = Utils.convetPsuU(v);
			double di = Utils.convetPsuI(i,dv);
			u_in.setValue(""+formatter.format(dv));
			i_in.setValue(""+formatter.format(di));
			p_in.setValue(""+formatter.format(dv*di));
			pin = dv*di;
			double maxi = di * (double)freq.getValue() * 2000. / (double)tpls.getValue();
			max_i.setValue(""+formatter.format(maxi));
		}
		if(pin > 0) {
			cop.setValue(""+formatter.format(100*pout/pin));
		} else {
			cop.setValue("0");
		}
	}
	
	
   	static PollingThread t;
   	
	private void startPolling() {
		synchronized (lockObject) {
			if(t == null) {
				t = new PollingThread();
				t.start();
			}
		}
	}

	
	private static void stopPolling() {
		synchronized (lockObject) {
			if(t != null) {
				t.bDone = true;
				try {
					t.join();
				}
				catch (InterruptedException e) {
		        }
				t = null;
			}
		}
	}

	
	class PollingThread extends Thread {
		boolean bDone = false;
	    
		public void run() {
	    	while(!bDone) {
	    		refreshMeas();
	        	try {
	        		for(int i=0; i < 30 && !bDone; i++)
	        			Thread.sleep(100);
	        	}
	        	catch (InterruptedException e) {
	        		break;
	        	}
	    	}
		}
    }

	private void syncSettings() {
		String answ = port.sendData("s",0);
		if(answ != null && answ.startsWith("z")) {
			int fq = Integer.parseInt(answ.substring(1, 1+8),16);
			int tau = Integer.parseInt(answ.substring(1+8, 1+8+8),16);
			int psu = Integer.parseInt(answ.substring(1+8+8, 1+8+8+2),16);
			int l = Integer.parseInt(answ.substring(1+8+8+2, 1+8+8+2+2),16);
			fq /= 1000000;
			tau /= 1000;
//			System.out.println("freq = "+fq);
//			System.out.println("tau = "+tau);
//			System.out.println("psu = "+psu);
//			System.out.println("load = "+load);
			freq.setValue(fq);
			tpls.setValue(tau);
			up.setValue(Utils.convetPSUback(psu));
			load.setValue(Utils.convetLoadback(l));
		}
	}

//=============================================================================
	
	static MppThread mppt;
	
	class MppThread extends Thread {
		static final int MAX_POINTS = 30;
		static final int N_MEAS = 20;
		static final int MEAS_DELAY = 5;	// seconds
		boolean bDone = false;
		
		public void run() {
			int percent = 1;
			double maxCOP = 0;
			Signal si = new Signal();
			Signal so = new Signal();
			Random rand = new Random();
			double pi[] = new double[MAX_POINTS];
			double po[] = new double[MAX_POINTS];
	    	while(!bDone && percent < MAX_POINTS) {
	    		bStart.setText(""+percent+"%");
	    		syncHW("load",percent);
	        	try {
	        		for(int i=0; i < MEAS_DELAY*10 && !bDone; i++)
	        			Thread.sleep(100);
	        	}
	        	catch (InterruptedException e) {
	        		break;
	        	}
	        	if(bDone) break;
	        	
	        	double temp_pout = 0;
	        	double temp_pin = 0;
	        	for(int i=0; i < N_MEAS && !bDone; i++) {
	        		refreshMeas();
	        		temp_pin += pin;
	        		temp_pout += pout;
		        	try {
		        		for(int j=0; j < rand.nextInt(100) && !bDone; j++)
		        			Thread.sleep(1);
		        	}
		        	catch (InterruptedException e) {
		        		break;
		        	}
	        	}
	        	temp_pin /= (double)N_MEAS;
	        	temp_pout /= (double)N_MEAS;
	        	if(bDone) break;
	        	
	        	double cop = 0;
	        	if(temp_pin != 0) {
	        		cop = 100 * temp_pout / temp_pin; 
	        	}
	        	if(cop > maxCOP) maxCOP = cop;
	        	System.out.println("pin("+percent+") = "+temp_pin);
	        	System.out.println("pout("+percent+") = "+temp_pout);
	        	System.out.println("COP("+percent+") = "+cop);

	        	pi[percent-1] = temp_pin;
	        	po[percent-1] = temp_pout;
	        	si.setSignal(pi,percent);
	        	so.setSignal(po,percent);
	        	g.setSignal(si,so);
	        	g.updateUI();
        	
	        	NumberFormat formatter = new DecimalFormat("#0.00");
	        	lInfo.setText("maxCOP = "+formatter.format(maxCOP)+" %");
	        	lInfo.updateUI();
	        	if(percent > 10 && po[percent-1] < 0.001) break;
	        	percent++;
	    	}
	    	bStart.setText("MPP");
		}
    }
	
	public void startMPP() {
		synchronized (lockObject) {
			if(t == null) {
				mppt = new MppThread();
				mppt.start();
			}
		}
	}
	
	public static void stopMPP() {
		synchronized (lockObject) {
			if(mppt != null) {
				mppt.bDone = true;
				try {
					mppt.join();
				}
				catch (InterruptedException e) {
		        }
				mppt = null;
			}
		}
	}
}


