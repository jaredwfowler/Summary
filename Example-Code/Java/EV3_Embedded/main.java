/* Comp 598EA w/Professor Wigely
 * Group Members: Jared Fowler, Ilia Benson, David Greenberg
 * 
 * Project: We are to program the LegoMindstorm EV3 to look
 * for a black circle on the ground with a smaller white 
 * circle within that black circle. This robot will be 
 * put into competition with other robots in a game of musical 
 * chairs. Whichever robot is more precisely on the white circle 
 * wins. 
 * 
 * Sensor and Connectors: (The location of the distance, eye-looking, sensor will be the front.)
 * 
 * -EV3 Box should be placed with the screen towards the front.
 * -Right Motor:        Slot D
 * -Left Motor:         Slot A
 * -Color Sensor:       Slot 2
 * -UltraSonic Sensor:  Slot 3
 * -Touch Sensor Right: Slot 1
 * -Touch Sensor Left:  Slot 4
 * 
 * 
 * http://sourceforge.net/p/lejos/wiki/Sensor%20Framework/?version=3
 * 
 */

package comp598EA_EV3;

import java.io.File;

import lejos.hardware.Button;
import lejos.hardware.Sound;
import lejos.hardware.lcd.LCD;
import lejos.hardware.motor.Motor;
import lejos.hardware.port.SensorPort;
import lejos.robotics.Color;
import lejos.robotics.RegulatedMotor;
import lejos.robotics.SampleProvider;
import lejos.utility.Delay;
import lejos.hardware.sensor.EV3ColorSensor;
import lejos.hardware.sensor.EV3UltrasonicSensor;
import lejos.hardware.sensor.EV3TouchSensor;
import lejos.hardware.sensor.SensorModes;


public class main {
	//Enumerations ############################################################
	private enum STATUS_FLAG{
		LOST,
	    CIRCLE_BLACK,
	    CIRCLE_WHITE
	};
	public enum PLAYLIST{
		FIND_SHIT,
		WE_DID_IT,
		NO_INTEL,
		NO_BRAIN,
		HATE_CONFRONT,
		EXIT_THREAD,
		NONE
	};

	//Constants ###############################################################
	private static final float CIRCLE_RADIUS_BLACK = 4.0f;    //inches of outer circle radius
	private static final float CIRCLE_RADIUS_WHITE = 0.5f;    //inches of inner circle radius
	private static final float AXIS_TO_COLOR_SENSOR = 1.25f;  //inches between center of rotation and color sensor
	private static final float CENTER_TO_COLOR_SENSOR = 1.0f; //inches between center of bot to color sensor
	private static final float AXIS_TO_REAR = 6.0f;           //inches between center of rotation and rear end
	private static final float WHEEL_CIRCUMFERNCE = 7.0f;     //inches of wheel circumference
	private static final int SPEED_NORMAL = 270;              //degrees rotation/second
	private static final int SPEED_SLOW = 90;                 //degrees rotation/second
	private static final int EDGE_CASE_ROTATE = 100;          //degrees rotate if lost black circle
	private static final float SMALL_MOVE = 0.25f;            //inches
	private static final int NORMAL_ROTATION = 135;           //degrees
	private static final int LOST_COUNT_B4_AUDIO = 450;       //calls to lost procedure before playing sound file (10sec ~ 100 calls)
	
	private static final String[] soundList = new String[] { "findshit_converted.wav", "DoraWeDidIt.wav", 
		"ToyStoryNoIntel.wav", "WizardOfOzNoBrain.wav", "ToyStoryHateConfront.wav" };
	
	//Variables ###############################################################
	private static SensorModes ultraSonic_sensor;
	private static SensorModes color_sensor;
	private static SensorModes touch_sensor0;
	private static SensorModes touch_sensor1;
	private static SampleProvider distance;
	private static SampleProvider color;
	private static SampleProvider touch0;
	private static SampleProvider touch1;
	
	private static float[] distance_sample;
	private static float[] color_sample;
	private static float[] touch_sample0;
	private static float[] touch_sample1;
	
	private static Thread musicThread = null;
	private static PLAYLIST currentSound;
	private static RegulatedMotor[] motorSync;
	private static STATUS_FLAG flag;
	private static long lostCount;
	
	//Functions ###############################################################
	
