package com.comp380.TaskRoulette;

import android.app.Activity;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

/**
 * Work added by Natalia on 3/22 - Present
 * TaskRoulette
 */
public class CreateProfile extends Activity {
    public EditText Name;
    public EditText Age;
    public EditText Sex;
    public Button btn;
    private StoreObj strobj;

    // need to add something here so that you cant proceed until at least one profile is created.

    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.create_profile);

        strobj = ((StoreObj) getApplicationContext());

        Name = (EditText) findViewById(R.id.name);
        Age = (EditText) findViewById(R.id.age);
        Sex = (EditText) findViewById(R.id.sex);
        btn = (Button) findViewById(R.id.ok);

        TextWatcher watcher = new LocalTextWatcher();
        Name.addTextChangedListener(watcher);
        Age.addTextChangedListener(watcher);
        Sex.addTextChangedListener(watcher);

        updateButtonState();
    }

    private boolean EditTextEmpty(EditText edit){
        return edit.getText().length() == 0;
    }

    public void updateButtonState(){
        // HOW THE FUCK.... SHOULD BE AND BUT ONLY WORKS WITH OR.... OH ANDROID U SO ASS BACKWARDS....
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

    public void makeProfile(View v){
        UserObj user = new UserObj();
        Name = (EditText) findViewById(R.id.name);
        Age = (EditText) findViewById(R.id.age);
        Sex = (EditText) findViewById(R.id.sex);

        user.setName(Name.getText().toString());
        user.setAge(Age.getText().toString());
        user.setSex(Sex.getText().toString());

        strobj.storeObject(user);

        CreateProfile.this.finish();
    }
}