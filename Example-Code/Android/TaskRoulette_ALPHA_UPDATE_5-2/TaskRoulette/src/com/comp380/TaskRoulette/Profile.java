package com.comp380.TaskRoulette;

import android.app.Activity;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.View;
import android.widget.*;

/**
 * Work added by Natalia on 3/22 - Present
 * TaskRoulette
 */

public class Profile extends Activity {
    private EditText Name;
    private EditText Age;
    private EditText Sex;
    private Button btn;
    private UserObj user;
    private RewardObj globalReward;
    private AchievementObj globalAchievement;
    private StoreObj strobj;
    private TextView info;
    private TextView emptyText;
    private final String rewardHeader = "rewardHeader";
    private final String achievementHeader = "achievementHeader";
    private int pageCase = 0;

    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        strobj = ((StoreObj) getApplicationContext());
        user = strobj.getUser();
        displayInfo();
    }

    private void displayInfo(){ // need to add avatars functionality
        pageCase = -1;
        setContentView(R.layout.profile);

        info = (TextView) findViewById(R.id.textView1);
        info.setTextSize(26);
        info.setText(user.getInfo());
    }

    public void editProfile(View v){
        pageCase = 1;
        setContentView(R.layout.edit_profile);

        Name = (EditText) findViewById(R.id.editname);
        Name.setText(user.getName());

        Age = (EditText) findViewById(R.id.editage);
        Age.setText(user.getAge());

        Sex = (EditText) findViewById(R.id.editsex);
        Sex.setText(user.getSex());

        btn = (Button) findViewById(R.id.ok);


        TextWatcher watcher = new LocalTextWatcher();
        Name.addTextChangedListener(watcher);
        Age.addTextChangedListener(watcher);
        Sex.addTextChangedListener(watcher);

        updateButtonState();
    }

    public void viewRewards(View v){
        pageCase = 1;
        if (strobj.getListName(rewardHeader,true) != null){
            setContentView(R.layout.rewards);
            ArrayAdapter<String> rewardNames = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_single_choice, strobj.getListName(rewardHeader, true));
            GridView rewardNamesGrid = (GridView) findViewById(R.id.rewards_grid);
            rewardNamesGrid.setAdapter(rewardNames);

            rewardNamesGrid.setOnItemClickListener(new AdapterView.OnItemClickListener() {
                @Override
                public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                    globalReward = strobj.getReward(searchRank(strobj.getListName(rewardHeader, true), rewardHeader, position));
                    selectedReward(view);
                }
            });
        }
        else{
            setContentView(R.layout.empty_textmsg);
            emptyText = (TextView) findViewById(R.id.empty_msg);
            emptyText.setText("You currently have not purchased any rewards");
        }
    }

    public void selectedReward(View v){ // this will change. need to select avatars
        pageCase = 2;
        setContentView(R.layout.profile_reward_item_layout);

        TextView Name = (TextView) findViewById(R.id.reward_name);
        TextView Cost = (TextView) findViewById(R.id.reward_cost_amount);
        ImageView image = (ImageView) findViewById(R.id.reward_image);

        Name.setText(globalReward.getName());
        Cost.setText(globalReward.getCost());
        int resID = getResources().getIdentifier(globalReward.getImageID(), "drawable", getPackageName());
        image.setImageResource(resID);
    }

    public void viewAchievements(View v){
        pageCase = 1;
        if (strobj.getListName(achievementHeader,true) != null){
            setContentView(R.layout.achievements);
            ArrayAdapter<String> AchievementNames = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_single_choice, strobj.getListName(achievementHeader, true));
            GridView achievementNameGrid = (GridView) findViewById(R.id.achievements_grid);
            achievementNameGrid.setAdapter(AchievementNames);

            achievementNameGrid.setOnItemClickListener(new AdapterView.OnItemClickListener() { // cant use xml onclick for GridView so have to use this one
                @Override
                public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                    globalAchievement = strobj.getAchievement(searchRank(strobj.getListName(achievementHeader, true), achievementHeader, position));
                    selectedAchievement(view);
                }
            });
        }
        else{
            setContentView(R.layout.empty_textmsg);
            emptyText = (TextView) findViewById(R.id.empty_msg);
            emptyText.setText("You currently have not earned any achievements");
        }
    }

    public void selectedAchievement(View v){
        pageCase = 3;
        setContentView(R.layout.achievement_item_layout);

        TextView AchievementName = (TextView)findViewById(R.id.achievement_name);
        TextView AchievementEarned = (TextView)findViewById(R.id.achievements_yesorno);
        TextView AchievementDescription = (TextView)findViewById(R.id.achievements_description);
        ImageView AchievementImage = (ImageView)findViewById(R.id.achievement_image);

        AchievementName.setText(globalAchievement.getName());
        AchievementDescription.setText(globalAchievement.getDescription());

        if (globalAchievement.getEarned()) {
            AchievementEarned.setText("YES");
            AchievementEarned.setTextColor(Color.GREEN);
        } else {
            AchievementEarned.setText("NO");
            AchievementEarned.setTextColor(Color.RED);
        }

        int resID = getResources().getIdentifier(globalAchievement.getImageName(), "drawable", getPackageName());
        AchievementImage.setImageResource(resID);
    }

    private boolean EditTextEmpty(EditText edit){
        return edit.getText().length() == 0;
    }

    public void updateButtonState(){
        if (EditTextEmpty(Name) || EditTextEmpty(Age) || EditTextEmpty(Sex)) btn.setEnabled(false);
        else btn.setEnabled(true);
    }

    public class LocalTextWatcher implements TextWatcher {
        public void afterTextChanged(Editable s) {
            updateButtonState();
        }

        public void beforeTextChanged(CharSequence s, int start, int count, int after) {
        }

        public void onTextChanged(CharSequence s, int start, int before, int count) {
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

    public void makeProfile(View v){
        Name = (EditText) findViewById(R.id.editname);
        Age = (EditText) findViewById(R.id.editage);
        Sex = (EditText) findViewById(R.id.editsex);

        user.setName(Name.getText().toString());
        user.setAge(Age.getText().toString());
        user.setSex(Sex.getText().toString());

        displayInfo();
    }

    @Override
    public void onBackPressed(){
        switch (pageCase){
            case 1: displayInfo();
                break;
            case 2: viewRewards(findViewById(R.id.rewards));
                break;
            case 3: viewAchievements(findViewById(R.id.achievements));
                break;
            default:
                super.onBackPressed();
                //Profile.this.finish();
                break;
        }
        /*
        if (doubleBackToMenuPressedOnce){
            super.onBackPressed();
            return;
        }

        this.doubleBackToMenuPressedOnce = true;
        displayInfo();
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