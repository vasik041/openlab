

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
import java.util.concurrent.ConcurrentLinkedDeque;

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
	final static String PROPS_PATH = "/com/lz/feex2";
	
	static class theLock extends Object { }
	static public theLock lockObject = new theLock();
	
	Comm d = new Comm(this);
	
	Param freq = new Param("Freq:", 1,100, 1, "ms", "freq", 20, this);
	Param tpls = new Param("Tpls:", 1,200, 1, "us", "tpls", 1,  this);
	Param ups  = new Param("Ups:",  10,35, 1, "v",  "ups",  10, this);
	Param load = new Param("Load:",  1,50, 1, "%",  "load", 10, this);
	
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
        cpanel0.add(ups);
        cpanel0.add(load);
        cpanel0.setBorder(BorderFactory.createMatteBorder(0,0,0,1,Color.GRAY));

        JPanel cpanel = new JPanel(new BorderLayout());
        cpanel.add(cpanel0,BorderLayout.CENTER);

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
        
        JPanel mpanel0 = new JPanel(new GridLayout(4,1));
        mpanel0.add(ipanel);
        mpanel0.add(opanel);
        mpanel0.add(epanel);
        mpanel0.add(ipanel1);
        mpanel0.setBorder(BorderFactory.createMatteBorder(0,0,0,1,Color.GRAY));
        
        JPanel mpanel = new JPanel(new BorderLayout());
        mpanel.add(mpanel0,BorderLayout.CENTER);
        
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
        JFrame frame = new JFrame("FE Ex v. "+VERSION);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        
        //Create and set up the content pane.
        Main p = new Main();
        p.addComponentToPane(frame.getContentPane());

        frame.addWindowListener(new java.awt.event.WindowAdapter() {
            @Override
            public void windowClosing(java.awt.event.WindowEvent windowEvent) {
            	stopMPP();
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
			if(param.equals("freq")) {
				d.setFreq(val);
			} else if(param.equals("tpls")) {
				d.setTpls(val);
			} else if(param.equals("ups")) {
				d.setPSU(val);
			} else if(param.equals("load")) {
				d.setLoad(val);
			}
		}
	}
	
	
	class PowerMeas {
   		double Pin;
   		double Pout;
   		
   		public PowerMeas(double Pin,double Pout) {
   			this.Pin = Pin;
   			this.Pout = Pout;
   		}
	}
	
	ConcurrentLinkedDeque<PowerMeas> pmq = new ConcurrentLinkedDeque<PowerMeas>();
	
	
   	public void refreshMeasurements(int Ui,int Ii,int Uo,int Io) {
   		double pin = 0;
   		double pout = 0;
		NumberFormat formatter = new DecimalFormat("#0.000");

		double dv = Utils.convetLoadU(Uo);
		double di = Utils.convetLoadI(Io);
		u_out.setValue(""+formatter.format(dv));
		i_out.setValue(""+formatter.format(di));
		p_out.setValue(""+formatter.format(dv*di));
		pout = dv*di;

		dv = Utils.convetPsuU(Ui);
		di = Utils.convetPsuI(Ii,dv);
		u_in.setValue(""+formatter.format(dv));
		i_in.setValue(""+formatter.format(di));
		p_in.setValue(""+formatter.format(dv*di));
		pin = dv*di;
		double maxi = di * (double)freq.getValue() * 2000. / (double)tpls.getValue();
		max_i.setValue(""+formatter.format(maxi));

		if(pin > 0) {
			cop.setValue(""+formatter.format(100*pout/pin));
		} else {
			cop.setValue("0");
		}
		
		pmq.addLast(new PowerMeas(pin,pout));
		if(pmq.size() > 100)
			pmq.removeFirst();
	}
	
   	
   	public void refreshSettings(int fq,int pulse,int psu,int load1)
   	{
   		freq.setValue(fq);
   		tpls.setValue(pulse);
   		ups.setValue(psu);
   		load.setValue(load1);
   	}
   	
//=============================================================================
	
	static MppThread mppt;
	
	class MppThread extends Thread {
		static final int MAX_POINTS = 30;
		static final int N_MEAS = 20;
		static final int MEAS_DELAY = 5;	// delay before start recording measurements, seconds
		boolean bDone = false;
    	NumberFormat formatter = new DecimalFormat("#0.00");
		
		public void run() {
			int point = 0;
			double maxCOP = 0;
			Signal si = new Signal();
			Signal so = new Signal();

			double pi[] = new double[MAX_POINTS];
			double po[] = new double[MAX_POINTS];

			while(!bDone && point < MAX_POINTS) {
				int percent = point+1;
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

	        	// wait for N_MEAS measurements
	        	pmq.removeAll(pmq);
	        	while(pmq.size() < N_MEAS) {
	        		try {
	        			Thread.sleep(100);
	        		}
		        	catch (InterruptedException e) {
		        		break;
		        	}
	        		if(bDone) break;
	        	}
	        	if(bDone) break;
	        	
	        	PowerMeas[] pm = pmq.toArray(new PowerMeas[pmq.size()]);
	        	double temp_pout = 0;
	        	double temp_pin = 0;
	        	for(int i=0; i < pm.length; i++) {
	        		temp_pin += pm[i].Pin;
	        		temp_pout += pm[i].Pout;
	        	}
	        	temp_pin /= (double)pm.length;
	        	temp_pout /= (double)pm.length;
	        	
	        	double cop = 0;
	        	if(temp_pin != 0) {
	        		cop = 100 * temp_pout / temp_pin; 
	        	}
	        	if(cop > maxCOP) maxCOP = cop;
	        	System.out.print("Pin("+percent+"%) = "+formatter.format(temp_pin));
	        	System.out.print(" Pout("+percent+"%) = "+formatter.format(temp_pout));
	        	System.out.println(" COP("+percent+"%) = "+formatter.format(cop));

	        	pi[point] = temp_pin;
	        	po[point] = temp_pout;
	        	si.setSignal(pi,point+1);
	        	so.setSignal(po,point+1);
	        	g.setSignal(si,so);
	        	g.updateUI();
        	
	        	lInfo.setText("maxCOP = "+formatter.format(maxCOP)+" %");
	        	lInfo.updateUI();
	        	if(point > 10 && po[point] < 0.001) break;
	        	point++;
	        	if(bDone) break;
	    	}
	    	bStart.setText("MPP");
		}
    }

	
	public void startMPP() {
		synchronized (lockObject) {
			mppt = new MppThread();
			mppt.start();
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


