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
	
}
