/* Created By: Jared Fowler
 * October 2013
 * FowlerPlayer282P2
 * 
 * FowlerPlayer282P2 extends Bot.  It adds on the functions of 
 * player strength, wealth, and can hold a map.  Each player also
 * keeps track of its starting and final destinations, and if player currently
 * has a treasure map, it will keep track of that destination as well. Once 
 * the treasure map is used up, the new start location will be updated with 
 * the treasure destination.  Each player also keeps track of how many moves
 * it has made.
 * 
 */

import java.awt.Color;
import java.awt.Point;
import java.util.Stack;
import SimulationFramework.Bot;



public class FowlerPlayer282P2 extends Bot {
	
	
	/*
	 *CONSTANTS****************************************************************************
	*/
	private final int START_STR 	= 2000;		//Initial Strength
	private final int START_GOLD	= 1000;		//Initial Wealth
		
	
	/*
	 * Variables***************************************************************************
	*/
	private int 	strength;		//Players current Strength
	private int 	wealth;			//Players current wealth
	private boolean hasMap;			//Does the player currently have a treasure map?
	private Point	startPoint;		//Holds the players starting point location
	private Point	endPoint;		//Holds the players final destination
	private Point	tmPoint;		//If treasure map found, this will its location
	private int		numMoves;		//Number of moves player has made
	private boolean pathFound;		//Does there exist a path between start and end points?
	private boolean isDone;			//Has the player reached its destination, or no path?
	protected Stack<Point> path2;	//Will hold the calculated A* path
	
	
	/*
	 * Constructors***********************************************************************
	*/
	//Constructor 1------------------------------------------------
	public FowlerPlayer282P2(String playerName, int x_p, int y_p, Color c) {
		
		//Call bot class constructor
		super(playerName, x_p, y_p, c);
		//Set player's initial strength
		setStrength(START_STR);	
		//Set player's initial health
		setWealth(START_GOLD);
		//Player will not start with a map
		setHasMap(false);
		//Player starts off with zero moves made
		setNumMoves(0);
		//Set all location points to null
		startPoint = null; endPoint = null; tmPoint = null;
		//Player has not found a path, nor reached its destinaiton
		pathFound = false;	isDone = false;
		
	}
	//Constructor 2------------------------------------------------
	public FowlerPlayer282P2(int x_p, int y_p){	
		
		//Call constructor 1, setting playerName and color to default values
		this("Player", x_p, y_p, Color.red);	
	}
	
	
	/*
	 * Other Methods***********************************************************************
	*/
	//------------------------------------------------------------
	//get Functions-----------------------------------------------
	protected String getName()		{return new String("name");}
	protected int getStrength()		{return strength;}
	protected int getWealth()		{return wealth;}
	protected boolean getHasMap()	{return hasMap;}
	protected Point getStartLoc()	{return new Point(startPoint);}
	protected Point getEndLoc()		{return new Point(endPoint);}
	protected Point getTmPoint()	{return new Point(tmPoint);}		
	protected int getNumMoves()		{return numMoves;}
	protected boolean getPathFound(){return pathFound;}
	protected boolean getIsDone()	{return isDone;}
	//------------------------------------------------------------
	//set Functions-----------------------------------------------
	protected void setStrength(int x)		{strength = x;}
	protected void setName(String n) 		{name = n;}
	protected void setWealth(int x)			{wealth = x;}
	protected void setHasMap(boolean b)		{hasMap = b;}
	protected void setStartLoc(Point p)		{startPoint = p;}
	protected void setEndLoc(Point p)		{endPoint = p;}
	protected void setTmPoint(Point p)		{tmPoint = p;}
	protected void setNumMoves(int x)		{numMoves = x;}
	protected void incNumMoves()			{++numMoves;}
	protected void setPathFound(boolean b)	{pathFound = b;}
	protected void setIsDone(boolean b)		{isDone = b;}
	//------------------------------------------------------------
	//------------------------------------------------------------
	public void move() {  
		//My move method does nothing.  I will probably just use Bot.moveTo method
	}
	
}
