package com.comp380.TaskRoulette;

import android.app.Activity;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.widget.*;
import android.view.View;

/**
 * Created by: Jared Fowler
 * Date: March 27, 2014
 */

public class Achievements extends Activity {

    private StoreObj strobj;
    private AchievementObj globalAchievement;
    private final String achievementHeader = "achievementHeader";
    private TextView AchievementName;
    private TextView AchievementEarned;
    private TextView AchievementDescription;
    private ImageView AchievementImage;
    private TextView emptyText;
    private int pageCase = 0;

    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        strobj = ((StoreObj) getApplicationContext());
        setAchievementView();
    }

    public void setAchievementView(){ // creates a clickable list of names of achievements
        pageCase = -1;
        if (strobj.getListName(achievementHeader, false) != null){
            setContentView(R.layout.achievements);
            final ArrayAdapter<String> AchievementNames = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_single_choice, strobj.getListName(achievementHeader, false));
            GridView achievementNameGrid = (GridView) findViewById(R.id.achievements_grid);
            achievementNameGrid.setAdapter(AchievementNames);
            Log.i("AchievementsMessage1", String.valueOf(AchievementNames.getCount()));

            achievementNameGrid.setOnItemClickListener(new AdapterView.OnItemClickListener() { // cant use xml onclick for GridView so have to use this one
                @Override
                public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                    globalAchievement = strobj.getAchievement(searchRank(strobj.getListName(achievementHeader, false), achievementHeader, position));
                    selectedAchievement(view);
                }
            });
        }
        else{
            setContentView(R.layout.empty_textmsg);
            emptyText = (TextView) findViewById(R.id.empty_msg);
            emptyText.setText("There are no more achievements to earn");
        }
    }

    // helper method, fixes miss-match bug
    private int searchRank(String[] Names, String Header, int position){
        String[] rewards = strobj.getListName(Header);

        for (int i = 0; i < rewards.length; i++){
            if (rewards[i].equals(Names[position])){
                return i;
            }
        }

        return 0;
    }

    public void selectedAchievement(View v){
        pageCase = 1;
        setContentView(R.layout.achievement_item_layout);

        AchievementName = (TextView)findViewById(R.id.achievement_name);
        AchievementEarned = (TextView)findViewById(R.id.achievements_yesorno);
        AchievementDescription = (TextView)findViewById(R.id.achievements_description);
        AchievementImage = (ImageView)findViewById(R.id.achievement_image);

        try {
            AchievementName.setText(globalAchievement.getName());
        }
        catch(Exception e){
            Log.i("AchievementsError1", "NULL Value Name");
        }
        try {
            AchievementDescription.setText(globalAchievement.getDescription());
        }
        catch(Exception e){
            Log.i("AchievementsError1", "NULL Value Description");
        }
        try {
            if (globalAchievement.getEarned()) {
                AchievementEarned.setText("YES");
                AchievementEarned.setTextColor(Color.GREEN);
            } else {
                AchievementEarned.setText("NO");
                AchievementEarned.setTextColor(Color.RED);
            }
        }
        catch(Exception e){
            Log.i("AchievementsError1", "NULL Value Earned");
        }
        try{
            int resID = getResources().getIdentifier(globalAchievement.getImageName(), "drawable", getPackageName());
            AchievementImage.setImageResource(resID);
        }
        catch(Exception e){
            Log.i("AchievementsError1", "NULL Value Image");
        }
    }

    @Override
    public void onBackPressed(){
        switch (pageCase){
            case 1: setAchievementView();
                break;
            default:
                super.onBackPressed();
                //Achievements.this.finish();
                break;
        }
        /*
        if (doubleBackToMenuPressedOnce){
            super.onBackPressed();
            return;
        }

        this.doubleBackToMenuPressedOnce = true;
        setAchievementView();
        Toast.makeText(this, "Please click BACK again to go to Menu", Toast.LENGTH_SHORT).show();

        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
                doubleBackToMenuPressedOnce = false;
            }
        }, 2000);
        */
    }
}