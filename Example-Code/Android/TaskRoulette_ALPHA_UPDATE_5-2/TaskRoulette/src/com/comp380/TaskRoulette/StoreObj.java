package com.comp380.TaskRoulette;

import android.app.Application;
import android.content.Context;
import com.google.gson.Gson;
import com.google.gson.reflect.TypeToken;
import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;

/**
 * Created with IntelliJ IDEA.
 * User: Ilia Benson
 * Date: 3/15/14
 * Time: 7:53 PM
 * TaskRoulette
 * Project:
 */

// stores the objects, all you need to do is keep track of id's and the object types of those id's

public class StoreObj extends Application{
    private final String fileName = "StorageFile";
    private ArrayList<TaskObj> taskList = new ArrayList<TaskObj>();
    private UserObj USER = null;
    private ArrayList<RewardObj> rewardList = new ArrayList<RewardObj>();
    private ArrayList<AchievementObj> achievementList = new ArrayList<AchievementObj>();
    private final String userHeader = "userHeader,";
    private final String taskHeader = "taskHeader,";
    private final String rewardHeader = "rewardHeader";
    private final String achievementHeader = "achievementHeader";
    private final String endOfLine = "\u00B6";

    // defualt
    public StoreObj(){

    }

    // stores a TaskObj object
    public void storeObject(TaskObj task){
        for (int i = 0; i < taskList.size(); i++){
            if (taskList.get(i).getTimeStamp() == (task.getTimeStamp())){
                taskList.remove(i);
            }
        }

        taskList.add(task);
    }

    // stores UserObj object
    public void storeObject(UserObj user){
        USER = user;
    }

    // stores RewardObj object
    public void storeObject(RewardObj reward){
        rewardList.add(reward);
    }

    // stores AchievementObj object
    public void storeObject(AchievementObj achievement){
        achievementList.add(achievement);
    }

    // removes object given index and header
    public void removeObject(int index, String Header){
        if (Header.equals(taskHeader)){
            taskList.remove(index);
        }

        if (Header.equals(userHeader)){
            USER = null;
        }

        if (Header.equals(rewardHeader)){
            rewardList.remove(index);
        }

        if (Header.equals(achievementHeader)){
            achievementList.remove(index);
        }
    }

    // removes task object if it exists
    public boolean removeObject(TaskObj task){
        if (taskList.contains(task)){
            taskList.remove(task);
            return true;
        }

        return false;
    }

    // removes user object if it exists
    public boolean removeObject(UserObj user){
        if (USER != null){
            USER = null;
            return true;
        }

        return false;
    }

    // removes reward object if it exists
    public boolean removeObject(RewardObj reward){
        if (rewardList.contains(reward)){
            rewardList.remove(reward);
            return true;
        }

        return false;
    }
    // removes achievement object if it exists
    public boolean removeObject(AchievementObj achievement){
        if (achievementList.contains(achievement)){
            achievementList.remove(achievement);
            return true;
        }

        return false;
    }

    // returns UserObj
    public UserObj getUser(){
        return USER;
    }

    // returns a TaskObj object
    public TaskObj getTask(int index){
        return taskList.get(index);
    }
    
    // returns a RewardObj
    public RewardObj getReward(int index){
        return rewardList.get(index);
    }
    
    // returns a AchievementObj
    public AchievementObj getAchievement(int index){
        return achievementList.get(index);
    }

    // helper method
    // makes a string of all ID lists
    private String makeIDList(ArrayList List){
        Gson gson = new Gson();
        return gson.toJson(List);
    }

    // helper method
    // makes a string of UserObj USER
    private String makeIDList(){
        Gson gson = new Gson();
        return gson.toJson(USER);
    }

    /*
    public void storeListSD(){

    }
    */

    // stores the lists of all the objects
    // must be called when activity goes onto onPause()
    // need to make sure that when it stores it old version is overwritten
    public void storeList(){
        String list = makeIDList() + endOfLine + makeIDList(taskList) + endOfLine  + makeIDList(rewardList) + endOfLine  + makeIDList(achievementList);

        try{
            //FileOutputStream fOut = fileContext.openFileOutput(fileName, Context.MODE_PRIVATE);
            FileOutputStream fOut = openFileOutput(fileName, Context.MODE_PRIVATE);
            fOut.write(list.getBytes());
            fOut.close();
        }
        catch (Exception e){
            e.printStackTrace();
        }
    }

