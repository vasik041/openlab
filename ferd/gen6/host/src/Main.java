

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
	final static String PROPS_PATH = "/com/lz/gen6";
	
	final static Port port = new Port("COM1",9600);
	
	Param freq = new Param("Freq:",20,127,  1, "s", "period", 10, this);
	Param tpls = new Param("Tpls:", 1, 50,  1, "s", "tpls",   1,  this);
	Param dly2 = new Param("Dly2:", 0, 100, 1, "s", "dly2",   1, this);
	Param tpls2= new Param("Tpls2:",1, 50,  1, "s", "tpls2",  1, this);
		
	Graph g;
	
	public void addComponentToPane(Container pane) {
		// controls panel
		JPanel cpanel0 = new JPanel(new GridLayout(4,1));
        cpanel0.add(freq);
        cpanel0.add(tpls);
        cpanel0.add(dly2);
        cpanel0.add(tpls2);
        //cpanel0.setBorder(BorderFactory.createMatteBorder(0,0,0,1,Color.GRAY));

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

        JPanel gpanel = new JPanel(new BorderLayout());
        g = new Graph();
        gpanel.add(g,BorderLayout.CENTER);
        
        // combine all together
        JPanel panel = new JPanel(new GridLayout(1,2));
        panel.add(cpanel);
        panel.add(gpanel);
        pane.add(panel);
        syncSettings();
    }

		
	private static void createAndShowGUI() {
        //Create and set up the window.
        JFrame frame = new JFrame("Gen6 v. "+VERSION);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        
        //Create and set up the content pane.
        Main p = new Main();
        p.addComponentToPane(frame.getContentPane());

        frame.addWindowListener(new java.awt.event.WindowAdapter() {
            @Override
            public void windowClosing(java.awt.event.WindowEvent windowEvent) {
            	port.close();
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
		if(param.equals("freq")) {
			port.setFreq(val);
		} else {
			port.setT(tpls.getValue(),dly2.getValue(),tpls2.getValue());
		}
		g.setVals(freq.getValue(),tpls.getValue(),dly2.getValue(),tpls2.getValue());
		g.updateUI();
	}
	
	
	public void syncSettings() {
		port.setFreq(freq.getValue());
		port.setT(tpls.getValue(),dly2.getValue(),tpls2.getValue());
		g.setVals(freq.getValue(),tpls.getValue(),dly2.getValue(),tpls2.getValue());
		g.updateUI();
	}
}


