/* Modified and Updated By: Jared Fowler
 * October 2013
 * FowlerTreasurePath
 * 
 * Origin: 
 * Mike Barnes
 * 8/12/2013
 * 
 */


import java.awt.*;
import java.awt.event.*;  
import javax.swing.*;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Random;
import java.util.Scanner;		
import java.io.*;			
import java.util.HashMap;		
import java.util.PriorityQueue;
import java.util.Stack;

import SimulationFramework.*;
// CLASSPATH = ... /282projects/SimulationFrameworkV3
// PATH = ... /282projects/SimulationFrameworkV3/SimulationFramework

/**
FowlerTreasurePath is the simulation's main class (simulation app) that is a
subclass of SimFrame.  
<p> 

282 Simulation Framework applications must have a subclass of SimFrame
that also has a main method.  The simulation app can make the
appropriate author and usage "help" dialogs, override
setSimModel() and simulateAlgorithm() abstract methods
inherited from SimFrame.  They should also add any specific model
semantics and actions.

<p>

The simulated algorithm is defined in simulateAlgorithm().

<p>
FowlerTreasurePath UML class diagram 
<p>
<Img align left src="../UML/FowlerTreasurePath.png">

@since 8/12/2013
@version 3.0
@author G. M. Barnes
*/

public class FowlerTreasurePath  extends SimFrame   {
	
	
	/*
	 *CONSTANTS****************************************************************************
	*/
	// eliminate warning @ serialVersionUID
	private static final long serialVersionUID	= 42L;
	private final Color startingPointColor 		= Color.blue;	//Starting Waypoing location colored green
	private final Color endingPointColor 		= Color.blue;	//Ending waypoint location colored red
	private final Color connectorColor			= Color.black;	//Color for the connectors
	private final Color openListColor			= Color.white;	//Color for waypoints on open list
	private final Color closedListColor			= Color.gray;	//Color for waypoints on closed list
	private final int SizeOfOpenMarker			= 3;			//Size of marker on open list
	private final int SizeOfClosedMarker		= 2;			//Size of marker on the closed list
	private final int SizeOfStartEndMarkers		= 4;			//Size of start and end waypoints
	private final int VerticalSeperation 		= 20;			//Seperation of waypoints vertical
	private final int HorizontalSeperation 		= 20;			//Horizontal
	
	private final String fileName				= "waypointNeighbor.txt";
   
	/*
	 * Variables***************************************************************************
	 */
	/** the simulation application */
	private FowlerTreasurePath 						app;	//GUI components for application's menu
	private HashMap<String, FowlerWayPoint282P2> 	hMap; 	//Holds the waypoints
	private ArrayList<FowlerPlayer282P2> 			bots;	//Holds list of players
	
	private int			numPlayers;			//Holds the number of players
	private boolean 	findPathPhase;		//Phase1 in simulate Algorithm
	private boolean		traversePathPhase;	//Phase2 in simulate Algorithm
	private boolean		finalInfoPhase;		//Phase3 in simulate Algorithm
	private boolean		firstPass;			//In Phase in simulate Algorithm, is it the first found initial path?
   
 
	/*
	 * Methods*****************************************************************************
	 */

	public static void main(String args[]) {
		//Create new app. Parameters include window title and background image.
		FowlerTreasurePath app = new FowlerTreasurePath("FowlerTreasurePath     -   QFG 2   So You Want To Be A Horder?      V 1.00", "terrain282.png");
		app.start();  // start is inherited from SimFrame
	}

/**
Make the application:  create the MenuBar, "help" dialogs, 
*/

