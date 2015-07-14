package com.comp380.TaskRoulette;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.util.Log;

/**
 * Created by Cory on 3/16/14.
 */
public class TextBitmap {
    private static final String TAG = "CORY";
    private static final int charXDim = 11; // number of chars across in the font map
    private static final int charWidth = 6; // spacing in font map
    private static final int charHeight = 10;
    private static final int textFieldWidth = 120;  // dimensions of the text display area
    private static final int textFieldHeight = 12;
    private Bitmap fontMap;
    private String text;    // our text string
    private char[] symbol;
    private int[] width;
    private int xPos;
    private int yPos;
    private Rect srcRect;
    private Rect dstRect;
    private Rect spinSrcRect;
    private Rect spinDstRect;
    private int scale;
    private int totalWidth; // used to keep track of spun letter position in font map
    private int spinWidth;  // used to keep track of spinning letter position in font map
    private int iSpunDown;  // index of what has been spun down
    private int spinningSymbolIndex;    // index of symbol that is spinning
    private boolean doneSpinning;       // to keep track of when it's done spinning

    private int spinDelay;  // used to create a delay when user first spins

    // animation fields
    private int fpsDelay;    // frames per second
    private int numFrames;  // number of frames
    private int currentFrame;
    private long frameTimer;

    public TextBitmap() {
        symbol = new char[]{'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
                'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
                'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
                'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
                's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2',
                '3', '4', '5', '6', '7', '8', '9', '!', '?'};

        width = new int[]{4,4,4,4,4,4,4,4,1,4,4,
                3,5,4,4,4,4,4,4,5,4,4,
                5,4,5,4,4,4,4,4,4,3,4,
                4,1,2,3,1,5,4,4,4,4,3,
                4,3,4,4,5,4,4,4,4,2,4,
                4,4,4,4,4,4,4,1,5};

        srcRect = new Rect(0,0,0,0); // left, top, right, bottom
        dstRect = new Rect(0,0,0,0);
        spinSrcRect = new Rect(0,0,0,0);
        spinDstRect = new Rect(0,0,0,0);
        xPos = 0;
        yPos = 0;
        totalWidth = 0;
        spinWidth = 0;
        iSpunDown = 0;
        spinningSymbolIndex = -1;
        fpsDelay = 0;
        numFrames = 12;
        frameTimer = 0;
        currentFrame = 0;

        spinDelay = 100;
        doneSpinning = true;
    }

    public void Initialize(Bitmap bitmap, String text, int scale){
        this.fontMap = bitmap;
        this.text = text;
        this.scale = scale;
        //iSpunDown = text.length();
        iSpunDown = 1;
    }

    public void setSpinning(){
        doneSpinning = false;
    }

    public boolean isDoneSpinning(){
        return doneSpinning;
    }

    public int getXPos() {
        return xPos;
    }

    public void setText(String text){
        this.text = text;
        iSpunDown = 0;
        spinWidth = 0;
        spinDelay = 100;
    }

    public int getYPos() {
        return yPos;
    }

    public void setXPos(int value) {
        xPos = value;
    }

    public void setYPos(int value) {
        yPos = value;
    }

    public void Update(long gameTime){
        // if not all spun down
        if (iSpunDown < text.length()){
            if (gameTime > frameTimer + fpsDelay){
                frameTimer = gameTime;
                currentFrame += 4;

                if (currentFrame >= numFrames){
                    iSpunDown++;
                    currentFrame = 0;
                }

                if (spinningSymbolIndex != -1) {
                    spinSrcRect.bottom = (spinningSymbolIndex / charXDim) * charHeight + charHeight;
                    spinSrcRect.top = spinSrcRect.bottom - currentFrame;
                    spinSrcRect.left = (spinningSymbolIndex % charXDim) * charWidth;
                    spinSrcRect.right = spinSrcRect.left + width[spinningSymbolIndex];

                    spinDstRect.left = getXPos() + spinWidth * scale;
                    spinDstRect.top = getYPos();
                    spinDstRect.right = getXPos() + (spinWidth + width[spinningSymbolIndex]) * scale;
                    spinDstRect.bottom = getYPos() + currentFrame * scale;
                } else {
                    // make a space of 3 pixels
                    spinSrcRect.top = 50;
                    spinSrcRect.bottom = 60;
                    spinSrcRect.left = 55;
                    spinSrcRect.right = 57;

                    spinDstRect.left = getXPos() + spinWidth * scale;
                    spinDstRect.top = getYPos();
                    spinDstRect.right = getXPos() + (spinWidth + 3) * scale;
                    spinDstRect.bottom = getYPos() + charHeight * scale;
                }
            }
        } else {
            doneSpinning = true;
        }
    }