    /*
    // trying something new
    public String storeListPrep(){
        return makeIDList() + endOfLine + makeIDList(taskList) + endOfLine  + makeIDList(rewardList) + endOfLine  + makeIDList(achievementList);
    }

    private void getListStorageSD(){

    }
    */

    // from storage, this is called on app start only
    public void getList(){
        StringBuilder sb = new StringBuilder();
        BufferedReader reader;
        String read;
        String[] pulledData;
        Gson gson = new Gson();

        try{
            //FileInputStream fIn = fileContext.openFileInput(fileName);
            FileInputStream fIn = openFileInput(fileName);
            reader = new BufferedReader(new InputStreamReader(fIn));
            while ((read = reader.readLine()) != null){
                sb.append(read);
            }
            reader.close(); // dont think i need this
            fIn.close();

            read = sb.toString();

            pulledData = read.split(endOfLine);

            // also need to make sure only 4 entries at all time, if more or less, something is wrong
            USER = gson.fromJson(pulledData[0],new TypeToken<List<UserObj>>(){}.getType());
            taskList = gson.fromJson(pulledData[1], new TypeToken<List<ArrayList<TaskObj>>>(){}.getType());
            rewardList = gson.fromJson(pulledData[2], new TypeToken<List<ArrayList<RewardObj>>>(){}.getType());
            achievementList = gson.fromJson(pulledData[3], new TypeToken<List<ArrayList<AchievementObj>>>(){}.getType());
        }
        catch (Exception e){
            e.printStackTrace();
        }
    }

    /*
    // trying something new
    public void getListPrep(String read){
        String[] pulledData;
        Gson gson = new Gson();

        try{
            pulledData = read.split(endOfLine);

            // also need to make sure only 4 entries at all time, if more or less, something is wrong
            USER = gson.fromJson(pulledData[0],new TypeToken<List<UserObj>>(){}.getType());
            taskList = gson.fromJson(pulledData[1], new TypeToken<List<ArrayList<TaskObj>>>(){}.getType());
            rewardList = gson.fromJson(pulledData[2], new TypeToken<List<ArrayList<RewardObj>>>(){}.getType());
            achievementList = gson.fromJson(pulledData[3], new TypeToken<List<ArrayList<AchievementObj>>>(){}.getType());
        }
        catch (Exception e){
            e.printStackTrace();
        }
    }
    */

    // returns List of Names given a Header
    // from object instance
    public String[] getListName(String Header){
        String[] Names = new String[1];

        if (Header.equals(userHeader)){
            Names = new String[1];
            Names[0] = USER.getName();
        }

        if (Header.equals(taskHeader)){
            Names = new String[taskList.size()];

            for (int i = 0; i < Names.length; i++){
                Names[i] = taskList.get(i).getName();
            }
        }

        if (Header.equals(rewardHeader)){
            Names = new String[rewardList.size()];

            for (int i = 0; i < Names.length; i++){
                Names[i] = rewardList.get(i).getName();
            }
        }

        if (Header.equals(achievementHeader)){
            Names = new String[achievementList.size()];

            for (int i = 0; i < Names.length; i++){
                Names[i] = achievementList.get(i).getName();
            }
        }

        return Names;
    }

    // these are all extra methods that we might need but arent being used now

    // returns list of achievements or rewards that are either earned (true) or not earned (false)
    // if user or task is requested, returns list of names of those regardless of boolean value
    public String[] getListName(String Header, boolean earned){
        ArrayList<String> Names = new ArrayList<String>();

        if (Header.equals(userHeader) || Header.equals(taskHeader)){
            return getListName(Header);
        }

        if (Header.equals(rewardHeader)){
            for (int i = 0; i < rewardList.size(); i++){
                if (rewardList.get(i).getEarned() == earned){
                    Names.add(rewardList.get(i).getName());
                }
            }
        }

        if (Header.equals(achievementHeader)){
            for (int i = 0; i < achievementList.size(); i++){
                if (achievementList.get(i).getEarned() == earned){
                    Names.add(achievementList.get(i).getName());
                }
            }
        }

        if (Names.size() == 0){
            return null;
        }

        return Names.toArray(new String[Names.size()]);
    }

    public int spinnerMath(int bet, boolean winNumber, boolean winColor){
        if (winNumber){
            return bet * 36;
        }

        if (winColor){
            return bet * 2;
        }

        return 0;
    }
}
