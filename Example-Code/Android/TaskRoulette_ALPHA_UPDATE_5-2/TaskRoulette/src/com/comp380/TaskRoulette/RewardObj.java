package com.comp380.TaskRoulette;

/**
 * Work added by Daniel on 3/22 - Present
 * TaskRoulette
 */

public class RewardObj {
    private String Name = "";
    private boolean Earned = false;
    private String Cost = "";
    private String imageID = "";

    public RewardObj(){

    }

    public RewardObj(String Name, String Cost, String imageID, boolean Earned){
        this.Name = Name;
        this.Earned = Earned;
        this.Cost = Cost.trim();
        this.imageID = imageID;
    }

    public String getName(){
        return Name;
    }

    public boolean getEarned(){
        return Earned;
    }

    public void setEarned(boolean status){
        Earned = status;
    }

    public String getCost(){
        return Cost;
    }

    public String getImageID(){
        return imageID;
    }
}