	public FowlerTreasurePath(String frameTitle, String imageFile) {
		super(frameTitle, imageFile);
		// create menus
		JMenuBar menuBar = new JMenuBar();
		// set About and Usage menu items and listeners.
		aboutMenu = new JMenu("About");
		aboutMenu.setMnemonic('A');
		aboutMenu.setToolTipText(
		"Display informatiion about this program");
		
		// create a menu item and the dialog it invoke 
		usageItem = new JMenuItem("usage");
		authorItem = new JMenuItem("author");
		usageItem.addActionListener( // anonymous inner class event handler
			new ActionListener() {        
				public void actionPerformed(ActionEvent event) {
					JOptionPane.showMessageDialog( FowlerTreasurePath.this,
						"Welcome to Shapier, the land of treasure! \n" +
						"Start by selecting a start and end point. \n" +
						"If a treasure map is found along your path, \n" +
						"you will go after the gold...being as greedy \n" +
						"as you are of course! \n" +
						"Yellow = Gold, Cyan = City, Magneta = Treasure Map",
						"Usage",   // dialog window's title
						JOptionPane.INFORMATION_MESSAGE, 
						new ImageIcon("OurHero.png"));
				}
			}
		);
		
		// create a menu item and the dialog it invokes
		authorItem.addActionListener(
				new ActionListener() {          
					public void actionPerformed(ActionEvent event) {
						JOptionPane.showMessageDialog( FowlerTreasurePath.this, 
						"Jared Fowler \n" +
						"yrolg4tseuq@sbcglobal.net \n" +
						"Comp 282",
						"author",  // dialog window's title
						JOptionPane.INFORMATION_MESSAGE,
						//  author's picture 
						new ImageIcon("me.png"));
					}
				}
		);
		
		// add menu items to menu 
		aboutMenu.add(usageItem);   // add menu item to menu
		aboutMenu.add(authorItem);
		menuBar.add(aboutMenu);
		setJMenuBar(menuBar);
		validate();  // resize layout managers
	
	}


/** 
Set up the actors (Bots), wayPoints (Markers), and possible traveral
paths (Connectors) for the simulation.
*/
	public void setSimModel() {
		//Phase 1 **************************************************************************************************
		// set any initial visual Markers or Connectors
	   
		//Initiate hMap, a HashMap   
			//Note1:Default HashMap contains 16 slots on list
			//Note2:Java will automatically take my key and Hash it
			//Note3:Create new hash map and reread file b/c some markers will be altered
		hMap = new HashMap<String, FowlerWayPoint282P2>();
		//Read WayPoint File into hMap
		readFile();
		
		
		//Iterate through the HashMap and add waypoints and connectors to animatePanel's permanentDrawable
		Iterator<Entry<String, FowlerWayPoint282P2>> it = hMap.entrySet().iterator();
		while (it.hasNext()) {
			Map.Entry<String, FowlerWayPoint282P2> pairs = (Map.Entry<String, FowlerWayPoint282P2>)it.next();

			//Add the waypoint (Marker) to animatePanel's permanent drawings
			animatePanel.addPermanentDrawable(pairs.getValue());
			
			//Go through current waypoints neighbor list and add permanent connectors
			for(int index = 0; index < pairs.getValue().getNeighbors(); index++){
				//Have the tempPtr point to waypoints adjList
				ArrayList<Point> tempPtr = pairs.getValue().getAdjList();
				//Create a new connector to connect these points
				Connector connect = new Connector(new Point(pairs.getValue().getX(), pairs.getValue().getY()),
												  tempPtr.get(index),
												  connectorColor
				);
				//Add connector to permanent drawings
				animatePanel.addPermanentDrawable(connect);	
			}
		}
		
		//Phase 2 **************************************************************************************************
		//Create the player(s) which means, getting start and end points for each as well. 
		
		//For this project, we will only have 1 player
		numPlayers = 1;
		//bots will hold the player info in an ArrayList
		bots = new ArrayList<FowlerPlayer282P2>(numPlayers);
		
		//Add the players
		for(int index = 0; index < numPlayers; index++){
			//Each Player will need a name, color, start, and end location
			
			//Get start Point
			//Update app's note to prompt user
			setStatus("Select starting location for Player" + (index + 1));
			FowlerWayPoint282P2 startPt; 
			do{
				waitForMousePosition();
				startPt = this.getNearestClick();   
			}while(startPt == null);
			//Add this start point to permanent drawables
			animatePanel.addPermanentDrawable(new Marker(startPt.getX(), 
														 startPt.getY(),
														 startingPointColor,
														 SizeOfStartEndMarkers)
			);
			
			//Get End Point
			//Update app's not to prompt user
			setStatus("Select ending location for Player" + (index + 1));
			FowlerWayPoint282P2 endPt;
			do{
				waitForMousePosition();
				endPt = this.getNearestClick();   
			}while(endPt == null);
			//Add this start point to permanent drawables
			animatePanel.addPermanentDrawable(new Marker(endPt.getX(), 
														 endPt.getY(),
														 endingPointColor,
														 SizeOfStartEndMarkers)
			);
			
			
			//Create the player object
			bots.add(new FowlerPlayer282P2("Player " + index,
											startPt.getX(),
											startPt.getY(),
											Color.red)
			);
			//update the players starting and ending points
			bots.get(index).setStartLoc(new Point(startPt.getX(), startPt.getY()));
			bots.get(index).setEndLoc(new Point(endPt.getX(), endPt.getY()));
			//Have the tm point be the same as the end point
			bots.get(index).setTmPoint(new Point(endPt.getX(), endPt.getY()));
			
			//Add the bots to animatePanel
			for(int index2 = 0; index2 < bots.size(); index2++){
				animatePanel.addBot(bots.get(index2));
			}
			
		}
		
		//Phase 3 **************************************************************************************************
		//Reset any other variables
		
		//Reset phases for simulateAlgorithm
		findPathPhase = true;
		traversePathPhase = false;
		finalInfoPhase = false;
		firstPass = true;
		
		// initialize any algorithm halting conditions (ie, number of steps/moves).
		setStatus("Initial state of simulation");
	}

/**
Will find the path using the A* algorithm and traverse the bot.
*/

