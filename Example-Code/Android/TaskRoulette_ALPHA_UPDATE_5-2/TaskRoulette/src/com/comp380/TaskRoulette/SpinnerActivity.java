package com.comp380.TaskRoulette;

import android.app.Activity;
import android.content.Context;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Point;
import android.os.Bundle;
import android.util.Log;
import android.view.Display;
import android.view.MotionEvent;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ImageButton;
import java.util.Random;

/**
 * Created by Cory on 3/14/14.
 */
public class SpinnerActivity extends Activity {

    private static final String TAG = "SPINNER";
    private static final int ROULETTE_WHEEL = 1;
    private static final int LETTER_BLUR = 2;
    private static final int LETTER_SLOT = 3;
    private static final int BLACK_RECT = 4;

    private int spinDelay = 100;

    private StoreObj strobj;
    private final String taskHeader = "taskHeader,";

    AnimatedSprite rouletteWheel = new AnimatedSprite();
    AnimatedSprite letterBlur = new AnimatedSprite();
    Sprite letterSlot = new Sprite();
    Sprite blackRect = new Sprite();
    TextBitmap textBmp = new TextBitmap();

    Random randomNumGen = new Random();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        strobj = ((StoreObj) getApplicationContext());
        setContentView(new SpinnerPanel(this));
    }

    class SpinnerPanel extends DrawablePanel {
        private boolean spinning;
        public SpinnerPanel(Context context) {
            super(context);
            spinning = false;
        }

        @Override
        public void onDraw(Canvas canvas) {
            super.onDraw(canvas);
            if(spinning){
                SpinnerActivity.this.letterBlur.draw(canvas);
            }else{
                SpinnerActivity.this.blackRect.draw(canvas);
            }
            SpinnerActivity.this.letterSlot.draw(canvas);
            SpinnerActivity.this.textBmp.draw(canvas);
            SpinnerActivity.this.rouletteWheel.draw(canvas);
        }

        @Override
        public void onInitialize() {
            SpinnerActivity.this.rouletteWheel.Initialize(BitmapFactory.decodeResource(getResources(),
                    R.drawable.roulette_wheel), ROULETTE_WHEEL, 32, 64, 5, 8, 6);
            SpinnerActivity.this.letterBlur.Initialize(BitmapFactory.decodeResource(getResources(),
                    R.drawable.letter_spin_blur), LETTER_BLUR, 12, 120, 64, 54, 3);
            SpinnerActivity.this.blackRect.Initialize(BitmapFactory.decodeResource(getResources(),
                    R.drawable.black_rect), BLACK_RECT, 32, 128, 3);
            SpinnerActivity.this.letterSlot.Initialize(BitmapFactory.decodeResource(getResources(),
                    R.drawable.letter_slot), LETTER_SLOT, 32, 128, 3);
            SpinnerActivity.this.textBmp.Initialize(
                    BitmapFactory.decodeResource(getResources(), R.drawable.green_pixel_font_64),
                    "Tap to spin ",3);
        }

        @Override
        public void onUpdate(long gameTime) {
            if(SpinnerActivity.this.textBmp.isDoneSpinning()){
                spinning = false;
            }
            if(spinning && !SpinnerActivity.this.textBmp.isDoneSpinning()){
                SpinnerActivity.this.rouletteWheel.Update(gameTime);
                SpinnerActivity.this.letterBlur.UpdateSpin(gameTime);

                // This causes a small delay (2-3 seconds) after the user presses spin
                if(spinDelay > 0){
                    spinDelay--;
                }else{
                    SpinnerActivity.this.textBmp.Update(gameTime);
                }

            }else{
                SpinnerActivity.this.textBmp.Update(gameTime);
            }
        }

        @Override
        public boolean onTouchEvent(MotionEvent e) {
            switch (e.getAction()){
                case MotionEvent.ACTION_DOWN:
                    if(spinning) {
                        spinning = false;
                        SpinnerActivity.this.textBmp.setText("Tap to spin ");
                    }else{
                        spinDelay = 100;
                        spinning = true;
                        SpinnerActivity.this.textBmp.setSpinning();
                        int taskListSize = strobj.getListName(taskHeader).length;
                        if (taskListSize == 0){
                            SpinnerActivity.this.textBmp.setText("Your task list is empty ");
                        }else{
                            int randomInt = randomNumGen.nextInt(taskListSize);
                            String taskName = strobj.getTask(randomInt).getName();
                            SpinnerActivity.this.textBmp.setText(taskName + " ");
                        }
                    }
            }
            return true;
        }
    }

    @Override
    public void onBackPressed(){
        super.onBackPressed();
    }
}
