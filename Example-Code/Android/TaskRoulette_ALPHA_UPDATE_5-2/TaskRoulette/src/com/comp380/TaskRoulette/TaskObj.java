package com.comp380.TaskRoulette;

import android.text.format.Time;

/**
 * Created with IntelliJ IDEA.
 * User: Ilia Benson
 * Date: 3/15/14
 * Time: 6:18 PM
 * TaskRoulette
 * Project:
 */

public class TaskObj {
    private String Name = "";
    private String Description = "";
    private int Hour = 0;
    private int Min = 0;
    private long TimeStamp;

    // default constructor
    public TaskObj(){
        TimeStamp = System.currentTimeMillis();
    }

    public TaskObj(String Name, String Description, int Hour, int Min){
        this.Name = Name;
        this.Description = Description;
        this.Hour = Hour;
        this.Min = Min;
        TimeStamp = System.currentTimeMillis();
    }

    public void setName(String Name){
        this.Name = Name;
    }

    public String getName(){
        return Name;
    }

    public void setDescription(String Description){
        this.Description = Description;
    }

    public String getDescription(){
        return Description;
    }

    public void setTime(int Hour, int Min){
        this.Hour = Hour;
        this.Min = Min;
    }

    public int getHour(){
        return Hour;
    }

    public int getMin(){
        return Min;
    }

    // used for storing (unique names always guaranteed) and if 2 instances exist with same Name, u can keep the newer or older version
    public long getTimeStamp(){
        return TimeStamp;
    }
}
