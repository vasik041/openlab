/**
 * Implementation of Plotdata class
 * 
 * Stores data for one 2D plotting axis
 *
 * Provides operators and mathematical functions to map a Plotdata
 * object to another using unary and binary functions.
 *
 * Author: 	jlk
 * Version:	1.1
 * Date:	July 2005
 *
 * This file is in the public domain and can be used without any
 * restriction.
 */

#include <functional>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <limits>
#include "graphics.h"

#include "plotdata.h" 


/*
 * Class (static) function
 * Extracts a colour change requested by special command
 * Colour change command is of format:
 *
 * sequence of doubles in x ->  NOPLOT | NOPLOT | colour |
 *                        ------------------------------------
 * sequence of doubles in y ->  NOPLOT | colour | NOPLOT |
 * 
 * @return - requested colour, or NOCOLOR if data sequences are not
 * a valid colour change command.
 */
int Plotdata::colorChange(dataIterator &x, dataIterator &y)
{
    int retColour = NOCOLOR;
    
    // Do nothing if NOPLOTs are incorrect
    if(    isfinite(*x) || isfinite(*(x+1)) || !isfinite(*(x+2))
        || isfinite(*y) || isfinite(*(y+2)) || !isfinite(*(y+1)));
    // Do nothing if x & y colours are not valid and equal
    else if(isfinite(*(x+2)) && isfinite(*(y+1))
              && (*(x+2) != *(y+1)|| !isColor(static_cast<int>(*(x+2)))));
    // Else, return the colour and reset iterators to last elements in sequences
    else
    {
        retColour = static_cast<int>(*(x+2));
        x += 2;
        y += 2;
    }
    
    return retColour;
}

/*
 * Class (static) function
 * Insert special command for a colour change within a pair of Plotdata
 * Colour change command is of format:
 *
 * sequence of doubles in x ->  NOPLOT | NOPLOT | colour |
 *                             ------------------------------------
 * sequence of doubles in y ->  NOPLOT | colour | NOPLOT |
 */
void Plotdata::colorChange(Plotdata &x, Plotdata &y, int colour)
{
    x << NOPLOT << NOPLOT << colour;
    y << NOPLOT << colour << NOPLOT;
}

/*
 * Class (static) function
 * Insert special command to reset colour to before last colour change
 * Colour change command is of format:
 *
 * sequence of doubles in x ->  NOPLOT |    NOPLOT   | RESETCOLOUR |
 *                             -------------------------------------------
 * sequence of doubles in y ->  NOPLOT | RESETCOLOUR |   NOPLOT    |
 */
void Plotdata::colorReset(Plotdata &x, Plotdata &y)
{
    x << NOPLOT  << NOPLOT    << RESETCOLOR;
    y << NOPLOT  << RESETCOLOR << NOPLOT;
}

/*
 * Class (static) function
 * Insert a single (invisible) point into plotdata pair
 */
void Plotdata::soliton(Plotdata &x, Plotdata &y, double xval, double yval)
{
    x << NOPLOT << xval << NOPLOT;
    y << NOPLOT << yval << NOPLOT;
}


/*
 * Class (static) function
 * Extracts single point coordinates requested by special command
 * Single point command is of format:
 *
 * sequence of doubles in x ->  NOPLOT | xCoord | NOPLOT |
 *                        ------------------------------------
 * sequence of doubles in y ->  NOPLOT | NOPLOT | yCoord |
 * 
 * @return - true if data sequences are a valid single point
 */
bool Plotdata::singlePoint( double & xCoord, double & yCoord,
                            dataIterator &x, dataIterator &y)
{
    bool ret = false;
    
    // Do nothing if NOPLOTs are incorrect
    if(    isfinite(*x) || isfinite(*(x+2)) || !isfinite(*(x+1))
        || isfinite(*y) || isfinite(*(y+1)) || !isfinite(*(y+2)));
    // Else, return the coordinates and reset iterators to the
    // last elements in the sequences
    else
    {
        xCoord = *(x+1);
        yCoord = *(y+2);
        x += 2;
        y += 2;
        ret = true;
    }
    
    return ret;
}

/*
 * Class (static) function
 * Insert special command for a single point within a pair of Plotdata
 * Single point command is of format:
 *
 * sequence of doubles in x ->  NOPLOT | xCoord | NOPLOT |
 *                             ------------------------------------
 * sequence of doubles in y ->  NOPLOT | NOPLOT | yCoord |
 */
void Plotdata::singlePoint( Plotdata &x, Plotdata &y, 
                            double xCoord, double yCoord)
{
    x << NOPLOT << xCoord << NOPLOT;
    y << NOPLOT << NOPLOT << yCoord;
}

