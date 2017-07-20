// Display the colours available for koolplot programs
#include "koolplot.h" // Provides the BGI graphics functions

const int START_X_POS = 16;
const int X_SPACING = 190;
const int START_Y_POS = 18;
const int Y_SPACING = 36;
const int XRADIUS = 35;
const int YRADIUS = 16;
const int NAMEWIDTH = 130;
const int BK_COLOUR = WHITE;

struct Colour
{
    char *name;
    int rgb;
    
    void display(int xpos, int ypos, bool nameBefore);
};

void Colour::display(int xpos, int ypos, bool nameBefore)
{
    if(rgb == BK_COLOUR || rgb == BEIGE)
        setcolor(BLACK);
    else
        setcolor(rgb);
    setfillstyle(SOLID_FILL, rgb);
    
    if(nameBefore) // Then display the name before (1st column)
    {
        outtextxy(xpos, ypos + 4 + YRADIUS / 2, name);
        fillellipse(xpos + NAMEWIDTH + XRADIUS / 2 , ypos + YRADIUS / 2,
                    XRADIUS, YRADIUS);
    }
    else // name after (second column)
    { 
        fillellipse(xpos + XRADIUS / 2 , ypos + YRADIUS / 2, 
                    XRADIUS, YRADIUS);
         outtextxy(xpos + 3 * XRADIUS / 2 + 10, ypos + 4 + YRADIUS / 2, name);
    }        
}

// Prototype
bool watchMouse(void);   
 
int main(void)
{
    // list available colours in two colours (matched pairs if possible)
    Colour avail[] =
    {   {"BLACK",        BLACK},         {"DARKSLATEGRAY",  DARKSLATEGRAY},
        {"BLUE",         BLUE},          {"BLUEBLUE",       BLUEBLUE},
        {"CYAN",         CYAN},          {"MEDIUMBLUE",     MEDIUMBLUE},
        {"LIGHTCYAN",    LIGHTCYAN},     {"TURQUOISE",      TURQUOISE},
        {"LIGHTBLUE",    LIGHTBLUE},     {"DODGERBLUE",     DODGERBLUE},
        {"MAGENTA",      MAGENTA},       {"DARKVIOLET",     DARKVIOLET},
        {"LIGHTMAGENTA", LIGHTMAGENTA},  {"DEEPPINK",       DEEPPINK},
        {"BROWN",        BROWN},         {"CHOCOLATE",      CHOCOLATE},
        {"RED",          RED},           {"CRIMSON",        CRIMSON},
        {"LIGHTRED",     LIGHTRED},      {"REDRED",         REDRED},
        {"YELLOW",       YELLOW},        {"GOLDENROD",      GOLDENROD},
        {"WHITE",        WHITE},         {"DARKORANGE",     DARKORANGE},
        {"GREEN",        GREEN},         {"DARKGREEN",      DARKGREEN}, 
        {"LIGHTGREEN",   LIGHTGREEN},    {"LIMEGREEN",      LIMEGREEN}, 
        {"DARKGRAY",     DARKGRAY},      {"MEDIUMSEAGREEN", MEDIUMSEAGREEN},
        {"LIGHTGRAY",    LIGHTGRAY},     {"BEIGE",          BEIGE}
       
    };    
    int numColours = 32;

    // Put the machine into graphics mode and get the maximum coordinates:
    initwindow(410, 590, "  koolplot colours"); 
            
    
    setactivepage(0);
    // set window to white while drawing on the background page
    setbkcolor(BK_COLOUR);
    cleardevice();
    
    // Swap foreground and background pages
    setactivepage(1);
    setvisualpage(0);

    // set backbround colour
    setbkcolor(BK_COLOUR);
    cleardevice();
    
	//Set font for colour names
	settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 0);
	setusercharsize(1, 2, 1, 2);
	settextjustify(LEFT_TEXT, CENTER_TEXT);

    // Display colours in two columns
    for( int count = 0; count < numColours; count++)
    {
        int ypos = START_Y_POS + count / 2 * Y_SPACING;
        if(count % 2) // second column of colours
            avail[count].display(START_X_POS + X_SPACING, ypos, false);
        else // first column
            avail[count].display(START_X_POS, ypos, true);
    }

    // Swap foreground and background pages
    setvisualpage(1);

	/* Check for mouse or keypress */
	while(watchMouse())
		delay(100);
    
    // finish
    closegraph( );
}

/* Checks the mouse for click or keyboard for keypress. 
 * returns true if neither the mouse was clicked nor the keyboard pressed.
 */
bool watchMouse( )
{
    if(ismouseclick(WM_LBUTTONDOWN) || ismouseclick(WM_LBUTTONDOWN))
        return false;
	
	return !kbhit();
}