	public synchronized void simulateAlgorithm() {
		//While the simulation is runnable
		while(runnable()){
				
			//Phase 1------------------------------------------------------------------------------------
			if(findPathPhase){
				//Use A* algorithm to determine the path for each bot
				for(int index = 0; index < numPlayers; index++){
					//If player has already completed path or has a path already, skip him
					if(bots.get(index).getIsDone() || bots.get(index).getPathFound()){
						continue;
					}
					
					//Clear the temporary open and closed list drawables
					animatePanel.clearTemporaryDrawables();
					
					
					//Call A* algorithm function. If no path exists, returns null
					Stack<Point> thePath = getAStarPath(bots.get(index).getStartLoc(),
															bots.get(index).getTmPoint()
															);
					
					
					//Check to see if the reset button has been pressed
					if(!runnable()) {break;}
					
					//If thePath != null, update player info (pathFound), (path)
					if(thePath != null){
						//Set pathFound to true
						bots.get(index).setPathFound(true);
						//if this is not bots first initial path, remove first element from thePath (otherwise, will revisit current site and errors will occur)
						if(!firstPass){
							thePath.pop();
						}
						//Set bots path
						bots.get(index).path2 = thePath;
					}
					//else, set isDone to true
					else{
						bots.get(index).setIsDone(true);
					}
					
					//if this is running for the first initial paths of the bots, after the last bot, set firstPass to false;
					if(firstPass && (index == numPlayers - 1))
					{
						firstPass = false;
					}
					
					//Clear the temporary open and closed list drawables
					//animatePanel.clearTemporaryDrawables();
					
					//Call simframes check state
					checkStateToWait();
				}
				
				//set phase 1 to false and phase 2 to true if simulation is runnable
				if(runnable()){
					findPathPhase = false;
					traversePathPhase = true;	
				}
				//else, set all to false
				else{
					findPathPhase = false;	//The other 2 phases should already be set to false
				}
			}
			
			//Phase 2------------------------------------------------------------------------------------
			if(traversePathPhase){
				//Switch through players, making one move at a time.
				int playerTurn = 0;
				
				while(true && runnable()){
					
					//get the player
					playerTurn %= numPlayers;
					FowlerPlayer282P2 thePlayer = bots.get(playerTurn);
					//Update the player turn
					++playerTurn;
					
					
					//Is the player already finished?
					if(thePlayer.getIsDone()){
						//Are all the players finished?
						if(allFinished()){
							//Set phase2 to false, and phase3 to true
							traversePathPhase = false;
							finalInfoPhase = true;
							//Exit out of phase2 loop
							break;
						}
						//If not all players are done, just continue on to the next player
						continue;
					}
					
					//else, make the move
					//get the next destination point off stack
					Point tempP = thePlayer.path2.pop(); 
					//Use tempP to get waypoint
					FowlerWayPoint282P2 tempW = hMap.get(FowlerWayPoint282P2.getHashValue((int)tempP.getX(), (int)tempP.getY()));
					//update number of moves made
					thePlayer.incNumMoves();
					//Subtract from players strength the cost of the move
					thePlayer.setStrength((int)(thePlayer.getStrength() - distanceBetween3D(hMap.get(FowlerWayPoint282P2.getHashValue((int)thePlayer.getPoint().getX(),
																						 										   (int)thePlayer.getPoint().getY()
																						 										   )),
																						 	tempW
																						   )
												)
										 );
					//Move bot to next waypoint
					thePlayer.moveTo(tempP);
					
					
					//Has the current player reached its final destination?
					if(tempP.equals(new Point(thePlayer.getEndLoc()))){
						thePlayer.setIsDone(true);
						continue;
					}
					
					
					//If the player has a treasure map, has he reached the waypoint?
					if(thePlayer.getHasMap() && (((int)thePlayer.getTmPoint().getX()) == tempW.getX()) && (((int)thePlayer.getTmPoint().getY()) == tempW.getY())){
						//Create string
						String info = new String();
						//Add player name to info, and that map location has been reached
						info = info.concat(thePlayer.getName() + ": Treasure Map Location Reached!  ");
						//Is the gold still there?
						if(tempW.getGold() > 0){
							info = info.concat("Gold Found: " + tempW.getGold() + "  ");
						}
						else{
							info = info.concat("Sorry. Gold has already been taken.");
						}
						//Display message
						setStatus(info);
						//Print message
						System.out.println(info);
						//Take away players map
						thePlayer.setHasMap(false);
						//The player needs to get a new path
						thePlayer.setPathFound(false);
						//set new start location
						thePlayer.setStartLoc(thePlayer.getTmPoint());
						//reset the tm point to the endpoint
						thePlayer.setTmPoint(thePlayer.getEndLoc());
						//Take away the waypoint's gold
						tempW.setGold(0);
					}
					
					
					//is the waypoint a city?
					if(tempW.getCost() != 0){
						//if players gold > city cost, then exchange for strength
						if(thePlayer.getWealth() >= tempW.getCost()){
							thePlayer.setStrength(thePlayer.getStrength() + tempW.getCost());
							thePlayer.setWealth(thePlayer.getWealth() - tempW.getCost());	
						}
						//Update message on screen
						setStatus(thePlayer.getName() + ": City Found! Exchanging gold for food. Yum Yum  "
								                      +"Wealth: " + thePlayer.getWealth()
													  + "  Strength: " + thePlayer.getStrength()
													 );	
						//Print to output
						System.out.println(thePlayer.getName() + ": City (" + tempW.getX() + "," + tempW.getY() + ")"
								  +"   Cost: " + tempW.getCost()
								  + "   Wealth: " + thePlayer.getWealth()
								  + "   Strength: " + thePlayer.getStrength()
								 );	
					}
					
					
					//Does the waypoint have/had treasure?
					else if(tempW.getGold() != 0){
						//Add the treasure to the player's wealth
						thePlayer.setWealth(thePlayer.getWealth() + tempW.getGold());
						//Update message on screen
						setStatus(thePlayer.getName() + ": Gold Found!\n"
								  + "Wealth: " + thePlayer.getWealth()
								  + "\nStrength: " + thePlayer.getStrength()
								 );
						//Print to output		
						System.out.println(thePlayer.getName() + ": Gold (" + tempW.getX() + "," + tempW.getY() + ")"
								  		   +"   Gold: " + tempW.getGold()
								  		   + "   Wealth: " + thePlayer.getWealth()
								  		   + "   Strength: " + thePlayer.getStrength()		
								  		   );
						//Take away the waypoint's gold
						tempW.setGold(0);
					}
					
					
					//Does the waypoint have a treasure map?
					else if(tempW.getMapX() != 0 || tempW.getMapY() != 0){
						//Does the player already have a map?
						if(thePlayer.getHasMap()){} //Do nothing
						//If player doesn't have a map....
						else{
							//set player hasMap to true
							thePlayer.setHasMap(true);
							//set current point to players startPoint
							thePlayer.setStartLoc(new Point(tempW.getX(), tempW.getY()));
							//set treasure map destination point to players Tm point
							thePlayer.setTmPoint(new Point(tempW.getMapX(),tempW.getMapY()));
							//Set player's pathFound to false
							thePlayer.setPathFound(false);
							//Update message on screen
							setStatus(thePlayer.getName() + ": Treasure Map Found!"
									  + "\nWealth: " + thePlayer.getWealth()
									  + "\nStrength: " + thePlayer.getStrength()
									 );
							//Print to output		
							System.out.println(thePlayer.getName() + ": Map (" + tempW.getX() + "," + tempW.getY() + ")   "
									  +"Treasure: (" + tempW.getMapX() + "," + tempW.getMapY() + ")"
									  + "   Wealth: " + thePlayer.getWealth()
									  + "   Strength: " + thePlayer.getStrength()
									 );			
									
							//eliminate the treasure map from the waypoint
							tempW.setMapX(0);
							tempW.setMapY(0);
						}
					}
					
					
					//is it a default waypoint?
					else{} //Do nothing
	
					//Incur pause in program
					checkStateToWait();
					
					//If the last player just had a turn and any player who is not done and doesn't have a path	
					if(playerTurn == numPlayers){
						if(unfinishedWithOutPath()){
							//set phase1 to true, phase2 to false, and break out of loop
							findPathPhase = true;
							traversePathPhase = false;
							break;
						}
					}
				}
			}
			
			//Phase 3------------------------------------------------------------------------------------
			if(finalInfoPhase){
				//set animate panel values
				animatePanel.setComponentState(false, false, false, false, true);
				//Create a string to hold all info to be printed
				String info = new String();
				//Fill info string
				for(int index = 0; index < numPlayers; index++){
					//Get the current player
					FowlerPlayer282P2 thePlayer = bots.get(index);
					//Add player name to string
					info = info.concat(thePlayer.getName() + ": ");
					//If success
					if(thePlayer.getPathFound() && (thePlayer.getStrength() >= 0)){
						//Add Success info to string
						info = info.concat("Success! Goal: (" + thePlayer.getEndLoc().getX() + "," + thePlayer.getEndLoc().getY() + ")  "
								    + "Strength: " + thePlayer.getStrength() + "  Wealth: " + thePlayer.getWealth() + "   Moves:" + thePlayer.getNumMoves() + "\n");
					}
					else{
						//Add failure to the string
						info = info.concat("Failure. ");
						//Was it because no path found?
						if(!(thePlayer.getPathFound())){
							info = info.concat("No Path.\n");
						}
						else{
							info = info.concat("Strength: " + thePlayer.getStrength() + "  Wealth: " + thePlayer.getWealth() + "   Moves:" + thePlayer.getNumMoves() +  "\n");
						}	
					}	
				}
				//Update status with the string
				setStatus(info);
				//Print info to output
				System.out.println(info);
				
				//Update simFrame
				checkStateToWait();
				
				//set simRunning to false
				setSimRunning(false);
				setModelValid(false);
				return;		
			}
			
		}
		
		//If we are to this point, it is because the reset button has been hit
		//Runnable is currently set to false
		//Here I will clear the temporary drawables.  Even though it has already been done
		//once already, becasue of the way my A* algorithm is set up, there should be a few
		//openlist or closed list nodes on the temp drawable list still.
		animatePanel.clearTemporaryDrawables();
		
		//return to simframe function
		return;		
	}
	
