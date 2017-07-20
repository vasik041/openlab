/*
 * Implementation of the koolplot 2D plotting library
 *
 * Author: 	jlk
 * Version:	1.1
 * Date:	July 2005
 * 
 * This file is in the public domain and can be used without
 * any restriction.
 */
#include <cmath>

#include "koolplot.h"

void (*saveFunc)(HWND h) = NULL;

// ----------- more function prototypes -------  ----------------------
// May be placed in the header file for user program access if they are
// deemed useful at some stage.

/*
 * Defines a plot data range with data spread linearly along the range
 *
 * @param min lowest point in the range
 * @param max highest point in the range
 */
void plotRange(Plotdata & pd, 
				double min,
				double max);

/*
 * Defines a plot data range with data spread along the range
 *
 * @param min lowest point in the range
 * @param max highest point in the range
 * @param numpoints number of points in the range 
 *                  (or one of EXTRA_FINE, FINE, COARSE, or GROSS).
 */
void plotRange(Plotdata & pd, 
				double min,
				double max,
				size_t numpoints);

/*
 * Defines a plot data range with data spread along the range
 * If the range is in logarithmic space, min and max must have
 * the same sign and must not be 0.
 *
 * @param min lowest point in the range
 * @param max highest point in the range
 * @param numpoints number of points in the range
 *                  (or one of EXTRA_FINE, FINE, COARSE, or GROSS).
 * @param space LOG for logarithmic space, LIN for linear space 
 */
void plotRange(Plotdata & pd, 
				double min,
				double max,
				size_t numpoints,
				LogSpace space);

// -----------------------------------------------------------------
// --------------------- Start Code --------------------------------

/*
 * Add a point to a plot data
 * @param pd  the Plotdata to add the point to
 * @param val value of the point to add
 */
void point(Plotdata & pd, double val)
{
	pd.point(val);
}

/*
 * Adds next point to plotting data
 *  
 * Same as above, but will take both axes and coordinates in one call. 
 * 
 * @param xAxis the x Plotdata to add the point to
 * @param xCoord value of the x coordinate to add
 * @param yAxis the y Plotdata to add the point to
 * @param yCoord value of the y coordinate to add
 */
void point(Plotdata & xAxis, Plotdata & yAxis,
           double xCoord, double yCoord)
{
	xAxis.point(xCoord);
	yAxis.point(yCoord);
}

/*
 * Insert array of data points into plotting data
 *  
 * @param axis the  plotdata to add the points to
 * @param array The array of points to insert
 * @param numToInsert the number of points to add from the array.
 */
void insert(plotdata_ref axis, const double array[], int numToInsert)
{
    axis.insert(array, numToInsert);
}

/** Sets the bottom left corner of the graph axes 
  * xmin and ymin should be less than or equal to any coordinate
  * on the graph.
  */ 
void axesBotLeft(Plotdata &x, Plotdata &y, 
                 double xmin, double ymin)
{
        Plotdata::soliton(x, y, xmin, ymin);
}
                 
/** Sets the top right corner of the graph axes
  * xmax and ymax should be larger than or equal to any coordinate
  * on the graph.
  */ 
void axesTopRight(Plotdata &x, Plotdata &y, double xmax, double ymax)
{
        Plotdata::soliton(x, y, xmax, ymax);
}

/** Adds visible mark to plot, centered on coordinates xpos, ypos */
void addMark(Plotdata &x, Plotdata &y, double xpos, double ypos)
{
    addMark(x, y,xpos, ypos, RED);
}

/** Adds visible mark at  coordinates xpos, ypos, specify colour */
void addMark(Plotdata &x, Plotdata &y, double xpos, double ypos, int colour)
{
    setColor(x, y, colour);
    Plotdata::singlePoint(x, y, xpos, ypos);
    resetColor(x, y);
}

/** Set colour of graph to requested colour */
void setColor(Plotdata &x, Plotdata &y, int colour)
{
    Plotdata::colorChange(x, y, colour);
}

/** Reset colour of graph to last colour before change */
void resetColor(Plotdata &x, Plotdata &y)
{
    Plotdata::colorReset(x, y);
}

void plotRange(Plotdata & pd, 
				double min,
				double max)
{
	pd.plotRange(min, max, FINE, false);
}

/*
 *	Add a range of points to a plot data
 */
 void plotRange(Plotdata & pd, 
				double min, double max,
 				size_t numpoints)
{
	pd.plotRange(min, max, numpoints, false);
}

/*
 *	Add a range of points to a plot data
 */
 void plotRange(Plotdata & pd, 
				double min, double max,
 				size_t numpoints,
				LogSpace isLog)
{
	pd.plotRange(min, max, numpoints, isLog);
}

/*
 * Print data on axis pd to standard output
 */
void printPlotdata(const Plotdata & pd)
{
	cout << pd;
}

/*
 * Clear data from a Plotdata object
 */
void clear(Plotdata & pd)
{
	pd.clear();
}

/*
 * Open a graphical Plotstream and plot x..y data
 */
void plot( const Plotdata & x, const Plotdata & y)
{
	Plotstream ps;
	
	ps.plot(x, y);
} 

	   
/*
 * Open a graphical Plotstream and plot x..y data, specify colour
 */
void plot( const Plotdata & x, const Plotdata & y, int colour)
{
	Plotstream ps;
	
	ps.plot(x, y, colour);
} 

/*
 * Plot x..y data, specify label
 */
void plot(const Plotdata & x, const Plotdata & y, char *label) 
{
	Plotstream ps(label);
	
	ps.plot(x, y);
} 

/*
 * Plot x..y data, specify label and colour
 */
