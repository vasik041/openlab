import java.awt.FlowLayout;

import javax.swing.JLabel;
import javax.swing.JPanel;


public class ParamView extends JPanel {

	JLabel n = new JLabel();
	JLabel v = new JLabel();
	String cap;
	
	public ParamView(String name,String cap1) {
		cap = cap1;
		setLayout(new FlowLayout());
		add(n);
		add(v);
		n.setText(name);
		setValue("0");
	}
	
	
	public void setValue(String val) {
		v.setText(val+" "+cap);
	}
}
