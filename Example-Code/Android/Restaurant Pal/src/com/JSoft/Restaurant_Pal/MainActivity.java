package com.JSoft.Restaurant_Pal;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;

public class MainActivity extends Activity {

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.rate_restaurant);

        //For testing purpose, just create a new intent here.

        Intent intent = new Intent(this,TipFinderActivity.class);
        startActivity(intent);







    }







}