   //--------------------------------------------------------------------------------
   //*********************************************************************************
   //Read in the WayPoint file put data into HashMap by way of Scanner
	private void readFile() {
		
		//Try creating a new file class, inputing the file, and adding that to scanner class
		try{
			Scanner scanner = new Scanner(new File(fileName));
			
			//Create an ArrayList object to hold adjacent waypoint info
			ArrayList<Point> adjList = new ArrayList<Point>();
			
			//Read the file into Waypoint objects
			while(scanner.hasNextInt()){
				//Read in first eight ints from the file
				FowlerWayPoint282P2 temp = new FowlerWayPoint282P2(scanner.nextInt(), scanner.nextInt(),
																   scanner.nextInt(), scanner.nextInt(), scanner.nextInt(),
																   scanner.nextInt(), scanner.nextInt(), scanner.nextInt());
				//Clear the adjacent waypoints list
				adjList.clear();
				
				//Put adjacent waypoints into adjList
				for(int index = 0; index < temp.getNeighbors(); index++){
					adjList.add(new Point(scanner.nextInt(),scanner.nextInt()));
				}
				
				//Put this list of adjacent waypoints into our waypoint object
				temp.setAdjacent(adjList);
				
				//Add the new FowlerWayPoint282P2 to the HashMap.  Key will be
				//provided by the same class, through a static function
				hMap.put(FowlerWayPoint282P2.getHashValue( temp.getX(), temp.getY() ), temp);	
			}
			//Close the scanner
			scanner.close();
		}
		catch(Exception e){ //If file not found, or scanner error
			System.out.print("Error: File Name or Data Incorrect");
			System.exit(1);
		}	
	  }
	
