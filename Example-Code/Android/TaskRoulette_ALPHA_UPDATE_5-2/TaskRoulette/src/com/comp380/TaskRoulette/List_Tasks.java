package com.comp380.TaskRoulette;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.View;
import android.widget.*;

/**
 * Created by ilia on 2/21/14.
 * TaskRoulette
 */

public class List_Tasks extends Activity {
    private EditText TaskName;
    private EditText TaskDescription;
    private EditText TaskHour;
    private EditText TaskMinute;
    private Button btn_add_task;
    private Button btn_edit_task;
    private final String taskHeader = "taskHeader,";
    private final String achievementHeader = "achievementHeader";
    private StoreObj strobj;
    private TaskObj globalTask;
    private UserObj user;
    private TextWatcher watcher;
    private int pageCase = 0;

    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        strobj = ((StoreObj) getApplicationContext());

        user = strobj.getUser();
    }

    public void onResume(){
        super.onResume();
        updateTaskView();
    }

    public void addToList(View v){
        pageCase = 1;
        setContentView(R.layout.lists_add_task);

        updateEditTexts(); // need to call every time i switch out layout
        btn_add_task = (Button) findViewById(R.id.add_task);

        watcher = new LocalTextWatcher();
        TaskName.addTextChangedListener(watcher);
        TaskDescription.addTextChangedListener(watcher);
        TaskHour.addTextChangedListener(watcher);
        TaskMinute.addTextChangedListener(watcher);

        updateAddToListButtonState();
    }

    public void addTask(View v){
        TaskObj task = new TaskObj();

        task.setName(TaskName.getText().toString());
        task.setDescription(TaskDescription.getText().toString());
        task.setTime(Integer.parseInt(TaskHour.getText().toString().trim()), Integer.parseInt(TaskMinute.getText().toString().trim()));

        strobj.storeObject(task);
        user.incrementTaskCount();

        updateTaskView();
    }

    private void selectedTask(View v){
        pageCase = 1;
        setContentView(R.layout.lists_edit_task);

        updateEditTexts(); // need to call every time i switch out layout
        TaskName.setText(globalTask.getName());
        TaskDescription.setText(globalTask.getDescription());
        TaskHour.setText(Integer.toString(globalTask.getHour()));
        TaskMinute.setText(Integer.toString(globalTask.getMin()));

        btn_edit_task = (Button) findViewById(R.id.edit_from_list);

        watcher = new LocalTextWatcher2();
        TaskName.addTextChangedListener(watcher);
        TaskDescription.addTextChangedListener(watcher);
        TaskHour.addTextChangedListener(watcher);
        TaskMinute.addTextChangedListener(watcher);

        updateEditFromListButtonState();
    }

    public void editTask(View v){
        globalTask.setName(TaskName.getText().toString());
        globalTask.setDescription(TaskDescription.getText().toString());
        globalTask.setTime(Integer.parseInt(TaskHour.getText().toString().trim()), Integer.parseInt(TaskMinute.getText().toString().trim()));

        strobj.storeObject(globalTask);

        updateTaskView();
    }

    public void deleteTask(View v){
        strobj.removeObject(globalTask);

        updateTaskView();
    }

    public void completeTask(View v){
        user.incrementCompletedTaskCount();
        user.addChips(globalTask.getHour());
        user.addMinutes(globalTask.getMin());
        strobj.removeObject(globalTask);

        checkAchievements();

        updateTaskView();
    }

    private void checkAchievements(){
        if (strobj.getListName(achievementHeader, false) != null){
            String[] achievs = strobj.getListName(achievementHeader, false);
            AchievementObj temp;

            for (int i = 0; i < achievs.length; i++){
                temp = strobj.getAchievement(i);
                if ((user.getCompletedTaskCount() >= temp.getTaskCountRequirement()) && (temp.getHoursRequirement() == -1) && !temp.getEarned()){
                    temp.setEarned(true);
                    Toast.makeText(getApplicationContext(), "EARNED ACHIEVEMENT " + temp.getName(),Toast.LENGTH_LONG).show();
                }

                if ((user.getHours() >= temp.getHoursRequirement()) && (temp.getTaskCountRequirement() == -1) && !temp.getEarned()){
                    temp.setEarned(true);
                    Toast.makeText(getApplicationContext(), "EARNED ACHIEVEMENT " + temp.getName(),Toast.LENGTH_LONG).show();
                }

                if ((user.getCompletedTaskCount() >= temp.getTaskCountRequirement()) && (user.getHours() >= temp.getHoursRequirement()) && !(temp.getTaskCountRequirement() == -1) && !(temp.getHoursRequirement() == -1) && !temp.getEarned()){
                    temp.setEarned(true);
                    Toast.makeText(getApplicationContext(), "EARNED ACHIEVEMENT " + temp.getName(),Toast.LENGTH_LONG).show();
                }
            }
        }
    }

    private void updateAddToListButtonState(){
        if (EditTextEmpty(TaskName) || EditTextEmpty(TaskDescription) || EditTextEmpty(TaskHour) || EditTextEmpty(TaskMinute) || timeFormatCheck(TaskHour) || timeFormatCheck(TaskMinute)) btn_add_task.setEnabled(false);
        else btn_add_task.setEnabled(true);
    }

    private void updateEditFromListButtonState(){
        if (EditTextEmpty(TaskName) || EditTextEmpty(TaskDescription) || EditTextEmpty(TaskHour) || EditTextEmpty(TaskMinute) || timeFormatCheck(TaskHour) || timeFormatCheck(TaskMinute)) btn_edit_task.setEnabled(false);
        else btn_edit_task.setEnabled(true);
    }

    private boolean EditTextEmpty(EditText edit){
        return edit.getText().length() == 0;
    }

    private boolean timeFormatCheck(EditText num){
        int tempNum;
        String tempString;
        try{
            tempString = num.getText().toString().trim();
            tempNum = Integer.parseInt(tempString);
            return false;
        }
        catch(Exception e){
            return true;
        }
    }

    private class LocalTextWatcher implements TextWatcher { // watches change in text and updates button when all checks in updateAddToListButtonState are true
        public void afterTextChanged(Editable s) {
            updateAddToListButtonState();
        }

        public void beforeTextChanged(CharSequence s, int start, int count, int after) {
        }

        public void onTextChanged(CharSequence s, int start, int before, int count) {
        }
    }

    private class LocalTextWatcher2 implements TextWatcher { // so u can use 1 textWatcher when all items being watched exist at the same time.
        // this is not the case here since i create a task and then later edit it. watcher 1 is for creation, 2 is for editing. must have 2
        // since i cant edit something that is only being created.
        public void afterTextChanged(Editable s) {
            updateEditFromListButtonState();
        }

        public void beforeTextChanged(CharSequence s, int start, int count, int after) {
        }

        public void onTextChanged(CharSequence s, int start, int before, int count) {
        }
    }

    private void updateTaskView(){ // creates a clickable list of names of tasks
        pageCase = -1;
        setContentView(R.layout.lists);
        ArrayAdapter<String> TaskNames = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_single_choice, strobj.getListName(taskHeader));
        GridView TaskNameGrid = (GridView) findViewById(R.id.selection_list);
        TaskNameGrid.setAdapter(TaskNames);

        TaskNameGrid.setOnItemClickListener(new AdapterView.OnItemClickListener() { // cant use xml onclick for GridView so have to use this one
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                globalTask = strobj.getTask(position);
                selectedTask(view);
            }
        });
    }

    private void updateEditTexts(){ // works for multiple layouts as long as the info overlaps. i.e. the layouts that use this method have
        // 3 edit fields named: name, description, and time_to_complete. and they all go to the same variable.
        // this is the ONLY reason I can get away with this. structure your code to match for ease of reading
        TaskName = (EditText) findViewById(R.id.name);
        TaskDescription = (EditText) findViewById(R.id.description);
        TaskHour = (EditText) findViewById(R.id.task_hour);
        TaskMinute = (EditText) findViewById(R.id.task_minute);
    }

    @Override
    public void onBackPressed(){
        switch (pageCase){
            case 1: updateTaskView();
                break;
            default:
                super.onBackPressed();
                //List_Tasks.this.finish();
                break;
        }
        /*
        if (doubleBackToMenuPressedOnce){
            super.onBackPressed();
            return;
        }

        this.doubleBackToMenuPressedOnce = true;
        updateTaskView();
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