package com.comp380.TaskRoulette;

import android.graphics.Canvas;
import android.view.SurfaceHolder;

/**
 * Created by Cory on 3/14/14.
 */
public class AnimationThread extends Thread {

    private SurfaceHolder surfaceHolder;
    private Surface panel;
    private boolean run = false;

    public AnimationThread(SurfaceHolder surfaceHolder, Surface panel) {
        this.surfaceHolder = surfaceHolder;
        this.panel = panel;

        panel.onInitialize();
    }

    public void setRunning(boolean value) {
        run = value;
    }

    private long timer;

    @Override
    public void run() {
        Canvas canvas;
        while (run) {
            canvas = null;
            timer = System.currentTimeMillis();
            panel.onUpdate(timer);

            try {
                canvas = surfaceHolder.lockCanvas(null);
                synchronized (surfaceHolder) {
                    panel.onDraw(canvas);
                }
            } finally {
                // do this in a finally so that if an exception is thrown
                // during the above, we don't leave the Surface in an
                // inconsistent state
                if (canvas != null) {
                    surfaceHolder.unlockCanvasAndPost(canvas);
                }
            }
        }
    }
}