/*
 * Class (static) function
 * Checks that an integer represents a valid colour
 */ 
bool Plotdata::isColor(int theColour)
{
    return IS_BGI_COLOR(theColour) 
             || (IS_RGB_COLOR(theColour) && theColour >= 0)
                || theColour == RESETCOLOR;
}

/* 
 * Constructors
 */
Plotdata::Plotdata(double lo, double hi)
: userFunction(0), userBinFunction(0)
{
	plotRange(lo, hi, MEDIUM, LIN);
}	 

Plotdata::Plotdata(double lo, double hi, Grain grain)
: userFunction(0), userBinFunction(0)
{
	plotRange(lo, hi, grain, LIN);
}

Plotdata::Plotdata(const double *array, int dataSize)
: data(array, array + dataSize), userFunction(0), userBinFunction(0)
{}

/*  
 * Member Functions
 */
// Insert dataSize elements from array into Plotdata
void Plotdata::insert(const double array[], int dataSize)
{
    // Append, rather than insert at the start
    copy(array, array + dataSize, back_inserter(data));
    // data = vector<double>(array, array + dataSize);
}

 
void Plotdata::plotRange(	double lo, 
							double hi, 
							int numpoints, 
							LogSpace isLog)
{
	int divisions = numpoints - 1;
	double rangeSize, shift;

	if (divisions < 2) divisions = 49; // Guarantees a smooth enough curve
	
	if(lo > hi)
		swap(lo, hi);
		
	// If space is logarithmic do nothing if hi and lo are of opposite signs
	if(isLog && lo < 0 && hi > 0)
		return;
	
	rangeSize = hi - lo;
	
	// if hi and lo are too close to each other do nothing
	if(rangeSize < numeric_limits<float>::epsilon())
		return;
		
	   
	data.clear(); 
	
	if( !isLog)
	{
		data.push_back(lo);

		// distance between 2 points
		double dist = rangeSize / divisions; 
		 
		for (int i = divisions; i > 1; i--)
			data.push_back(lo += dist);
	}
	else // Logarithmic range
	{
		// Neither lo nor hi may be 0. Move away if too close
		// Dirty kludge! Avoids crash but introduces inaccuracies.
		if (fabsl(lo) < numeric_limits<float>::epsilon())
		{
			shift = rangeSize / 1000;
			if ( lo < 0 )
				shift = - shift;
			while(fabsl(lo) < numeric_limits<float>::epsilon())	   
				lo += shift;
		}
		if (fabsl(hi) < numeric_limits<float>::epsilon())
		{
			shift = - rangeSize / 1000;
			if ( hi > 0 )
				shift = - shift;
			while(fabsl(hi) < numeric_limits<float>::epsilon())	   	      	  	  	  
				hi += shift;
		}
		
		data.push_back(lo);
	      	   
	   	double mult = logl(hi/lo) / divisions;
		for (int i = 1; i < divisions; i++)
		{
			data.push_back(lo * expl(i * mult));
		}
	}
	
	data.push_back(hi);
}

Plotdata Plotdata::operator + (const Plotdata & other) const
{
    int thisSize = data.size(),
        otherSize = other.data.size();
    
	// Size of returned data is that of the smallest of the two
	size_t len = thisSize < otherSize ? thisSize : otherSize;
	
	vector<double> vec(len);
	transform(data.begin(),
		 	  data.begin() + len, 
		 	  other.data.begin(),
			  vec.begin(),
		 	  plus<double>());
	return Plotdata(vec);
}

Plotdata Plotdata::operator + (double val) const
{
	vector<double> vec(data.size());
	transform(data.begin(),
		 	  data.end(), 
			  vec.begin(),
		 	  bind2nd(plus<double>(), val));
	return Plotdata(vec);
}

Plotdata operator + (double op1, const Plotdata & pd)
{
	vector<double> vec(pd.data.size());
	transform(pd.data.begin(),
		 	  pd.data.end(), 
			  vec.begin(),
		 	  bind2nd(plus<double>(), op1));
	return Plotdata(vec);
}
	   
Plotdata Plotdata::operator - (const Plotdata & other) const
{
	// Size of returned data is that of the smallest of the two
	size_t len = size() > other.size() ? size() : other.size();
	
	vector<double> vec(len);
	transform(data.begin(),
		 	  data.begin() + len, 
 	  	      other.data.begin(),
			  vec.begin(),
		 	  minus<double>());
	return Plotdata(vec);
}

Plotdata Plotdata::operator - (double val) const
{
	vector<double> vec(data.size());
	transform(data.begin(),
		 	  data.end(), 
			  vec.begin(),
		 	  bind2nd(minus<double>(), val));
	return Plotdata(vec);
}