	//-------------------------------------------------------------------------------------
	//Will find the nearest waypoint from where mouse was clicked (Start and End waypoints)
	private FowlerWayPoint282P2 getNearestClick() {
		
		//get the current mouse position
		int x_p = (int)mousePosition.getX();
		int y_p = (int)mousePosition.getY();
		int x_p2 = x_p;
		int y_p2 = y_p;
		
		//This will point to the nearest waypoint. 
		FowlerWayPoint282P2 theNearest = null;
		//Create a list to hold all possible waypoints within range
		ArrayList<FowlerWayPoint282P2> list = new ArrayList<FowlerWayPoint282P2>();  
		
		
		//Find how far away the selected point is away from a multiple of VerticalSeperation and HorizontalSeperation
		int remainder = x_p % HorizontalSeperation;
		
		if(remainder < (HorizontalSeperation / 2))  //If closer to the left multiple, subtract
			x_p -= remainder;
		else	//otherwise add
			x_p += (HorizontalSeperation - remainder);
			
		remainder = y_p % VerticalSeperation;
		
		if(remainder < (VerticalSeperation / 2))	//If closer to the north multiple, subtract
			y_p -= remainder;
		else	//otherwise add
			y_p += (VerticalSeperation - remainder);
			
			
		//Get the waypoint and the possible 8 surrounding waypoints and put them into the array
		for(int index = -(HorizontalSeperation);index <= HorizontalSeperation; index += HorizontalSeperation) {
		
			for(int index2 = -(VerticalSeperation);index2 <= VerticalSeperation; index2 += VerticalSeperation) {
			
				if(hMap.containsKey(FowlerWayPoint282P2.getHashValue(x_p + index, y_p + index2))){
				
					list.add(hMap.get(FowlerWayPoint282P2.getHashValue(x_p + index, y_p + index2)));
				
				}
			
			}
			
		}
		
		//If no waypoint was within range, return null.  This will be handled by the calling method
		if(list.size() == 0)
			return theNearest;
		
		//Set the first element in the list to be the nearest, then go through the list and compare
		theNearest = list.get(0);
		
		//Temporary Waypoint to hold original mouse click position
		FowlerWayPoint282P2 temp = new FowlerWayPoint282P2(x_p2, y_p2, 0, 0, 0, 0, 0, 0); 
		
		for(int index = 1; index < list.size(); ++index){
			
			theNearest = getNearest2D(theNearest, list.get(index), temp);
			
		}
			
		return theNearest;
	}
	