    public void draw(Canvas canvas) {
        Paint p = new Paint();
        p.setAlpha(0);  // make the background transparent
        canvas.drawRect(0,0, canvas.getWidth(), canvas.getHeight(), p);

        // set position
        int xyOffset = scale*2;
        setXPos((canvas.getWidth() - scale*textFieldWidth)/2 + xyOffset);
        setYPos(canvas.getHeight() -(canvas.getHeight()/3) - ((scale*textFieldHeight/2)) + xyOffset);

        // draw each character
        int symbolIndex;
        int yOffset = 0;
        for(int i = 0; i < (text.length()); i++){

            // identify the character
            symbolIndex = -1;
            if (text.charAt(i)>= 'A' && text.charAt(i) <= 'Z') {
                // See if it is A-Z, then calculate symbol index
                symbolIndex = text.charAt(i) - 'A';
            }else if (text.charAt(i) >= 'a' && text.charAt(i) <= 'z'){
                // See if it is a-z, then calculate symbol index
                symbolIndex = text.charAt(i) - 'a' + 26;
            }else if (text.charAt(i) >= '0' && text.charAt(i) <= '9'){
                // See if it is 0-9, then calculate symbol index
                symbolIndex = text.charAt(i) - '0' + 52;
            }else if (text.charAt(i) == '!') {
                // See if it is !, then calculate symbol index
                symbolIndex = 62;
            }else if (text.charAt(i) == '?') {
                // See if it is ?, then calculate symbol index
                symbolIndex = 63;
            }

            // if character identified, set up the srcRect and dstRect
            if (symbolIndex != -1) {
                srcRect.top = (symbolIndex / charXDim) * charHeight;
                srcRect.bottom = srcRect.top + charHeight;
                srcRect.left = (symbolIndex % charXDim) * charWidth;
                srcRect.right = srcRect.left + width[symbolIndex];

                dstRect.left = getXPos() + totalWidth * scale;
                dstRect.top = getYPos();
                dstRect.right = getXPos() + (totalWidth + width[symbolIndex]) * scale;
                dstRect.bottom = getYPos() + charHeight * scale;

                spinWidth = totalWidth;
                totalWidth += width[symbolIndex] + 1;
            // else assume it is a space
            } else {
                // make a space of 3 pixels
                srcRect.top = 50;
                srcRect.bottom = 60;
                srcRect.left = 55;
                srcRect.right = 57;

                dstRect.left = getXPos() + totalWidth * scale;
                dstRect.top = getYPos();
                dstRect.right = getXPos() + (totalWidth + 3) * scale;
                dstRect.bottom = getYPos() + charHeight * scale;

                spinWidth = totalWidth;
                totalWidth += 3;
            }

            // draw the character
            if(i >= iSpunDown){
                // spin it down

                spinningSymbolIndex = symbolIndex;

                i = text.length();
            }else{
                p.setAlpha(255);
                p.setColor(Color.BLACK);
                // left top right bottom
                Rect blackRect = new Rect(0,0,0,0);
                blackRect.top = (canvas.getHeight()-canvas.getHeight()/3-textFieldHeight*scale/2);

                // only fill behind the text
                blackRect.right = (canvas.getWidth()-textFieldWidth*scale)/2 + (totalWidth+1)*scale;
                if(symbolIndex != -1){
                    blackRect.left = blackRect.right - scale*(width[symbolIndex]+1);
                }else{
                    blackRect.left = blackRect.right - 3*scale;
                }

                blackRect.bottom = blackRect.top + textFieldHeight*scale;
                canvas.drawRect(blackRect, p);
                canvas.drawBitmap(fontMap, srcRect, dstRect, null);
            }
        }
        canvas.drawBitmap(fontMap, spinSrcRect, spinDstRect, null);
        totalWidth = 0;
    }
}