void plot(const Plotdata & x, const Plotdata & y, char *label, int colour) 
{
	Plotstream ps(label);
	
	ps.plot(x, y, colour);
} 

/*
 * Plot x..y data, specify colour, then label.
 */
void plot(const Plotdata & x, const Plotdata & y, int colour, char *label) 
{
	Plotstream ps(label);
	
	ps.plot(x, y, colour);
} 

/*
 * Plot f(x) function (use together with f(x) function)
 */
void plot(const Plotdata & x, Func & aFunction)
{
	if(aFunction == 0)
		return;
		
	Plotstream ps;
	
	ps.plot(x, x.doFunc(aFunction));
}

/*
 * Plot f(x) function, specify curve colour
 */
void plot(const Plotdata & x, Func & aFunction, int colour)
{
	if(aFunction == 0)
		return;

   Plotstream ps;
	
	ps.plot(x, x.doFunc(aFunction), colour);
} 
 

/**
 * Plot f(x) function, specify label (use together with f(x) function)
 */
void plot(const Plotdata & x, Func & aFunction, char *label)
{
	Plotstream ps(label);
	
	ps.plot(x, x.doFunc(aFunction));
} 
 
/**
 * Plot f(x) function, specify label and curve colour (use together with f(x) function)
 */
void plot(const Plotdata & x, Func & aFunction, char *label, int colour) 
{
	if(aFunction == 0)
		return;
		
	Plotstream ps(label);
	
	ps.plot(x, x.doFunc(aFunction), colour);
} 

/**
 * Plot f(x) function, specify curve colour, then label (use together with f(x) function)
 */
void plot(const Plotdata & x, Func & aFunction, int colour, char *label) 
{
	if(aFunction == 0)
		return;
		
	Plotstream ps(label);
	
	ps.plot(x, x.doFunc(aFunction), colour);
} 

/**
 * Access a user-defined unary function of x as f(x)
 * example: Plotdata x(-6, 6);
 *			f(x) = sinc;	// if sinc is a function defined by the user.
 *			plot(x, f(x));	// Plots the graph of sinc(x) from -6 to 6
 */
Func & f(Plotdata & x)
{
	return x.userfunc();
}

/*
 * Access a user-defined binary function of x as f2(x)
 * example: Plotdata x(-270, 270);
 *			f2(x) = tanLessThan; // Install user-defined binary function.
 */
BinFunc & f2(Plotdata & x)
{
	return x.userBinfunc();
}

/*
 * Calculates a user-defined binary function of x as y = f2(x, double_value)
 * example: Plotdata x(-270, 270);
 *			f2(x) = tanLessThan; 
 *			plot(x, f2(x, 10));	 // Plots the graph of tanLessThan(x, 10)
 */
Plotdata f2(const Plotdata & x, double op2)
{
	return x.doBinFunc(op2);
}

/*
 * Breaks the current plot data at this point.
 *
 * Later, when plotting, the last point of the data previously 
 * entered will not be joined to the first point of the next data.
 * This allows plotting more than one function on a single graph
 * using the "point" method of data entry.
 * @param xAxis the x data to break
 * @param yAxis the y data to break
 */
void breakplot(Plotdata & x, Plotdata & y)
{
 	point(x, y, NOPLOT, NOPLOT);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~ MATH.H FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*
 * Return new data, the sine of the original data
 */
Plotdata sin(const Plotdata & pd)
{
	return pd.doFunc(sin);
}

/*
 * Return new data, the cosine of the original data
 */
Plotdata cos(const Plotdata & pd)
{
	return pd.doFunc(cos);
}

/*
 * Return new data, the tan of the original data
 */
Plotdata tan(const Plotdata & pd)
{
	return pd.doFunc(tan);
}

/*
 * Return new data, the asin of the original data
 */
Plotdata asin(const Plotdata & pd)
{
	return pd.doFunc(asin);
}

/*
 * Return new data, the acos of the original data
 */
Plotdata acos(const Plotdata & pd)
{
	return pd.doFunc(acos);
}


/*
 * Return new data, the atan of original the data
 */
Plotdata atan(const Plotdata & pd)
{
	return pd.doFunc(atan);
}

/*
 * Return new data, the hyperbolic sine of the original data
 */
Plotdata sinh(const Plotdata & pd)
{
	return pd.doFunc(sinh);
}

/*
 * Return new data, the hyperbolic cosine of the original data
 */
Plotdata cosh(const Plotdata & pd)
{
	return pd.doFunc(cosh);
}

/*
 * Return new data, the hyperbolic tan of the original data
 */
Plotdata tanh(const Plotdata & pd)
{
	return pd.doFunc(tanh);
}

/*
 * Return new data, the square root of the original data
 */
Plotdata sqrt(const Plotdata & pd)
{
	return pd.doFunc(sqrt);
}

/*
 * Return new data, the absolute value of the original data
 */
Plotdata fabs(const Plotdata & pd)
{
	return pd.doFunc(fabs);
}

/*
 * Return new data, the natural logarithm of the original data
 */
Plotdata log(const Plotdata & pd)
{
	return pd.doFunc(log);
}

/*
 * Return new data, the log (base 10)l of the original data
 */
Plotdata log10(const Plotdata & pd)
{
	return pd.doFunc(log10);
}

/*
 * Return new data, the exponential of the original data
 */
Plotdata exp(const Plotdata & pd)
{
	return pd.doFunc(exp);
}

/*
 * Return new data, the power "exp" of the original data
 */
Plotdata pow(const Plotdata & pd, double exp)
{
	return pd.doBinFunc(pow, exp);
}