	//-------------------------------------------------------------------------------------
	//Method that will find the nearest waypoint by comparing two waypoints to the destination 2D
				
	private FowlerWayPoint282P2 getNearest2D(FowlerWayPoint282P2 a, FowlerWayPoint282P2 b, FowlerWayPoint282P2 destination) {
			
			if(distanceBetween2D(a,destination) < distanceBetween2D(b,destination))
			{
				return a;
			}
			else{
				return b;
			}
	}
		
	//------------------------------------------------------------------------------------------
	//Method that will find distance between two waypoints 3D
	private double distanceBetween3D(FowlerWayPoint282P2 a, FowlerWayPoint282P2 b){
			
		return (Math.sqrt(Math.pow(b.getX() - a.getX(), 2) + Math.pow(b.getY() - a.getY(), 2) + Math.pow(b.getHeight() - a.getHeight(), 2)));				
	}
	
	//------------------------------------------------------------------------------------------
	//Method that will find distance between two waypoints 2D
	private double distanceBetween2D(FowlerWayPoint282P2 a, FowlerWayPoint282P2 b) {
		return (Math.sqrt(Math.pow(b.getX() - a.getX(), 2) + Math.pow(b.getY() - a.getY(), 2)));
	}
	//------------------------------------------------------------------------------------------
	//Determines if all the players have reached end location, or no path exists
	private boolean allFinished(){
		for(int index = 0; index < numPlayers; index++){
			//Will return false moment one is found to be false
			if(!bots.get(index).getIsDone()){
				return false;
			}
		}
		return true;
	}
	//------------------------------------------------------------------------------------------
	//Determine if any player needs to get a new path
	private boolean unfinishedWithOutPath(){
		for(int index = 0; index < numPlayers; index++){
			//Will return true moment one is found to be true
			if(!bots.get(index).getIsDone() && !bots.get(index).getPathFound()){
				return true;
			}
		}
		return false;
	}
	//------------------------------------------------------------------------------------------
	/**
	 * Determines the A* path between 2 FowlerWayPoint282P2.
	 * @param s
	 * @param e
	 * @return 
	 * Stack<Point>
	 */
	//Determine the A* path, and return an ArrayList of Points
	private Stack<Point> getAStarPath(Point s, Point e){
		//Will use a minHeap Priority Queue for the open list
		PriorityQueue<FowlerWayPoint282P2> oList = new PriorityQueue<FowlerWayPoint282P2>();
		//Closed list will use a hashMap
		HashMap<String, FowlerWayPoint282P2> cList = new HashMap<String, FowlerWayPoint282P2>();
		//Has a path been found?
		boolean found = false;
		//Get a copy of the destination waypoint
		FowlerWayPoint282P2 EndW = hMap.get(FowlerWayPoint282P2.getHashValue((int)e.getX(), (int)e.getY()));
		
		//Get first waypoint
		FowlerWayPoint282P2 tempW = hMap.get(FowlerWayPoint282P2.getHashValue((int)s.getX(), (int)s.getY()));
		//Have it point to null
		tempW.setPtrTo(null);
		//Set its gValue
		tempW.gValue = 0;
		//add the first point to the open list
		oList.add(tempW);
		//add temp drawable to represent waypoint on open list
		animatePanel.addTemporaryDrawable(new Marker(tempW.getX(),tempW.getY(),openListColor,SizeOfOpenMarker));
		
		
		
		//Loop: while the open list is not empty || destination !Found , and while runnable
		while(!oList.isEmpty() && !found && runnable()){
			
			//Allow SimFrame to repaint
			checkStateToWait();
			
			
			//Dequeue from the open list 
			tempW = oList.poll();
			//Is this the end point?
			if(tempW.equals(EndW)){
				//Set found equal to true
				found = true;
				//exit out of the loop
				continue;
			}
			//If not, then put waypoint onto closed list, and add its adjacent waypoints
			cList.put(tempW.getX() +"::" + tempW.getY(), tempW);
			//Add tempDrawable representing waypoint on closed list
			animatePanel.addTemporaryDrawable(new Marker(tempW.getX(),tempW.getY(),closedListColor,SizeOfClosedMarker));
			//Add adjacent waypoints to open list
			//Get the list from the waypoint
			ArrayList<Point> nList = tempW.getAdjList();
			//Determine which ones to add to open list
			for(int index = 0; index < nList.size(); ++index){
				//Is the waypoint already on the closed list?
				if(cList.containsKey((int)nList.get(index).getX() +"::"+ (int)nList.get(index).getY())){
					//If so, then skip it
					continue;
				}
				//Is the waypoint already on the open list?
				if(oList.contains(hMap.get(FowlerWayPoint282P2.getHashValue((int)nList.get(index).getX(), (int)nList.get(index).getY())))){
					//If so, then skip it
					continue;
				}
				//Else, this waypoint needs to be added to the open list
				//First have it point back to the waypoint that is accessing it
				FowlerWayPoint282P2 tempW2 = hMap.get(FowlerWayPoint282P2.getHashValue((int)nList.get(index).getX(), (int)nList.get(index).getY()));
				tempW2.setPtrTo(tempW);
				//Next, update the gValue (that is the waypoints g value + distance between + hValue
				tempW2.gValue = tempW.gValue + distanceBetween3D(tempW2,tempW) + distanceBetween3D(tempW2,EndW);
				//Finally, add this waypoint to the open list
				oList.add(tempW2);
				//Add temp drawable
				animatePanel.addTemporaryDrawable(new Marker(tempW2.getX(),tempW2.getY(),openListColor,SizeOfOpenMarker));
			}
		}
		
		
		//At this point, either the path has been found or no path exists
		//It is also possible that the client has pushed the reset key
		//If no path exists, return null
		if(!found || !runnable()){
			return null;
		}
		//If it does exist, we will now need to trace the path from end to start.
		Stack<Point> stackPath = new Stack<Point>();
		//Add the end point to the stack
		stackPath.push(new Point(EndW.getX(), EndW.getY()));
		//The destination waypoint should point back to what accesses it
		tempW = EndW.getPtrTo();
		//While the pointer is not null, keep on going back
		while(tempW != null){
			//push tempW onto stack
			stackPath.push(new Point(tempW.getX(), tempW.getY()));
			//Point back
			tempW = tempW.getPtrTo();
		}
		
		return stackPath;
	}
}