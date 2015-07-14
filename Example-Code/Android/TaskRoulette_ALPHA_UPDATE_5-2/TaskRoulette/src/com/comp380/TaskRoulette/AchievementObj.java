package com.comp380.TaskRoulette;

/**
 * Writer/Editor: Jared Fowler
 * Date 3/27/2014
 * Using: ECLIPSE/INTELLIJ
 */

public class AchievementObj {
    private String name = "";
    private double hoursReq = -1;
    private double taskCountReq = -1;
    private boolean Earned = false;
    private String imageName = "";
    private String reqDescription = "";

    public AchievementObj(){

    }

    public AchievementObj(String Name, double hours, double taskCount, boolean Earned, String imgFile, String description){
        name = Name;
        hoursReq = hours;
        taskCountReq = taskCount;
        this.Earned = Earned;
        imageName = imgFile;
        reqDescription = description;
    }

    public void setName(String Name){
        this.name = Name;
    }

    public String getName(){
        return this.name;
    }

    public String getDescription(){
        return this.reqDescription;
    }

    public String getImageName(){
        return this.imageName;
    }

    public void setEarned(boolean b){
        this.Earned = b;
    }

    public boolean getEarned(){
        return this.Earned;
    }

    public double getHoursRequirement(){
        return this.hoursReq;
    }

    public double getTaskCountRequirement(){
        return this.taskCountReq;
    }
}

