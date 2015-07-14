package com.comp380.TaskRoulette;

/**
 * Work added by Natalia on 3/22 - Present
 * TaskRoulette
 */

public class UserObj {
    private String Name = "";
    private String Age = "";
    private String Sex = "";
    private long TimeStamp;
    private int chips = 0;
    private int hours = 0;
    private int minutes = 0;
    private int taskCount = 0;
    private int completedTaskCount = 0;

    // default constructor
    public UserObj(){
        TimeStamp = System.currentTimeMillis();
    }

    public UserObj(String Name, String Age, String Sex){
        this.Name = Name;
        this.Age = Age;
        this.Sex = Sex;
        this.chips = 0;
        this.taskCount = 0;
        this.completedTaskCount = 0;
        TimeStamp = System.currentTimeMillis();
    }

    public void setName(String Name){
        this.Name = Name;
    }

    public String getName(){
        return Name;
    }

    public void setAge(String Age){
        this.Age = Age;
    }

    public String getAge(){
        return Age;
    }

    public void setSex(String Sex){
        this.Sex = Sex;
    }

    public String getSex(){
        return Sex;
    }

    public String getInfo(){
        return "Name: " + Name + "\nAge: " + Age + "\nSex: " + Sex + "\nChips: " + chips + "\nCompleted Tasks: " + completedTaskCount + "\nCurrent Tasks: "
                + taskCount + "\nTotal Hours Completed: " + hours;
    }

    // used for storing (unique names always guaranteed) and if 2 instances exist with same Name, u can keep the newer or older version
    public long getTimeStamp(){
        return TimeStamp;
    }

    public int getHours(){
        return hours;
    }

    public void addMinutes(int minutes){
        this.minutes += minutes;

        while (this.minutes >= 60){
            this.minutes -= 60;
            hours++;
            chips++;
        }
    }

    // Chips methods
    public int getChips(){
        return chips;
    }

    public void addChips(int x){
        chips += x;
        hours += x;
    }

    public void deleteChips(int x){
        chips -= x;
    }

    // Task count methods
    public void incrementTaskCount(){
        taskCount++;
    }

    // Completed task methods
    public int getCompletedTaskCount(){
        return completedTaskCount;
    }

    public void incrementCompletedTaskCount(){
        completedTaskCount++;
        taskCount--;
    }
}
