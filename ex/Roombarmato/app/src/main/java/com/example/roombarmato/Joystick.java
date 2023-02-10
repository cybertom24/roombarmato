package com.example.roombarmato;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;

public class Joystick extends View {
    // Values
    static final String TAG = "JOYSTICK";
    // Colors
    private int knobColor, externalColor;
    // Sizes
    private float knobSize;
    private int knobRadius, externalRadius;
    private int width, height;
    private int myPadding;
    // For drawing
    private Paint knobPaint, externalPaint;
    // Utility
    private float viewX, viewY, viewCentreX, viewCentreY;
    private float posX = 200, posY = 200;
    private boolean clicked = false;
    private boolean firstDraw = true;
    // Listener
    JoystickListener listener = null;

    public Joystick(Context context, AttributeSet attrs) {
        super(context, attrs);

        // Get the values
        TypedArray values = context.getTheme().obtainStyledAttributes(attrs, R.styleable.Joystick, 0, 0);
        try {
            knobColor = values.getInteger(R.styleable.Joystick_joystick_knobColor, 0);
            externalColor = values.getInteger(R.styleable.Joystick_joystick_externalColor, 0);
            knobSize = values.getFloat(R.styleable.Joystick_joystick_knobSize, 0);
            myPadding = values.getInteger(R.styleable.Joystick_joystick_myPadding, 0);
        } finally {
            values.recycle();
        }
        viewX = this.getX();
        viewY = this.getY();

        // Create the paints
        knobPaint = new Paint();
        externalPaint = new Paint();
    }

    @Override
    protected void onDraw(Canvas canvas) {
        // When it tries to draw the view:

        // Update the values
        width = this.getMeasuredWidth();
        height = this.getMeasuredHeight();
        viewCentreX = width/2;
        viewCentreY = height/2;
        if(firstDraw) {
            posX = viewCentreX;
            posY = viewCentreY;
        }
        externalRadius = width/2 - myPadding;
        knobRadius = (int) ( (float) externalRadius * (knobSize / 100) );

        // Update the paints
        knobPaint.setColor(knobColor);
        knobPaint.setStyle(Paint.Style.FILL);
        knobPaint.setAntiAlias(true);

        externalPaint.setColor(externalColor);

        // Draw the view
        canvas.drawCircle(viewCentreX, viewCentreY, externalRadius, externalPaint);
        canvas.drawCircle(posX, posY, knobRadius, knobPaint);
        firstDraw = false;
        super.onDraw(canvas);
    }

    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) {
        invalidate();
        requestLayout();
        super.onSizeChanged(w, h, oldw, oldh);
    }

    float oldX, oldY;

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        // Only activates when the touch is knob the view
        float x = event.getX();
        float y = event.getY();
        switch(event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                // Check if it's clicked knob the knob
                float distanceFromKnob = (float) Math.sqrt(Math.pow(x - posX, 2) + Math.pow(y - posY, 2));
                clicked = distanceFromKnob <= knobRadius;
                break;
            case MotionEvent.ACTION_MOVE:
                if(clicked) {
                    float touchAngle = (float) Math.atan2(y - viewCentreY, x - viewCentreX);
                    float distance = (float) Math.sqrt(Math.pow(viewCentreX - x, 2) + Math.pow(viewCentreY - y, 2));
                    if(distance > externalRadius) distance = externalRadius;
                    posX = (float) (distance*Math.cos(touchAngle)) + viewCentreX;
                    posY = (float) (distance*Math.sin(touchAngle)) + viewCentreY;
                }
                break;
            case MotionEvent.ACTION_UP:
                clicked = false;
                posX = viewCentreX;
                posY = viewCentreY;
                break;
        }
        if(posX != oldX || posY != oldY) {
            if(listener != null) listener.onPositionChanged(new float[]{posX, posY}, new float[]{oldX, oldY});
        }
        oldX = posX;
        oldY = posY;

        invalidate();
        requestLayout();
        return true;
    }

    public float angle() {
        return (float) Math.atan2(posY - viewCentreY, posX - viewCentreX);
    }

    public float value() {
        return (float) Math.sqrt(Math.pow(viewCentreX - posX, 2) + Math.pow(viewCentreY - posY, 2));
    }

    public float value_p() {
        return (value() / externalRadius) * 100f;
    }

    public float valX() {
        return ((posX - viewCentreX) / externalRadius) * 100;
    }

    public float valY() {
        return ((viewCentreY - posY) / externalRadius) * 100;
    }

    public interface JoystickListener {
        public void onPositionChanged(float[] position, float[] oldPosition);
    }

    public void setJoystickListener(JoystickListener _listener) {
        listener = _listener;
    }

    public void setknobColor(int color) {
        knobColor = color;
        // Redraw
        invalidate();
        requestLayout();
    }

    public void setExternalColor(int color) {
        externalColor = color;
        // Redraw
        invalidate();
        requestLayout();
    }

    public void setKnobSize(float size) {
        knobSize = size;
        // Redraw
        invalidate();
        requestLayout();
    }

    public int getknobColor() {
        return knobColor;
    }

    public int getExternalColor() {
        return externalColor;
    }

    public float getknobSize() {
        return knobSize;
    }

    @Override
    protected void finalize() throws Throwable {
        super.finalize();
    }
}
