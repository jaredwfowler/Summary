package com.JSoft.Restaurant_Pal;

import android.app.Activity;
import android.graphics.Color;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

/**
 * Created by Jared on 5/5/2014.
 *
 * This class handles various content views from getting the initial tip, to tip deductions.  This activity
 * will give the user the opportunity to enter into the rate and calculate final tip and bill activities.
 *
 */
public class TipFinderActivity extends Activity {

    //Constants
    private int MAX_DIGIT = 9;
    private int MIN_DIGIT = 0;


    //Handles and Variables
    private Tip theTip;

    //tip_finder_initialinput handles
    private TextView tvTens;
    private TextView tvOnes;
    private TextView tvTenths;
    private TextView tvDecimal;
    private TextView tvPercentage;

    //tip_finder_idle handles
    private TextView tvTipIdle;




    //------------------------------------------------------------------------------------------------------------------
    public void onCreate(Bundle savedInstanceState){
        super.onCreate(savedInstanceState);

        //We begin this activity by loading the values for the first content view
        setContentView(R.layout.tip_finder_initialinput);
        setHandles_initialInput();

        //We want to create a tip object that will be used throughout the course of this activity.
        theTip = new Tip();   //We will use the default values

        //We will also set the starting tip to what is defined by our tip object, in this case, the default tip.
        setInitialTip();
        //Set the appropriate color
        colorChanger();


    }

    //------------------------------------------------------------------------------------------------------------------
    //Content View 1 Handles and Listeners and other helper functions
    //------------------------------------------------------------------------------------------------------------------
    private void setHandles_initialInput(){
        /*
        Sets the handles for the content view "tip_finder_initialinput"
        */

        tvTens = (TextView)findViewById(R.id.textView_tensPlace);
        tvOnes = (TextView)findViewById(R.id.textView_onesPlace);
        tvTenths = (TextView)findViewById(R.id.textView_tenthsPlace);
        tvDecimal = (TextView)findViewById(R.id.textView_decimal);
        tvPercentage = (TextView)findViewById(R.id.textView_percentage);

    }

    public void button_start_listener(View v){
        /*
        This button will lead us into our next content view.  Before leaving this view, we should update our tip to
        match the current values given by the user.
        */

        //Update the tip into the tip object
        theTip.setStartTip(getTip());
        theTip.setTip(getTip());

        //Call method to set up the next content
        startContent2();
    }

    public void button_up_tenths_listener(View v){
        if(Integer.valueOf(tvTenths.getText().toString()) == MAX_DIGIT)
            tvTenths.setText(String.valueOf(MIN_DIGIT));
        else
            tvTenths.setText(String.valueOf(Integer.valueOf(tvTenths.getText().toString()) + 1));

        colorChanger();
    }

    public void button_up_tens_listener(View v){
        if(Integer.valueOf(tvTens.getText().toString()) == MAX_DIGIT)
            tvTens.setText(String.valueOf(MIN_DIGIT));
        else
            tvTens.setText(String.valueOf(Integer.valueOf(tvTens.getText().toString()) + 1));

        colorChanger();
    }

    public void button_up_ones_listener(View v){
        if(Integer.valueOf(tvOnes.getText().toString()) == MAX_DIGIT)
            tvOnes.setText(String.valueOf(MIN_DIGIT));
        else
            tvOnes.setText(String.valueOf(Integer.valueOf(tvOnes.getText().toString()) + 1));

        colorChanger();
    }

    public void button_down_tenths_listener(View v){
        if(Integer.valueOf(tvTenths.getText().toString()) == MIN_DIGIT)
            tvTenths.setText(String.valueOf(MAX_DIGIT));
        else
            tvTenths.setText(String.valueOf(Integer.valueOf(tvTenths.getText().toString()) - 1));

        colorChanger();
    }

    public void button_down_tens_listener(View v){
        if(Integer.valueOf(tvTens.getText().toString()) == MIN_DIGIT)
            tvTens.setText(String.valueOf(MAX_DIGIT));
        else
            tvTens.setText(String.valueOf(Integer.valueOf(tvTens.getText().toString()) - 1));

        colorChanger();
    }

    public void button_down_ones_listener(View v){
        if(Integer.valueOf(tvOnes.getText().toString()) == MIN_DIGIT)
            tvOnes.setText(String.valueOf(MAX_DIGIT));
        else
            tvOnes.setText(String.valueOf(Integer.valueOf(tvOnes.getText().toString()) - 1));

        colorChanger();
    }

    private void setInitialTip(){ //Sets the GUI tip.  Takes a double and breaks it down to tens, ones, and tenths

        double tempDouble = theTip.getStartTip();
        int tempInt = (int)tempDouble;  //We downcast on purpose in order to get rid of the decimal!
        //set the tens place
        if(tempInt/10 > MAX_DIGIT)
            tvTens.setText(String.valueOf(MAX_DIGIT));
        else
            tvTens.setText(String.valueOf(tempInt/10));
        //set the ones place
        tvOnes.setText(String.valueOf((tempInt - 10 *(tempInt/10))));
        //set the tenths place
        tvTenths.setText(String.valueOf(((int)(10*(tempDouble-tempInt)))));
    }

    private double getTip(){ //Converts the GUI Tip into a double and returns it
        return ((Double.valueOf(tvTens.getText().toString())) * 10) +
                (Double.valueOf(tvOnes.getText().toString())) +
                ((Double.valueOf(tvTenths.getText().toString())) / 10);
    }

    private void colorChanger(){  //Changes the tip color depending on certain values
        //get the current tip in double format
        double currentTip = getTip();
        int color;
        //we test to see if we need to change the color
        if(currentTip > theTip.DEFAULT_TIP) //Above default tip color
            color = theTip.ABOVE_DEF_COLOR;
        else if(currentTip < theTip.DEFAULT_TIP) //Below default tip color
            color = theTip.BELOW_DEF_COLOR;
        else
            color = theTip.DEFAULT_COLOR;

        //We finish by coloring the text views to the appropriate color
        tvPercentage.setTextColor(color);
        tvDecimal.setTextColor(color);
        tvTens.setTextColor(color);
        tvOnes.setTextColor(color);
        tvTenths.setTextColor(color);
    }

    //------------------------------------------------------------------------------------------------------------------
    //Content View 2 Handles and Listeners and other helper functions
    //------------------------------------------------------------------------------------------------------------------
    private void startContent2(){//Takes the necessary steps to load the second content menu
        //Change the view
        this.setContentView(R.layout.tip_finder_idle);
        //Set the new Handles
        setHandles_idle();

        //Set the correct Tip value
        tvTipIdle.setText(String.valueOf(theTip.getTip()) + "%");
        tvTipIdle.setTextColor(theTip.getTipColor());

    }

    private void setHandles_idle(){
        tvTipIdle = (TextView)findViewById(R.id.textView_tip);
    }
















}
