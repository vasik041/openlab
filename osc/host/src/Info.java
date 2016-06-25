import java.awt.Color;
import java.awt.FlowLayout;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;

import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JFileChooser;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.filechooser.FileNameExtensionFilter;


public class Info extends JPanel {
	Signal sig1;
	Signal sig2;
	Main frame;

	JButton bSmoothA;
	JButton bSmoothB;
	JButton bSave;
	JButton bLoad;
	int nSaveId = 0;

	JLabel l11;
	JLabel l12;
	JLabel l13;
	JLabel l14;
	
	JLabel l21;
	JLabel l22;
	JLabel l23;
	JLabel l24;
	
	public Info(Signal s1,Signal s2,Main p) {
		super();
		sig1 = s1;
		sig2 = s2;
		frame = p;
	
		setLayout(new GridLayout(3,1));
		setBorder(BorderFactory.createLineBorder(Color.WHITE)); 
		JPanel p1 = new JPanel();
		p1.setLayout(new BoxLayout(p1, BoxLayout.Y_AXIS));
		p1.setBorder(BorderFactory.createEmptyBorder(10,10,10,10));
		l11 = new JLabel("Max: "+String.format("%4.3f",sig1.getMax()));
		l12 = new JLabel("Min: "+String.format("%4.3f",sig1.getMin()));
		l13 = new JLabel("Pk-Pk: "+String.format("%4.3f",sig1.getPkPk()));
		l14 = new JLabel("Mean: "+String.format("%4.3f",sig1.getMean()));
		p1.add(l11);
		p1.add(l12);
		p1.add(l13);
		p1.add(l14);

		JPanel p2 = new JPanel();
		p2.setLayout(new BoxLayout(p2, BoxLayout.Y_AXIS));
		p2.setBorder(BorderFactory.createEmptyBorder(10,10,10,10));
		l21 = new JLabel("Max: "+String.format("%4.3f",sig2.getMax()));
		l22 = new JLabel("Min: "+String.format("%4.3f",sig2.getMin()));
		l23 = new JLabel("Pk-Pk: "+String.format("%4.3f",sig2.getPkPk()));
		l24 = new JLabel("Mean: "+String.format("%4.3f",sig2.getMean()));
		p2.add(l21);
		p2.add(l22);
		p2.add(l23);
		p2.add(l24);
		
		JPanel p3 = new JPanel();
		p3.setLayout(new BoxLayout(p3, BoxLayout.Y_AXIS));
		p3.setBorder(BorderFactory.createEmptyBorder(10,10,10,10));

		bSmoothA = new JButton("Smooth A");
		bSmoothA.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				smoothData(true);
			}
        });
        p3.add(bSmoothA);

        bSmoothB = new JButton("Smooth B");
		bSmoothB.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				smoothData(false);
			}
        });
        p3.add(bSmoothB);
		
        bLoad = new JButton("Load");
        bLoad.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				loadData();
			}
        });
        p3.add(bLoad);
        
        bSave = new JButton("Save");
        bSave.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				saveData();
				bSave.setText("Save "+nSaveId);
			}
        });
        p3.add(bSave);
		
		add(p1);
		add(p2);
		add(p3);
	}
	
	
	public void updateValues() {
		l11.setText("Max: "+String.format("%4.3f",sig1.getMax()));
		l12.setText("Min: "+String.format("%4.3f",sig1.getMin()));
		l13.setText("Pk-Pk: "+String.format("%4.3f",sig1.getPkPk()));
		l14.setText("Mean: "+String.format("%4.3f",sig1.getMean()));
		
		l21.setText("Max: "+String.format("%4.3f",sig2.getMax()));
		l22.setText("Min: "+String.format("%4.3f",sig2.getMin()));
		l23.setText("Pk-Pk: "+String.format("%4.3f",sig2.getPkPk()));
		l24.setText("Mean: "+String.format("%4.3f",sig2.getMean()));
	}
	
	private void saveData() {
		PrintWriter writer;
		try {
			writer = new PrintWriter("d:\\0\\dso"+nSaveId+".txt", "UTF-8");
			writer.println(sig1.toString());
			writer.println(sig2.toString());
			writer.close();
		} catch (FileNotFoundException | UnsupportedEncodingException e) {
			e.printStackTrace();
		}
		nSaveId++;
	}
	
	private void loadData() {
		JFileChooser chooser = new JFileChooser();
	    FileNameExtensionFilter filter = new FileNameExtensionFilter("DSO data files", "txt");
	    chooser.setFileFilter(filter);
	    byte[] cd= Main.prefs.getByteArray("curdir", null);
	    if(cd != null) {
	    	String dir = new String(cd);
	    	System.out.println("selected folder: "+dir);
	    	chooser.setCurrentDirectory(new File(dir));
	    }
	    int retVal = chooser.showOpenDialog(this);
	    if(retVal == JFileChooser.APPROVE_OPTION) {
	    	String dir = chooser.getSelectedFile().getParent();
	    	Main.prefs.putByteArray("curdir", dir.getBytes());
	    	System.out.println("loading " + chooser.getSelectedFile().getPath());
	    	BufferedReader br = null;
	    	try {
	    		br = new BufferedReader(new FileReader(chooser.getSelectedFile()));
	    		String line1 = br.readLine();
	    		String line2 = br.readLine();
	    		String[] vals1 = line1.split(",");
	    		String[] vals2 = line2.split(",");

	    		double[] chA = new double[vals1.length];
	    		double[] chB = new double[vals2.length];
	    		for(int i=0; i < vals1.length; i++)
	    			chA[i] = Double.parseDouble(vals1[i]);
	    		for(int i=0; i < vals2.length; i++)
	    			chB[i] = Double.parseDouble(vals2[i]);
		    	System.out.println("lenA="+vals1.length+" lenB="+vals2.length);
	    		sig1.setSignal(chA);
				sig2.setSignal(chB);
				frame.refresh();
	    	} catch (FileNotFoundException e) {
	    		e.printStackTrace();
	    	} catch (IOException e) {
	    		e.printStackTrace();
	    	}
   			try {
   				br.close();
   			} catch (IOException e) {
   				e.printStackTrace();
   			}
	    }
	}
	

	private double[] smooth(double[] s) {
		double beta = 1-0.3;
		double[] y = new double[s.length];
		y[0] = s[0];
		for(int i=1; i < s.length; i++) {
			y[i] = beta * s[i] + (1.-beta) * y[i-1];
		}
		return y;
	}
	
	private double[] smooth1(double[] s) {
		double Wc = Math.tan(Math.PI*6./100.);
		double k1 = Math.sqrt(2.)*Wc;
		double k2 = Wc*Wc;
		double a0 = k2/(1+k1+k2);
		double k3 = 2.*a0/k2;
		double a1 = 2.*a0;
		double b1 = -2.*a0+k3;
		double b2 = 1-2.*a0-k3;
		
		double[] y = new double[s.length];
		y[0] = s[0];
		y[1] = s[1];
		for(int i=2; i < s.length; i++) {
			y[i] = a0 * s[i] + a1 * s[i-1] + b1 * y[i-1] + b2 * y[i-2];
		}
		return y;
	}
	
	private void smoothData(boolean isA) {
		if(isA) {
			double[] chA = sig1.getSignal();
			sig1.setSignal(smooth(chA));
		} else {
			double[] chB = sig2.getSignal();
			sig2.setSignal(smooth(chB));
		}
		frame.refresh();
	}
}
