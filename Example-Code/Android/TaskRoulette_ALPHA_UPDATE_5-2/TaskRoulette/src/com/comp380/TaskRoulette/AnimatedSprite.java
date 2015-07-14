package com.comp380.TaskRoulette;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;

/**
 * Created by Cory on 3/14/14.
 */
public class AnimatedSprite extends Sprite{
    public int fps;
    public int numFrames;
    public int currentFrame;
    public long frameTimer;

    public AnimatedSprite(){
        super();
        frameTimer = 0;
        currentFrame = 0;
    }

    public void Initialize(	Bitmap bitmap,
                               int imgType,
                               int height,
                               int width,
                               int fps,
                               int frameCount,
                               int scale){
        this.image = bitmap;
        this.imgType = imgType;
        this.spriteHeight = height;
        this.spriteWidth = width;
        this.sRectangle.top = 0;
        this.sRectangle.bottom = spriteHeight;
        this.sRectangle.left = 0;
        this.sRectangle.right = spriteWidth;
        this.fps = 1000 / fps;
        this.numFrames = frameCount;
        this.scale = scale;
    }

    public void Update(long gameTime) {
        if (gameTime > frameTimer + fps) {
            frameTimer = gameTime;
            currentFrame += 1;

            if (currentFrame >= numFrames) {
                currentFrame = 0;
            }

            sRectangle.left = currentFrame * spriteWidth;
            sRectangle.right = sRectangle.left + spriteWidth;
        }
    }

    public void UpdateSpin(long gameTime) {
        if (gameTime > frameTimer + fps) {
            frameTimer = gameTime;
            currentFrame -= 1;

            if (currentFrame <= 0) {
                currentFrame = numFrames;
            }

            sRectangle.top = currentFrame;
            sRectangle.bottom = sRectangle.top + spriteHeight;
        }
    }

    public void draw(Canvas canvas) {
        if(imgType == ROULETTE_WHEEL){
            // Set position for roulette wheel
            setXPos((canvas.getWidth() - scale*spriteWidth)/2);
            setYPos((canvas.getHeight()/3) - ((scale*spriteHeight/2)));
        }else if(imgType == LETTER_BLUR){

            // clear the screen underneath to black between frames
            Paint p = new Paint();
            p.setColor(Color.BLACK);
            canvas.drawRect(0, 0, canvas.getWidth(), canvas.getHeight(), p);

            // Set position for letter blur
            setXPos((canvas.getWidth() - scale*spriteWidth)/2);
            setYPos(canvas.getHeight() -(canvas.getHeight()/3) - ((scale*spriteHeight/2)));
        }

        // draw the next frame of the animation
        Rect destinationRect = new Rect( getXPos(),
                getYPos(),
                getXPos() + scale*spriteWidth,
                getYPos() + scale*spriteHeight);
        canvas.drawBitmap(image, sRectangle, destinationRect, null);
    }
}
