package com.JSoft.Restaurant_Pal;

import android.graphics.Color;

/**
 * Created by Jared Fowler on 4/8/2014.
 * Rewrite from Original Project on Eclipse
 */

public class Tip {

    //********************************************************************
    //CONSTANTS***********************************************************
    //********************************************************************
    public final static double DEFAULT_TIP = 15.00;
    public final static double MAX_TIP = 1000.00;
    public final static double MIN_TIP = 0.00;
    public final static int DEFAULT_COLOR = Color.YELLOW;
    public final static int ABOVE_DEF_COLOR = Color.GREEN;
    public final static int BELOW_DEF_COLOR = Color.RED;
    public final static int WARNING_TIME = 120;  //120 seconds
    public final static int DRAIN_TIME = 15;

    //********************************************************************
    //VARIABLES***********************************************************
    //********************************************************************
    private double tip = -1;
    private double startTip = -1;
    private long waitTimeWarningTotal = -1;
    private long waitTimeDrainTotal = -1;

    //********************************************************************
    //METHODS*************************************************************
    //********************************************************************
    //Constructors
    Tip(){
        this(DEFAULT_TIP);
    }
    //---------------------------------------------------------------------
    Tip(double d){
        this.setStartTip(d);
        this.setTip(d);
        this.setWaitTimeWarningTotal(0);
        this.setWaitTimeDrainTotal(0);
    }
    //---------------------------------------------------------------------
    Tip(Tip t){
        this.setStartTip(t.getStartTip());
        this.setTip(t.getTip());
        this.setWaitTimeWarningTotal(t.getWaitTimeWarningTotal());
        this.setWaitTimeDrainTotal(t.getWaitTimeDrainTotal());
    }
    //---------------------------------------------------------------------
    public double getStartTip(){return this.startTip;}
    //---------------------------------------------------------------------
    public void setStartTip(double d){
        this.startTip = (d < MIN_TIP) ? MIN_TIP : ((d > MAX_TIP) ? MAX_TIP : d);
    }
    //---------------------------------------------------------------------
    public double getTip(){return this.tip;}
    //---------------------------------------------------------------------
    public void setTip(double d){
        this.tip = (d < MIN_TIP) ? MIN_TIP : ((d > MAX_TIP) ? MAX_TIP : d);
    }
    //---------------------------------------------------------------------
    public long getWaitTimeWarningTotal(){return this.waitTimeWarningTotal;}
    //---------------------------------------------------------------------
    public void setWaitTimeWarningTotal(long l){
        this.waitTimeWarningTotal = (l < 0) ? 0 : l;
    }
    //---------------------------------------------------------------------
    public long getWaitTimeDrainTotal(){return this.waitTimeDrainTotal;}
    //---------------------------------------------------------------------
    public void setWaitTimeDrainTotal(long l){
        this.waitTimeDrainTotal = (l < 0) ? 0 : l;
    }
    //---------------------------------------------------------------------
    public int getTipColor(){
        return getValueColor(this.tip);
    }
    //---------------------------------------------------------------------
    public int getValueColor(double d){
        return ((d < DEFAULT_TIP) ? BELOW_DEF_COLOR : ((d > DEFAULT_TIP) ? ABOVE_DEF_COLOR : DEFAULT_COLOR));
    }
    //---------------------------------------------------------------------
    public String toString(){
        return new String(this.getStartTip() + " " + this.getTip() + " " + this.getWaitTimeWarningTotal()
                          + " " + this.getWaitTimeDrainTotal());
    }
}