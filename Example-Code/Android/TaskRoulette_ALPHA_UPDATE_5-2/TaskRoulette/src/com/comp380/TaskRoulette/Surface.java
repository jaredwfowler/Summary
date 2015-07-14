package com.comp380.TaskRoulette;

import android.graphics.Canvas;

/**
 * Created by Cory on 3/14/14.
 */

// This interface abstracts the interaction between the animation logic and the display logic
// The Surface is instantiated inside AnimationThread class, implemented by the DrawablePanel class,
// and its methods are defined (overridden) in the class SpinnerPanel which extends DrawablePanel
public interface Surface {
    void onInitialize();
    void onDraw(Canvas canvas);
    void onUpdate(long gameTime);
}
