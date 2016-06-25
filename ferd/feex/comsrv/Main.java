import java.awt.Container;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.Console;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;



public class Main {

	public void addComponentToPane(Container pane) {
		JPanel p = new JPanel();
		JButton b = new JButton("close");
		b.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				stop();
			}
		});
		p.add(b);
		pane.add(p);
	}
	
	private static void createAndShowGUI() {
        //Create and set up the window.
        JFrame frame = new JFrame("Virtual Com Port Server");
        frame.setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
       
        //Create and set up the content pane.
        Main p = new Main();
        p.addComponentToPane(frame.getContentPane());
        p.start();
        
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
	
	
	Server server = null;
	
	public void start() {
		server = new Server();
		server.start();
	}
	
	public void stop() {
	    server.terminate();
	    try {
			server.join();
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	    System.exit(0);
	}
}