	//*************************************************************************
	//*************************************************************************
	public static void main(String[] args) {
		
		//Initialize Sensors, Motors, etc.
		init();      
		writeLCD("Ready to go.. press button to start");
		Button.waitForAnyEvent();
		Delay.msDelay(4000);
		
		//Flag Based Procedure
		while(true) {
			if(flag == STATUS_FLAG.LOST){
				lostProcedure();
			}
			else if(flag == STATUS_FLAG.CIRCLE_BLACK){
				outerCircleProcedureSmart();
			}
			else if(flag == STATUS_FLAG.CIRCLE_WHITE){
				//Set audio flag
				changeCurrentSound(PLAYLIST.WE_DID_IT);
				break;
			}
			else{
				//Illegal flag value
				break;
			}

			if(Button.waitForAnyPress(1) != 0)
			{ //Exit if button is held down
				//Stop motors
				fullStop();
				//Play sound file
				changeCurrentSound(PLAYLIST.NO_BRAIN);
				break;
			}
		}
		
		//Clean up
		//Exit audio thread
		changeCurrentSound(PLAYLIST.EXIT_THREAD);
		
		//Wait for keypress
		writeLCD("DONE. Press a buton.");
		Button.waitForAnyEvent();
		
	}
	//*************************************************************************
	//*************************************************************************
	