Plotdata operator - (double op1, const Plotdata & pd)
{
	vector<double> vec(pd.data.size());
	transform(pd.data.begin(),
		 	  pd.data.end(), 
			  vec.begin(),
		 	  bind2nd(minus<double>(), op1));
	return Plotdata(vec);
}

Plotdata Plotdata::operator * (const Plotdata & other) const
{
	// Size of returned data is that of the smallest of the two
	size_t len = size() > other.size() ? size() : other.size();
	
	vector<double> vec(len);
	transform(data.begin(),
		 	  data.begin() + len, 
	   	      other.data.begin(),
			  vec.begin(),
		 	  multiplies<double>());
	return Plotdata(vec);
}

Plotdata Plotdata::operator * (double val) const
{
	vector<double> vec(data.size());
	transform(data.begin(),
		 	  data.end(), 
			  vec.begin(),
		 	  bind2nd(multiplies<double>(), val));
	return Plotdata(vec);
}

Plotdata operator * (double op1, const Plotdata & pd)
{
	vector<double> vec(pd.data.size());
	transform(pd.data.begin(),
		 	  pd.data.end(), 
			  vec.begin(),
		 	  bind2nd(multiplies<double>(), op1));
	return Plotdata(vec);
}

 
Plotdata Plotdata::operator / (const Plotdata & other) const
{
	// Size of returned data is that of the smallest of the two
	size_t len = size() > other.size() ? size() : other.size();
	
	vector<double> vec(len);
	transform(data.begin(),
		 	  data.begin() + len, 
			  other.data.begin(),
			  vec.begin(),
		 	  divides<double>());
	return Plotdata(vec);
}

Plotdata Plotdata::operator / (double val) const
{
	vector<double> vec(data.size());
	transform(data.begin(),
		 	  data.end(), 
			  vec.begin(),
		 	  bind2nd(divides<double>(), val));
	return Plotdata(vec);
}

Plotdata Plotdata::operator ^ (double val) const
{
	vector<double> vec(data.size());
	transform(data.begin(),
		 	  data.end(), 
			  vec.begin(),
		 	  bind2nd(ptr_fun(powl), val));
	return Plotdata(vec);
}


Plotdata operator / (double op1, const Plotdata & pd)
{
	vector<double> vec(pd.data.size());
	transform(pd.data.begin(),
		 	  pd.data.end(), 
			  vec.begin(),
		 	  bind2nd(divides<double>(), op1));
	return Plotdata(vec);
}


Plotdata Plotdata::operator - ( ) const
{
	vector<double> vec(data);
	transform(vec.begin(),
		 	  vec.end(), 
		 	  vec.begin(),
		 	  negate<double>());
	return Plotdata(vec);
}

// Concatenation operator
Plotdata & Plotdata::operator << (const Plotdata & toadd)
{
	data.insert(data.end(), toadd.data.begin(), toadd.data.end());
	
	return *this; // This makes the << operator transitive
}

// add a double to the data
Plotdata & Plotdata::operator << (double toadd)
{
	data.push_back(toadd);
	return *this;
}

// Functor class is_finite: Returns true if val is neither
// NaN (NOPLOT) nor infinite. A wrapper around isfinite(), necessary as
// isfinite() is a macro and cannot be used as a predicate.
class is_finite
{
public:
    bool operator() (double val) const
    {
		return isfinite(val);   
    }
};
	   
// Return the smallest number in data
// Or NOPLOT if no data or all data is NOPLOT
double Plotdata::min( )const
{
	double smallest;
	
	if(!data.size())
		return NOPLOT;
		
	// Start only from the 1st finite element in vector if any
	vector<double>::const_iterator firstfinite, it;
	firstfinite = find_if(data.begin(), data.end(), is_finite());
	
	// return if no finite element
	if(firstfinite == data.end())
		return NOPLOT;
	
	smallest = *firstfinite;
	for(it = firstfinite; it != data.end(); it++)
	{
		if(isfinite(*it) && *it < smallest)
			smallest = *it;
	}
	return smallest;
}

// Return the largest number in data
// Or NOPLOT if no data or all data is NOPLOT
double Plotdata::max( )const
{
	if(!data.size())
		return NOPLOT;

	// Start only from the 1st finite element in vector if any
	vector<double>::const_iterator firstfinite, it;
	firstfinite = find_if(data.begin(), data.end(), is_finite());
	
	// return if no finite element
	if(firstfinite == data.end())
		return NOPLOT;
	
	double largest = *firstfinite;
	for(it = firstfinite; it != data.end(); it++)
	{
		if(isfinite(*it) && *it > largest)
			largest = *it;
	}
	return largest;
}

