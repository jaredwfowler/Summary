package com.JSoft.Restaurant_Pal;

/**
 * Created by Jared on 4/8/2014.
 *
 *
 * Notes:
 *
 * There will not be any max limit to scores, but there will be a min of zero
 * getFunctions that return non-primitive types will return deep copied objects
 *
 *
 *
 */
public class Restaurant extends Building {

    //********************************************************************
    //CONSTANTS***********************************************************
    //********************************************************************
    public static final double DEFAULT_SCORE = 5;
    public static final String DEFAULT_ID = "Unknown_ID";
    public static final String DEFAULT_COMMENTARY = "No_Commentary";

    //********************************************************************
    //VARIABLES***********************************************************
    //********************************************************************
    private String idNumber = null;
    private double cleanliness;
    private double food;
    private double atmosphere;
    private double location;
    private String commentary = null;
    private Waiter waiter = null;

    //********************************************************************
    //METHODS*************************************************************
    //********************************************************************
    //Constructors
    Restaurant(){
        super();
        this.setDefaults();
    }
    //---------------------------------------------------------------------
    Restaurant(String s){
        super(s);
        this.setDefaults();
    }
    //---------------------------------------------------------------------
    Restaurant(String s, Address a){
        super(s,a);
        this.setDefaults();
    }
    //---------------------------------------------------------------------
    Restaurant(String s1, Address a, String s2){
        super(s1, a);
        this.setDefaults();
        this.setId(s2);
    }
    //---------------------------------------------------------------------
    Restaurant(Restaurant r){
        super(r.getName(), r.getAddress());
        this.setId(r.getId());
        this.setCleanliness(r.getCleanliness());
        this.setFood(r.getFood());
        this.setAtmosphere(r.getAtmosphere());
        this.setLocation(r.getLocation());
        this.setCommentary(r.getCommentary());
        this.setWaiter(r.getWaiter());
    }
    //---------------------------------------------------------------------
    private void setDefaults() {
        this.setId(DEFAULT_ID);
        this.setCleanliness(DEFAULT_SCORE);
        this.setFood(DEFAULT_SCORE);
        this.setAtmosphere(DEFAULT_SCORE);
        this.setLocation(DEFAULT_SCORE);
        this.setCommentary(DEFAULT_COMMENTARY);
    }
    //---------------------------------------------------------------------
    public String getId(){return this.idNumber;}
    //---------------------------------------------------------------------
    public void setId(String s){
        this.idNumber = (idNumber == null || idNumber.isEmpty()) ? DEFAULT_ID : s;
    }
    //---------------------------------------------------------------------
    public double getCleanliness(){return this.cleanliness;}
    //---------------------------------------------------------------------
    public void setCleanliness(double d){
        this.cleanliness = (d < 0) ? 0 : d;
    }
    //---------------------------------------------------------------------
    public double getAtmosphere(){return this.atmosphere;}
    //---------------------------------------------------------------------
    public void setAtmosphere(double d){
        this.atmosphere = (d < 0) ? 0 : d;
    }
    //---------------------------------------------------------------------
    public double getLocation(){return this.location;}
    //---------------------------------------------------------------------
    public void setLocation(double d){
        this.location = (d < 0) ? 0 : d;
    }
    //---------------------------------------------------------------------
    public double getFood(){return this.food;}
    //---------------------------------------------------------------------
    public void setFood(double d){
        this.food = (d < 0) ? 0 : d;
    }
    //---------------------------------------------------------------------
    public String getCommentary(){return this.commentary;}
    //---------------------------------------------------------------------
    public void setCommentary(String s){
        this.commentary = (s == null || s.isEmpty()) ? DEFAULT_COMMENTARY : s;
    }
    //---------------------------------------------------------------------
    public Waiter getWaiter(){return new Waiter(waiter);}
    //---------------------------------------------------------------------
    public void setWaiter(Waiter w){
        this.waiter = (w == null) ? new Waiter() : w;
    }
    //---------------------------------------------------------------------
    public String toString(){
        return new String(super.toString() + " " + this.idNumber + " " + this.cleanliness + " " +
                          this.food + " " + this.atmosphere + " " + this.location + " " +
                          this.commentary);
    }

}