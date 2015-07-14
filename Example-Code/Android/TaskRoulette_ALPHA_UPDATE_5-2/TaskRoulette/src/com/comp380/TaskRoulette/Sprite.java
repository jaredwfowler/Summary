package com.comp380.TaskRoulette;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Rect;

/**
 * Created by Cory on 3/26/14.
 */
public class Sprite {
    public static final int ROULETTE_WHEEL = 1;
    public static final int LETTER_BLUR = 2;
    public static final int LETTER_SLOT = 3;
    public static final int BLACK_RECT = 4;

    public Bitmap image;
    public int xPos;
    public int yPos;
    public Rect sRectangle;
    public int spriteHeight;
    public int spriteWidth;
    public int scale;
    public int imgType;

    public Sprite() {
        sRectangle = new Rect(0,0,0,0);
        xPos = 0;
        yPos = 0;
    }

    public void Initialize( Bitmap bitmap,
                            int imgType,
                            int height,
                            int width,
                            int scale){
        this.image = bitmap;
        this.imgType = imgType;
        this.spriteHeight = height;
        this.spriteWidth = width;
        this.sRectangle.top = 0;
        this.sRectangle.bottom = spriteHeight;
        this.sRectangle.left = 0;
        this.sRectangle.right = spriteWidth;
        this.scale = scale;
    }

    public int getXPos() {
        return xPos;
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

    public void draw(Canvas canvas) {

        //Set position for letter slot
        setXPos((canvas.getWidth() - scale*spriteWidth)/2);
        setYPos(canvas.getHeight() -(canvas.getHeight()/3) - ((scale*spriteHeight/2)));

        // draw the image
        Rect destinationRect = new Rect( getXPos(),
                getYPos(),
                getXPos() + scale*spriteWidth,
                getYPos() + scale*spriteHeight);
        canvas.drawBitmap(image, sRectangle, destinationRect, null);
    }
}