	public static void lostProcedure(){
		//Update the lostCount
		lostCount += 1;
		//Play audio?
		if(lostCount >= LOST_COUNT_B4_AUDIO){
			//Set audio flag
			changeCurrentSound(PLAYLIST.FIND_SHIT);
			lostCount = 0;
		}
			
		//Move forward
		setSpeed(SPEED_NORMAL);
		fullForward();
		
		//Get the color we are on
		float _color = getCurrentColor();
		
		if(_color == Color.BLACK){ //We have found the outer circle
			//Stop the robot
			fullStop();
			setSpeed(SPEED_SLOW);
			
			//Are we still on the same color?
			_color = getCurrentColor();
			
			if(_color != Color.BLACK){
				//Attempt to move back onto the black
				for(int i = 0; i < 2; i++){
					//Back it up a little
					moveBackward(SMALL_MOVE);
					//Get the color again
					_color = getCurrentColor();
					if(_color == Color.BLACK)
						break;
				}
			}
			
			if(_color == Color.BLACK){ //We have found black circle!
				//Update status flag
				flag = STATUS_FLAG.CIRCLE_BLACK;
			}
			else{ //We are still lost
				//Restore Motor Speed
				setSpeed(SPEED_NORMAL);
			}
			
			//In any case, return out of this function
			return;
		}
		
		//Handle the Touch Sensors
		float _touch0 = getTouch0();
		float _touch1 = getTouch1();
		if(_touch0 == 1 || _touch1 == 1){ //We have been rear-ended
			//Set audio flag
			changeCurrentSound(PLAYLIST.NO_INTEL);
		}
		
		//Handle the UltraSonic Sensor
		float _distance = getDistanceCM();
		if(_distance < AXIS_TO_REAR * 1.5){
			//Stop, turn around, start again.
			fullStop();
			rotateRight(NORMAL_ROTATION);
		}
	}
	private static void outerCircleProcedureSmart(){
		/* We know a few things... the black circle is only 8
		 * inches in diameter. We can therefore traverse across the circle .5 inches 
		 * at a time, counting the number of times we do so. Upon reaching a non-black
		 * area, we can either conclude that we are at the center b/c we will have gone 
		 * 4 inches, or if the value is greater than or less to this, we can calculate where 
		 * we are in the circle, rotate the correct amount, and hopefully find the center.
		 */
		float _color;
		
		//It is possible that we only hit the edge of the black and are back on white
		_color = getCurrentColor();
		if(_color != Color.BLACK){
			//Back up a little bit
			moveBackward(AXIS_TO_COLOR_SENSOR);
			//Rotate the robot
			rotateLeft(EDGE_CASE_ROTATE);
			//Black should be either forward or backwards...
			moveForward(SMALL_MOVE * 4);
			_color = getCurrentColor();
			if(_color != Color.BLACK){
				//Turn around and go the other way
				rotateLeft(360);
				moveForward(SMALL_MOVE * 8);
				_color = getCurrentColor();
				if(_color != Color.BLACK){
					//We are lost... :-(
					flag = STATUS_FLAG.LOST;
					return;
				}
			}
		}
		
		//Move backwards until we find the edge of black circle
		for(int i = 0; i < ((2 * CIRCLE_RADIUS_BLACK) / SMALL_MOVE); i++)
		{
			moveBackward(SMALL_MOVE);
			//Are we on non-black?
			_color = getCurrentColor();
			if(_color != Color.BLACK){
				//Move back onto black
				moveForward(SMALL_MOVE);
				break;
			}
			else if(i == (((2 * CIRCLE_RADIUS_BLACK) / SMALL_MOVE) - 1)){
				//We are probably stuck on the black..good enough
				writeLCD("Stuck in black circle?");
				flag = STATUS_FLAG.CIRCLE_WHITE;
				return;
			}
		}
		
		//At this point we are hopefully at the edge of the circle
		//Count how many forward increments are needed to reach another non-black area
		int forwardIncrements = 0;
		for(int i = 0; i < ((2 * CIRCLE_RADIUS_BLACK) / SMALL_MOVE); i++)
		{
			moveForward(SMALL_MOVE);
			forwardIncrements += 1;
			//Are we on non-black?
			_color = getCurrentColor();
			if(_color != Color.BLACK)
				break;
		}
		
		//Determine if we are at the center, on on the edge
		moveForward(SMALL_MOVE * 2);
		_color = getCurrentColor();
		moveBackward(SMALL_MOVE * 2);
		if(_color == Color.BLACK){ //It is the center circle
			//Set Flag and return
			flag = STATUS_FLAG.CIRCLE_WHITE;
			return;
		}	
		
		//We know how many intervals it takes to get across the circle. Backup half-way, 
		//turn 90 degrees and move (4 - d/2), where d = forwardIncrements * SMALL_MOVE. 
		//We won't know which way to turn, so try one way first.. and it doesn't work
		//out, try the other.
		float moveDist = ((forwardIncrements / 2.0f) * SMALL_MOVE);
		//We can find the distance to the center of the circle using this equation: d = (r^2 - moveDist^2)^.5
		float moveDistCenter = (float) Math.sqrt(Math.pow(CIRCLE_RADIUS_BLACK, 2) - Math.pow(moveDist, 2));
		
		//Backup half-way (Take into account that when we rotate, the color sensor is not on the axis of rotation)
		moveBackward(moveDist + AXIS_TO_COLOR_SENSOR);
		
		//Turn 90 degrees to the right
		rotateRight(90);
		
		//Move forward, hopefully towards the center white circle, and overshoot it
		moveForward(moveDistCenter + (3 * CIRCLE_RADIUS_WHITE));
		
		//If the color value is black, we went the right way, else, reverse 
		_color = getCurrentColor();
		if(_color == Color.BLACK){
			//Back up onto the white circle
			moveBackward(3 * CIRCLE_RADIUS_WHITE - CENTER_TO_COLOR_SENSOR);
			//Set flag, and return
			flag = STATUS_FLAG.CIRCLE_WHITE;
		}
		else{
			//Backup 
			moveBackward((3 * CIRCLE_RADIUS_WHITE) + (2 * moveDistCenter) - CENTER_TO_COLOR_SENSOR);
			//Set flag, and return
			flag = STATUS_FLAG.CIRCLE_WHITE;
		}
		
		//We 'should' be at our destination... but not guaranteed 
		//due to possibly being hit by another bot.
		return;
	}
	private static void init(){
		initSensors();
		initMotors();
		
		//Initialize variables
		lostCount = 0;
		flag = STATUS_FLAG.LOST;
		changeCurrentSound(PLAYLIST.NONE);
		
		//Initialize audio thread - Sounds seem to not work properly...unknown reason. random behavior.
		//musicThread = new Thread(new MusicThread());
		//musicThread.start();
		
	}
	private static void initMotors(){
		//Initialize motor speeds
		setSpeed(SPEED_NORMAL);
		
		//Initialize motor sync.
		motorSync = new RegulatedMotor[1];  
		motorSync[0] = Motor.A;
		Motor.D.synchronizeWith(motorSync);
	}
	private static void initSensors(){
		//Initialize UltraSonic Sensor
		ultraSonic_sensor = new EV3UltrasonicSensor(SensorPort.S3);
		distance = ultraSonic_sensor.getMode("Distance");
		distance_sample = new float[distance.sampleSize()];
		
		//Initialize Color Sensor
		//Color ID mode. The sensor can identify 8 unique colors (NONE, BLACK, BLUE, GREEN, YELLOW, RED, WHITE, BROWN).
		color_sensor = new EV3ColorSensor(SensorPort.S2);
		color = color_sensor.getMode("ColorID");
		color_sample = new float[color.sampleSize()];
		
		//Initialize Touch Sensors ... Value is either 0 or 1, aka, true or false
		touch_sensor0 = new EV3TouchSensor(SensorPort.S1);
		touch_sensor1 = new EV3TouchSensor(SensorPort.S4);
		touch0 = touch_sensor0.getMode("Touch");
		touch1 = touch_sensor1.getMode("Touch");
		touch_sample0 = new float[touch0.sampleSize()];
		touch_sample1 = new float[touch1.sampleSize()];
	}
	private static void writeLCD(String str){
		LCD.clear();
		LCD.drawString(str, 0, 0);
	}
	private static void setSpeed(int speed){
		Motor.D.startSynchronization();
		Motor.A.setSpeed(speed);
		Motor.D.setSpeed(speed);
		Motor.D.endSynchronization();
	}
	private static void fullForward(){
		Motor.D.startSynchronization();
		Motor.A.forward();
		Motor.D.forward();
		Motor.D.endSynchronization();
	}
	private static void fullReverse(){
		Motor.D.startSynchronization();
		Motor.A.backward();
		Motor.D.backward();
		Motor.D.endSynchronization();
	}
	private static void fullTurnRight(){
		Motor.D.startSynchronization();
		Motor.A.forward();
		Motor.D.backward();
		Motor.D.endSynchronization();
	}
	private static void fullTurnLeft(){
		Motor.D.startSynchronization();
		Motor.A.backward();
		Motor.D.forward();
		Motor.D.endSynchronization();
	}
	private static void fullStop(){
		Motor.D.startSynchronization();
		Motor.A.stop();
		Motor.D.stop();
		Motor.D.endSynchronization();
	}
	private static float getCurrentColor(){
		color.fetchSample(color_sample, 0);
		return color_sample[0];
	}
	private static float getDistanceCM(){
		distance.fetchSample(distance_sample, 0);
		return (distance_sample[0] * 100);
	}
	private static float getTouch0(){
		touch0.fetchSample(touch_sample0, 0);
		return touch_sample0[0];
	}
	private static float getTouch1(){
		touch1.fetchSample(touch_sample1, 0);
		return touch_sample1[0];
	}
	private static void moveForward(float distance /*Inches*/){
		//Ensure that the motor speeds are the same
		Motor.D.setSpeed(Motor.A.getSpeed());
		int timeMS = (int)(((distance / WHEEL_CIRCUMFERNCE) * 360.0) * (1.0 / Motor.A.getSpeed()) * 1000);
		fullForward();
		Delay.msDelay(timeMS);
		fullStop();
	}
	private static void moveBackward(float distance /*Inches*/){
		//Ensure that the motor speeds are the same
		Motor.D.setSpeed(Motor.A.getSpeed());
		int timeMS = (int)(((distance / WHEEL_CIRCUMFERNCE) * 360.0) * (1.0 / Motor.A.getSpeed()) * 1000);
		fullReverse();
		Delay.msDelay(timeMS);
		fullStop();
	}
	private static void rotateLeft(float angle /*Degrees*/){
		/*One full rotation is 720 degrees on wheels.*/
		//Ensure that the motor speeds are the same
		Motor.D.setSpeed(Motor.A.getSpeed());
		int timeMS = (int)((angle * 2) * (1.0 / Motor.A.getSpeed()) * 1000);
		fullTurnLeft();
		Delay.msDelay(timeMS);
		fullStop();
	}
	private static void rotateRight(float angle /*Degrees*/){
		/*One full rotation is 720 degrees on wheels.*/
		//Ensure that the motor speeds are the same
		Motor.D.setSpeed(Motor.A.getSpeed());
		int timeMS = (int)((angle * 2) * (1.0 / Motor.A.getSpeed()) * 1000);
		fullTurnRight();
		Delay.msDelay(timeMS);
		fullStop();
	}
	private static synchronized void changeCurrentSound(PLAYLIST play){
		currentSound = play;
	}
	
	//Embedded Classes ########################################################
	public static class MusicThread implements Runnable {
		/* NOTE: I've discovered that the audio NEEDS to be in a separate thread.
		 * Also, I've discovered that the EV3 will break if 2 audio files are 
		 * attempted to play at once.
		 */
		
		//Variables
		File file = null;
		
        //Functions
        public MusicThread() {
              // TODO Auto-generated constructor stub
        }
        
		public void run() {
			
			//Variables
			file = new File(soundList[0]);
			PLAYLIST _currentSound;
			
			while(true){
				
				_currentSound = currentSound;
				
				if(_currentSound != PLAYLIST.NONE){
					
					if(_currentSound == PLAYLIST.EXIT_THREAD){
						Sound.beep();
						return;
					}
					
					Sound.setVolume(20);
					int delay = Sound.playSample(file, 100); //TODO - CHANGE VALUE TO toPlay and sound level
					//Delay.msDelay(delay);
					
					//Sound.setVolume(10);
		            //Sound.beep();
					
					changeCurrentSound(PLAYLIST.NONE);
				}
				else{
					//Rest for a while
					Delay.msDelay(25);
				}
			}
        }
	}
	
}
