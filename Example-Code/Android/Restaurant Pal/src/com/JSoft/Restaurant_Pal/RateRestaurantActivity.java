package com.JSoft.Restaurant_Pal;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.RatingBar;

/**
 * Created by Jared on 4/11/2014.
 *
 * The purpose of this activity it to collect survey information from the user and then to
 * transmit it to my database.  The information will be handled by a php handler.
 *
 */


public class RateRestaurantActivity extends Activity {

    //Constants
    final String InsertCommandBegin = "INSERT INTO TABLE RestaurantPal (rName, rAddress, wName, rCleanliness, rFoodQuality, rAtmosphere, rLocation, wPersonality, wApparel, wService, wAccuracy, commentary) VALUES ";
    final int COMMENTARY_MAX = 200;
    final int RESTAURANT_NAME_MAX = 30;
    final int WAITER_NAME_MAX = 30;

    //------------------------------------------------------------------------------------------------------------------
    //Handles and Variables
    private RatingBar barCleanliness;
    private RatingBar barFoodQuality;
    private RatingBar barAtmosphere;
    private RatingBar barLocation;
    private RatingBar barPersonality;
    private RatingBar barApparel;
    private RatingBar barService;
    private RatingBar barAccuracy;
    private EditText editTextCommentary;
    private Button buttonSubmit;

    private String commentary = null;


    //------------------------------------------------------------------------------------------------------------------
    public void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);

        //Set the handles
        barCleanliness = (RatingBar)(findViewById(R.id.ratingBar1));
        barFoodQuality = (RatingBar)(findViewById(R.id.ratingBar2));
        barAtmosphere = (RatingBar)(findViewById(R.id.ratingBar3));
        barLocation = (RatingBar)(findViewById(R.id.ratingBar4));
        barPersonality = (RatingBar)(findViewById(R.id.ratingBar5));
        barApparel = (RatingBar)(findViewById(R.id.ratingBar6));
        barService = (RatingBar)(findViewById(R.id.ratingBar7));
        barAccuracy = (RatingBar)(findViewById(R.id.ratingBar8));
        editTextCommentary = (EditText)(findViewById(R.id.editText));
        buttonSubmit = (Button)(findViewById(R.id.button));

    }

    //------------------------------------------------------------------------------------------------------------------
    void Button_Submit_Click(View v){
        /*
        Upon selection, we now need to gather the information, connect to the database and submit the information.
         */

        //Most of the information is contained in the objects themselves such as the rating bars.  We do need to, however,
        //limit the length of the commentary and the Restaurant name and waiter name before submitting them to the database.

        //get the commentary string
        commentary = editTextCommentary.getText().toString();

        //Ensure that commentary does not exceed the maximum number of chars
        if(commentary.length() > COMMENTARY_MAX)
            commentary = commentary.substring(0,COMMENTARY_MAX - 1);






    }

}
