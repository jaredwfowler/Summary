package com.comp380.TaskRoulette;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.view.View;
import android.widget.*;

import java.util.ArrayList;

/**
 * Work added by Daniel on 3/22 - Present
 * TaskRoulette
 */

public class Rewards extends Activity {
    private StoreObj strobj;
    private RewardObj globalReward;
    private final String rewardHeader = "rewardHeader";
    private TextView Name;
    private TextView Cost;
    private ImageView image;
    private TextView chips;
    private Button btn_buy;
    private TextView emptyText;
    private UserObj user;
    private int pageCase = 0;

    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        strobj = (StoreObj) getApplicationContext();
        user = strobj.getUser();
        rewardView();
    }

    public void rewardView(){
        pageCase = -1;
        if (strobj.getListName(rewardHeader, false) != null){
            setContentView(R.layout.rewards);
            final ArrayAdapter<String> rewardNames = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_single_choice, strobj.getListName(rewardHeader, false));
            GridView rewardNamesGrid = (GridView) findViewById(R.id.rewards_grid);
            rewardNamesGrid.setAdapter(rewardNames);

            rewardNamesGrid.setOnItemClickListener(new AdapterView.OnItemClickListener() {
                @Override
                public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                    globalReward = strobj.getReward(searchRank(strobj.getListName(rewardHeader, false), rewardHeader, position));
                    selectedReward(view);
                }
            });
        }
        else{
            setContentView(R.layout.empty_textmsg);
            emptyText = (TextView) findViewById(R.id.empty_msg);
            emptyText.setText("There are no more rewards to buy");
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

    public void selectedReward(View v){
        pageCase = 1;
        setContentView(R.layout.reward_item_layout);

        Name = (TextView) findViewById(R.id.reward_name);
        Cost = (TextView) findViewById(R.id.reward_cost_amount);
        chips = (TextView) findViewById(R.id.user_chips_amount);
        image = (ImageView) findViewById(R.id.reward_image);
        btn_buy = (Button) findViewById(R.id.buy_reward);

        Name.setText(globalReward.getName());
        Cost.setText(globalReward.getCost());
        chips.setText(Integer.toString(user.getChips()));
        int resID = getResources().getIdentifier(globalReward.getImageID(), "drawable", getPackageName());
        image.setImageResource(resID);

        updateBuyButtonState();
    }

    public void buyReward(View v){
        globalReward.setEarned(true);
        user.deleteChips(Integer.parseInt(globalReward.getCost()));

        Toast.makeText(getApplicationContext(), "REWARD PURCHASED",Toast.LENGTH_LONG).show();

        rewardView();
    }

    private void updateBuyButtonState(){
        if (user.getChips() >= Integer.parseInt(globalReward.getCost())) btn_buy.setEnabled(true);
        else btn_buy.setEnabled(false);
    }

    @Override
    public void onBackPressed(){
        switch (pageCase){
            case 1: rewardView();
                break;
            default:
                super.onBackPressed();
                //Rewards.this.finish();
                break;
        }
        /*
        if (doubleBackToMenuPressedOnce){
            super.onBackPressed();
            return;
        }

        this.doubleBackToMenuPressedOnce = true;
        rewardView();
        Toast.makeText(this, "Please click BACk again to go to Menu", Toast.LENGTH_SHORT).show();

        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
                doubleBackToMenuPressedOnce = false;
            }
        }, 2000);
        */
    }
}