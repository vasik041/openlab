import java.awt.Frame;
import java.awt.Rectangle;
import java.util.prefs.Preferences;


public class Utils {

	public static void storeSizeAndPosition(Frame f) {
		Preferences prefs = Preferences.systemRoot().node(Main.PROPS_PATH);
		Rectangle r = f.getBounds();
        int x = (int)r.getX();
        int y = (int)r.getY();
        int w = (int)r.getWidth();
        int h = (int)r.getHeight();

        prefs.putInt("win.x",x);
        prefs.putInt("win.y",y);
        prefs.putInt("win.w",w);
        prefs.putInt("win.h",h);
    }

	public static void restoreSizeAndPosition(Frame f) {
		Preferences prefs = Preferences.systemRoot().node(Main.PROPS_PATH);
        int x = prefs.getInt("win.x",-1);
        int y = prefs.getInt("win.y",-1);
        int w = prefs.getInt("win.w",-1);
        int h = prefs.getInt("win.h",-1);
        if(x != -1 && y != -1 && w != -1 && h != -1) {
        	f.setBounds(new Rectangle(x,y,w,h));
        }
    }

	
	public static double interpolate(double x,double x0,double y0,double x1,double y1) {
		if(x <= x0) return y0;
		if(x >= x1) return y1;
		double dx = x1-x0;
		double dy = y1-y0;
		return (x-x0) * dy / dx + y0;
	}

	// power supply voltage DAC
	// 10..30 -> 0..255
	public static int convetPSU(int v) {
		//10v,49 -- 30v,164
		if(v <= 10) return 49;
		if(v >= 30) return 164;
		return (int)interpolate(v,10,49,30,164);
	}

	public static int convetPSUback(int v) {
		//10v,49 -- 30v,164
		if(v <= 49) return 10;
		if(v >= 164) return 30;
		return (int)interpolate(v,49,10,164,30);
	}

	// load value 0..99 -> 0..255
	public static int convetLoad(int v) {
		return (int)interpolate(v,0,0,99,255);
	}

	public static int convetLoadback(int v) {
		return (int)interpolate(v,0,0,255,99);
	}

	// power supply voltage ADC
	// 0..1023 -> 0..35v
	public static double convetPsuU(int v) {
		return interpolate(v,0,0,1023,35);
	}

	// power supply current ADC
	// 0..1023 -> 0..10mA
	public static double convetPsuI(int i,double u) {
		// 10k resistor as a constant load was added to improve stability
		return interpolate(i,0,0,1023,10)-interpolate(u,9.7,0,30,0.283)-u/10.;
	}

	
	// load voltage ADC
	// 0..1023 -> 0..5v
	public static double convetLoadU(int v) {
		return interpolate(v,0,0,1023,5);
	}

	// load current ADC
	// 0..1023 -> 0..150mA
	public static double convetLoadI(int v) {
		return interpolate(v,0,0,1023,150);
	}
	
}
