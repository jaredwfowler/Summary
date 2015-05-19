/*	Created By: Jared Fowler, CEO of J-Soft
	October 2013
	FowlerWayPoint282P2
	
	FowlerWayPoint282P2, extends Marker
	Waypoint is a node on a map that can be either default, gold, city, 
	or treasure map.  Each waypoint holds a list of its adjacent waypoints.
	Each waypoint has the variable ptrTo and gValue.  These are used when
	determining the A* path.  ptrTo points to the previous waypoint, and 
	gValue will hold a waypoint's traversal cost.
	
*/


import java.awt.Color;
import java.awt.Point;
import java.util.ArrayList;
import SimulationFramework.Marker;



public class FowlerWayPoint282P2 extends Marker implements Comparable<FowlerWayPoint282P2> {
	
	/*
	 *CONSTANTS****************************************************************************
	*/
	final private Color goldColor = Color.yellow;		//Color of gold waypoint
	final private Color cityColor = Color.cyan;			//Color of city waypoint
	final private Color mapColor  = Color.magenta;		//Color of treasure map waypoint
	final private Color dfColor	  = Color.black;		//Color of default waypoint
	
	final private int   goldSize  = 5;					//Size of gold waypoint
	final private int   citySize  = 5;					//Size of city waypoint
	final private int	mapSize   = 5;					//Size of treasure map waypoint
	final private int   dfSize    = 2;					//Size of default waypoint
	
	
	
	
	/*
	 * Variables***************************************************************************
	*/
	private int height,					//z position on map
		 		cost, 					//Cost of food at a city waypoint
		 		gold, 					//Gold at a waypoint
		 		mapX, 					//Treasure map x destination
		 		mapY, 					//Treasure map y destination
		 		numNeighbors;			//Number of adjacent waypoints
	
	private boolean visited;  			//Has the way point been visited?
	private ArrayList<Point> neighbors;	//Holds x,y coordinates of adjacent waypoints
	private FowlerWayPoint282P2 ptrTo;	//Points to previous waypoint in the found A* path
	
	protected double gValue;			//Overall cost to traverse to adjacent node
										//Distance + hValue + original gValue
	
	/*
	 * Constructors***********************************************************************
	*/
	//Base constructor.  Takes in minimum info needed to construct a waypoint
	public FowlerWayPoint282P2(int x_p, int y_p, int height_p, int cost_p, 
							   int gold_p, int mapX_p, int mapY_p, int neighbors_p) {
		
		
		//Set parent class Marker's Point variable - This requires me to
		//pass in a color, as Marker doesn't have appropriate constructor
		super(x_p, y_p, Color.black);  //initially set color to black
		
		//Set all the variables in this class
		setAll(height_p,cost_p,gold_p,mapX_p,mapY_p,neighbors_p);
		
		//Update the color and size of the waypoint in Marker class
		colorAndSizeSetter();
		
		//Allocate memory for array of adjacent waypoints
		neighbors = new ArrayList<Point>(numNeighbors);
		
		//Set visited to false
		visited = false;
		
		//No path found yet, so set ptrTo to null
		ptrTo = null;
	}
	
	//Base constructor is called.  List of neighbors is copied to neighbors arraylist
	public FowlerWayPoint282P2(int x_p, int y_p, int height_p, int cost_p, 
			   int gold_p, int mapX_p, int mapY_p, int neighbors_p, ArrayList<Point> ap) {
		
		//Call base constructor
		this(x_p,y_p,height_p,cost_p,gold_p,mapX_p,mapY_p,neighbors_p);
		
		//Copy values of adjacent waypoints into local arraylist variable
		setAdjacent(ap);
		
	}
	
	/*
	 * Other Methods***********************************************************************
	*/
	//------------------------------------------------------------
	//get Functions-----------------------------------------------
	protected int getX()						{return (int)getPoint().getX();}
	protected int getY()						{return (int)getPoint().getY();}
	protected int getHeight()					{return height;}
	protected int getCost()						{return cost;}
	protected int getGold()						{return gold;}
	protected int getMapX()						{return mapX;}
	protected int getMapY()						{return mapY;}
	protected int getNeighbors() 				{return numNeighbors;}
	protected ArrayList<Point> getAdjList()		{return new ArrayList<Point>(neighbors);}
	protected FowlerWayPoint282P2 getPtrTo()	{return ptrTo;}
	//------------------------------------------------------------
	//set Functions-----------------------------------------------
	protected void setHeight(int n)		{height = n;}
	protected void setCost(int n)		{cost = n;}
	protected void setGold(int n)		{gold = n;}
	protected void setMapX(int n)		{mapX = n;}
	protected void setMapY(int n)		{mapY = n;}
	protected void setNeighbors(int n)	{numNeighbors = n;}
	
	protected void setPtrTo(FowlerWayPoint282P2 fwp) {ptrTo = fwp;}
	
	protected void setAll(int z,int c,int g,int mx,int my,int n){
		setHeight(z);
		setCost(c);
		setGold(g);
		setMapX(mx);
		setMapY(my);
		setNeighbors(n);	
	}
	
	//setAdjacent will update local arraylist with the one passed in
	protected void setAdjacent(ArrayList<Point> al){
		
		//Make sure we are getting expected number of elements
		if(al.size() != numNeighbors){
			System.out.print("Warning: (# Adjacent) =! (# neighbors)\n"
							+ "# neighbors will be updated.");
			setNeighbors(al.size());
		}
				
		//Copy contents of passed in arraylist to local arraylist
		if(!(neighbors.addAll(al))){
			System.out.print("Error in copy.");
		}
	}
	//-------------------------------------------------------------
	//Visited?-----------------------------------------------------
	protected boolean isVisited()        {return visited;}	//get visited
	protected void setVisited(boolean b) {visited = b;}	//set visited
	//-------------------------------------------------------------
	//Helper Function----------------------------------------------
		/* colorAndSizeSetter determines the size and color of a waypoint
		 * and then sets it.  This method is only called when an object 
		 * of this class is constructed.
		 */
	private void colorAndSizeSetter() {
		
		//Treasure Map Waypoint
		if(mapX != 0 || mapY != 0){
			super.setColor(mapColor);
			super.setSize(mapSize);
		}
		
		//City Waypoint
		else if(cost != 0){					
			super.setColor(cityColor);
			super.setSize(citySize);
		}
		
		//Gold Waypoint
		else if(gold != 0){
			super.setColor(goldColor);
			super.setSize(goldSize);
		}
		
		//Default Waypoint
		else{
			super.setColor(dfColor);
			super.setSize(dfSize);
		}
		
	}
	//------------------------------------------------------------
	//Hash Key Function
	static protected String getHashValue(int x_value, int y_value){
		return (String.valueOf(x_value) + String.valueOf(y_value));
	}
	//------------------------------------------------------------
	//CompareTo
	public int compareTo(FowlerWayPoint282P2 wp){
		//Will compare the value stored in variable gValue
		if(gValue > wp.gValue)		{return 1;}
		else if(gValue < wp.gValue)	{return -1;}
		else						{return 0;}
	}
}