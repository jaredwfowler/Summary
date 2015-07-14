package com.JSoft.Restaurant_Pal;

/**
 * Created by Jared Fowler on 4/8/2014.
 */

public class Waiter {

    //********************************************************************
    //CONSTANTS***********************************************************
    //********************************************************************
    public static final double DEFAULT_SCORE = 5;
    public static final String DEFAULT_NAME = "Unknown_Name";
    public static final String DEFAULT_COMMENTARY = "No_Commentary";

    //********************************************************************
    //VARIABLES***********************************************************
    //********************************************************************
    private String name = null;
    private double personality;
    private double apparel;
    private double service;
    private double accuracy;
    private String commentary = null;

    //********************************************************************
    //METHODS*************************************************************
    //********************************************************************
    //Constructors
    public Waiter(){
        this(DEFAULT_NAME);
    }
    //---------------------------------------------------------------------
    public Waiter(String s){
        this.setName(s);
        this.setDefaults();
    }
    //---------------------------------------------------------------------
    public Waiter(Waiter w) {
        this.setName(w.getName());
        this.setPersonality(w.getPersonality());
        this.setApparel(w.getApparel());
        this.setService(w.getService());
        this.setAccuracy(w.getAccuracy());
        this.setCommentary(w.getCommentary());
    }
    //---------------------------------------------------------------------
    private void setDefaults(){
        this.setPersonality(DEFAULT_SCORE);
        this.setApparel(DEFAULT_SCORE);
        this.setService(DEFAULT_SCORE);
        this.setAccuracy(DEFAULT_SCORE);
    }
    //---------------------------------------------------------------------
    public String getName(){return this.name;}
    //---------------------------------------------------------------------
    public void setName(String s){
        this.name = (s == null || s.isEmpty()) ? DEFAULT_NAME : s;
    }
    //---------------------------------------------------------------------
    public double getPersonality(){return this.personality;}
    //---------------------------------------------------------------------
    public void setPersonality(double d){
        this.personality = (d < 0) ? 0 : d;
    }
    //---------------------------------------------------------------------
    public double getApparel(){return this.apparel;}
    //---------------------------------------------------------------------
    public void setApparel(double d){
        this.apparel = (d < 0) ? 0 : d;
    }
    //---------------------------------------------------------------------
    public double getService(){return this.service;}
    //---------------------------------------------------------------------
    public void setService(double d){
        this.service = (d < 0) ? 0 : d;
    }
    //---------------------------------------------------------------------
    public double getAccuracy(){return this.accuracy;}
    //---------------------------------------------------------------------
    public void setAccuracy(double d){
        this.accuracy = (d < 0) ? 0 : d;
    }
    //---------------------------------------------------------------------
    public String getCommentary(){return this.commentary;}
    //---------------------------------------------------------------------
    public void setCommentary(String s){
        this.commentary = (s == null || s.isEmpty()) ? DEFAULT_COMMENTARY : s;
    }
    //---------------------------------------------------------------------
    public String toString(){
        return new String(this.name + " " + this.personality + " " + this.apparel +
                          " " + this.service + " " + this.accuracy + " " + this.commentary);
    }

}
