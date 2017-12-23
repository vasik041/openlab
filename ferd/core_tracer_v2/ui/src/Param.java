import java.awt.Dimension;
import java.awt.FlowLayout;
import java.util.prefs.Preferences;

import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;



public class Param extends JPanel implements ChangeListener {
	String cap;
	String pref_key;
	ParamChanged cb;
	JLabel c = new JLabel();
	JSlider slider;
	Preferences prefs = Preferences.systemRoot().node(Main.PROPS_PATH);
	int value;
	boolean ignoreNextChange = false;
	
	private void setCaption(String v) {
		c.setText(v+cap);
	}
	
	public Param(String name,int minVal,int maxVal,int step,String cap1,
			String pref_key1,int defVal1,ParamChanged cb1) {
		cap = cap1;
		pref_key = pref_key1;
		cb = cb1;
		int defVal = prefs.getInt(pref_key, defVal1);
		if(defVal > maxVal) defVal = maxVal;
		if(defVal < minVal) defVal = minVal;
		JLabel n = new JLabel(name);
		slider = new JSlider(JSlider.HORIZONTAL, minVal, maxVal, defVal);
		slider.addChangeListener(this);
		slider.setMajorTickSpacing(step);
		slider.setPaintTicks(true);
		slider.setPreferredSize(new Dimension(400,50));
		setLayout(new FlowLayout());
		add(n);
		add(slider);
		add(c);
		setCaption(""+defVal);
		value = defVal;
	}
	
	public void stateChanged(ChangeEvent e) {
		if(ignoreNextChange) {
			ignoreNextChange = false;
			return;
		}
		JSlider source = (JSlider)e.getSource();
	    if (!source.getValueIsAdjusting()) {
	        int v = (int)source.getValue();
	        setCaption(""+v);
	        prefs.putInt(pref_key,v);
	        cb.syncHW(pref_key, v);
	        value = v;
	    }
	}

	
	public void setValue(int v) {
		if (v == value) return;
		
		if (!slider.getValueIsAdjusting()) {
			ignoreNextChange = true;
			setCaption(""+v);
			slider.setValue(v);
			prefs.putInt(pref_key,v);
			value = v;
		}
	}
	
	public int getValue() {
		int v = slider.getValue();
		value = v;
		return v;
	}
}