/**
 * Class (static) function
 * Retrieves the maximums of each of x and y in a Plotdata pair
 * Values corresponding to a NOPLOT in the other Plotdata are not included;
 * Assign their respective maximums to xMax and yMax; NOPLOT if either does
 * not have a valid maximum.
 */
void Plotdata::max( const Plotdata &x, const Plotdata &y,
                    double &xMax, double &yMax)
{
	 xMax = yMax = NOPLOT;

    // Only search if both vectors contain some values
    if(x.data.size() && y.data.size())
    {
    	dataIterator xIter = x.data.begin();
        dataIterator yIter = y.data.begin();
        dataIterator xEnd  = x.data.end();
        dataIterator yEnd  = y.data.end();
        
        for( ; xIter != xEnd && yIter != yEnd; xIter++, yIter++)
        {
            if(isfinite(*xIter) && isfinite(*yIter))
            {
                if(!isfinite(xMax) || xMax < *xIter)
                    xMax = *xIter;       
                if(!isfinite(yMax) || yMax < *yIter)
                    yMax = *yIter; 
            }
            // If both x and y are NOPLOT it may be a single point
            else if((xIter + 2) != xEnd) // if a special command
            {
                double xCoord, yCoord;
                if(Plotdata::singlePoint(xCoord, yCoord, xIter, yIter)) 
                {
                    if(!isfinite(xMax) || xMax < xCoord)
                        xMax = xCoord;       
                    if(!isfinite(yMax) || yMax < yCoord)
                        yMax = yCoord; 
                }
            }
        }
    }
}
   
/**
 * Class (static) function
 * Retrieves the minimums of each of x and y in a Plotdata pair
 * Values corresponding to a NOPLOT in the other Plotdata are not included;
 * Assign their respective maximums to xMax and yMax; NOPLOT if either does
 * not have a valid minimum.
 */
void Plotdata::min( const Plotdata &x, const Plotdata &y, 
                    double &xMin, double &yMin)
{
	 xMin = yMin = NOPLOT;

    // Only search if both vectors contain some values
    if(x.data.size() && y.data.size())
    {
    	dataIterator xIter = x.data.begin();
        dataIterator yIter = y.data.begin();
        dataIterator xEnd  = x.data.end();
        dataIterator yEnd  = y.data.end();
        
        for( ; xIter != xEnd && yIter != yEnd; xIter++, yIter++)
        {
            if(isfinite(*xIter) && isfinite(*yIter))
            {
                if(!isfinite(xMin) || xMin > *xIter)
                    xMin = *xIter;       
                if(!isfinite(yMin) || yMin > *yIter)
                    yMin = *yIter; 
            }     
            // If both x and y are NOPLOT it may be a single point
            else if((xIter + 2) != xEnd) // if a special command
            {
                double xCoord, yCoord;
                if(Plotdata::singlePoint(xCoord, yCoord, xIter, yIter)) 
                {
                    if(!isfinite(xMin) || xMin > xCoord)
                        xMin = xCoord;       
                    if(!isfinite(yMin) || yMin > yCoord)
                        yMin = yCoord; 
                }
            }
        }
    }
}

// Return new plot data with unary function applied to each 
// original data elements values
Plotdata Plotdata::doFunc(Func aFunction) const
{
	Plotdata ret(size()); 
	
	transform(data.begin(), data.end(), ret.data.begin(), aFunction);
	return ret;
}	 

// Return new plot data with binary function applied to each original
// data elements values and op2 as the second function param
Plotdata Plotdata::doBinFunc(BinFunc aFunc, double op2) const
{
	Plotdata ret(size()); 
	
	transform(data.begin(), data.end(), ret.data.begin(),
				bind2nd(ptr_fun(aFunc), op2));
	return ret;
}	 

// Return new plot data with user binary function applied to each
// original data elements values and op2 as the second function param
Plotdata Plotdata::doBinFunc(double op2) const
{
	Plotdata ret(size()); 
	
	transform(data.begin(), data.end(), ret.data.begin(),
				bind2nd(ptr_fun(userBinFunction), op2));
	return ret;
}	 


ostream & operator << (ostream & out, const Plotdata & pd)
{
	out << endl << pd.data.size() << endl;
	copy(pd.data.begin(), pd.data.end(), ostream_iterator<double>(out, " "));
	return out;
}

istream & operator >> (istream & in, Plotdata & pd)
{
	int size;
	double val;
	
	in >> size;
	
	for(int i = size; i > 0; i--)
	{
		in >> val;
		pd.data.push_back(val);
	}
	return in;
}

