package com.comp380.TaskRoulette;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.view.View;
import android.widget.Toast;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStreamReader;

/**
 * Created by ilia on 2/21/14.
 * TaskRoulette
 */

public class Menu extends Activity {
    private StoreObj strobj;
    private final String achievementFileName = "achievementList.txt";
    private final String rewardFileName = "rewardList.txt";
    private final String fileName = "StorageFile";
    private boolean doubleBackToMenuPressedOnce = false;
    //private Context context = getApplication();

    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);

        strobj = ((StoreObj) getApplication()); //getApplicationContext());

        strobj.getList();
    }

    public void onResume(){
        super.onResume();

        if (strobj.getUser() == null){
            readAchievements();
            readRewards();

            Intent intent = new Intent(Menu.this, CreateProfile.class);
            startActivity(intent);
        }
    }

    /*
    public void onPause(){
        super.onPause();
        strobj.storeList();
    }
    */

    public void onStop(){
        strobj.storeList();
        super.onStop();
    }

    /*
    public void onDestroy(){
        super.onDestroy();
        strobj.storeIDList();
    }
    */

    public void displayUser(View v){
        Intent intent = new Intent(Menu.this, Profile.class);
        startActivity(intent);
    }

    public void displayLists(View v){
        Intent intent = new Intent(Menu.this, List_Tasks.class);
        startActivity(intent);
    }

    public void displayRewards(View v){
        Intent intent = new Intent(Menu.this, Rewards.class);
        startActivity(intent);
    }

    public void displayAchievements(View v){
        Intent intent = new Intent(Menu.this, Achievements.class);
        startActivity(intent);
    }

    public void displaySpin(View v){
        Intent intent = new Intent(Menu.this, SpinnerActivity.class);
        startActivity(intent);
    }

    private void readAchievements(){
        try{
            BufferedReader reader = new BufferedReader(
                    new InputStreamReader(getAssets().open(achievementFileName)));
            String tempStr;

            while(reader.ready()){
                tempStr = reader.readLine();
                String[] strAry = tempStr.split(",");
                AchievementObj achievement = new AchievementObj(strAry[0], Double.valueOf(strAry[1]), Double.valueOf(strAry[2]),
                        false, strAry[3], strAry[4]);

                strobj.storeObject(achievement);
            }

            reader.close();
        }
        catch(Exception e){
            e.printStackTrace();
        }
    }

    private void readRewards(){
        BufferedReader reader;
        String read;
        String[] pulledData;

        try{
            reader = new BufferedReader(new InputStreamReader(getAssets().open(rewardFileName)));

            while ((read = reader.readLine()) != null){
                pulledData = read.split(",");

                RewardObj reward = new RewardObj(pulledData[0], pulledData[1], pulledData[2], false);
                strobj.storeObject(reward);
            }

            reader.close();
        }
        catch(Exception e){
            e.printStackTrace();
        }
    }

    /*
    // stores data in memory
    private void storeList(){
        try{
            FileOutputStream fOut = openFileOutput(fileName, Context.MODE_PRIVATE);
            fOut.write(strobj.storeListPrep().getBytes());
            fOut.close();
        }
        catch (Exception e){
            e.printStackTrace();
        }
    }

    // gets data from memory
    private void getList(){
        StringBuilder sb = new StringBuilder();
        BufferedReader reader;
        String read;

        try{
            FileInputStream fIn = openFileInput(fileName);
            reader = new BufferedReader(new InputStreamReader(fIn));
            while ((read = reader.readLine()) != null){
                sb.append(read);
            }
            //reader.close(); // dont think i need this
            fIn.close();

            strobj.getListPrep(sb.toString());
        }
        catch (Exception e){
            e.printStackTrace();
        }
    }
    */

    @Override
    public void onBackPressed(){
        if (doubleBackToMenuPressedOnce){
            strobj.storeList();
            Menu.this.finish();
            return;
        }

        this.doubleBackToMenuPressedOnce = true;
        Toast.makeText(this, "press BACK again to exit", Toast.LENGTH_SHORT).show();

        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
                doubleBackToMenuPressedOnce = false;
            }
        }, 2000);
    }
}