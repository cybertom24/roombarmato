package com.example.roombarmato;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;

public class Lever extends View {
    static final String TAG = "LEVER";
    // Drawing
    private int knobColor, sliderColor;
    private float knobSize, sliderWidth, sliderHeight;
    private Paint knobPaint, sliderPaint, textPaint;
    private float width, height, viewCenterX, viewCenterY;
    private float knobRadius, rectWidth, rectHeight;
    private boolean vertical = true, left;
    private float maxX, maxY, minX, minY;
    private boolean textVisible;
    // Utility
    private boolean clicked = false, firstDraw = true;
    private float value, posX, posY, defaultValue, step, resolution, numberOfSteps, oldNumberOfSteps, deltaValue, quantum;
    private float minValue, maxValue;
    // Other
    private LeverListener listener;

    public Lever(Context context, AttributeSet attrs) {
        super(context, attrs);
        TypedArray values = context.getTheme().obtainStyledAttributes(attrs, R.styleable.Lever, 0, 0);
        try {
            knobColor = values.getInteger(R.styleable.Lever_lever_knobColor, 0);
            sliderColor = values.getInteger(R.styleable.Lever_lever_sliderColor, 0);
            sliderColor = values.getInteger(R.styleable.Lever_lever_sliderColor, 0);
            knobSize = values.getFloat(R.styleable.Lever_lever_knobSize, 0);
            sliderWidth = values.getFloat(R.styleable.Lever_lever_sliderWidth, 0);
            sliderHeight = values.getFloat(R.styleable.Lever_lever_sliderHeight, 0);
            vertical = values.getInteger(R.styleable.Lever_lever_orientation, context.getResources().getInteger(R.integer.vertical)) == context.getResources().getInteger(R.integer.vertical);
            defaultValue = values.getFloat(R.styleable.Lever_lever_defaultValue, 50);
            resolution = values.getFloat(R.styleable.Lever_lever_resolution, 100);
            maxValue = values.getFloat(R.styleable.Lever_lever_maxValue, 100);
            minValue = values.getFloat(R.styleable.Lever_lever_minValue, 0);
            textVisible = values.getBoolean(R.styleable.Lever_lever_textVisible, false);
            left = values.getInteger(R.styleable.Lever_lever_textSide, context.getResources().getInteger(R.integer.left)) == context.getResources().getInteger(R.integer.left);
        } finally {
            values.recycle();
        }

        knobPaint = new Paint();
        sliderPaint = new Paint();
        textPaint = new Paint();

        width = getMeasuredWidth();
        height = getMeasuredHeight();
        viewCenterX = width/2;
        viewCenterY = height/2;

        deltaValue = maxValue - minValue;
        quantum = deltaValue/resolution;

        if(defaultValue > maxValue) defaultValue = maxValue;
        if(defaultValue < minValue) defaultValue = minValue;

        listener = null;
    }

    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) {
        width = w;
        height = h;
        viewCenterX = width/2;
        viewCenterY = height/2;
        super.onSizeChanged(w, h, oldw, oldh);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        knobPaint.setColor(knobColor);
        knobPaint.setStyle(Paint.Style.FILL);
        knobPaint.setAntiAlias(true);

        sliderPaint.setColor(sliderColor);
        sliderPaint.setStyle(Paint.Style.FILL);
        sliderPaint.setAntiAlias(true);

        textPaint.setColor(knobColor);
        textPaint.setTextSize(40);
        textPaint.setAntiAlias(true);

        if(vertical) {
            rectWidth = width * sliderWidth/100;
            rectHeight = height * sliderHeight/100;
            canvas.drawRect((width-rectWidth)/2, (height-rectHeight)/2, rectWidth/2+viewCenterX, rectHeight/2+viewCenterY, sliderPaint);

            posX = viewCenterX;
            minX = viewCenterX;
            maxX = viewCenterX;
            minY = viewCenterY - rectHeight/2;
            maxY = viewCenterY + rectHeight/2;
            step = rectHeight/resolution;
            knobRadius = rectWidth * knobSize/100;

            if(firstDraw) {
                deltaValue = maxValue - minValue;
                quantum = deltaValue/resolution;
                numberOfSteps = (float) Math.floor((defaultValue - minValue)/quantum);

                oldNumberOfSteps = numberOfSteps;
            }

            posY = (resolution - numberOfSteps)*step + minY;
            canvas.drawCircle(posX, posY, knobRadius, knobPaint);
             if(textVisible) {
                 if (left) {
                     canvas.drawText("" + (int) maxValue, (float) (viewCenterX - knobRadius * 1.25), minY, textPaint);
                     canvas.drawText("" + (int) minValue, (float) (viewCenterX - knobRadius * 1.25), maxY, textPaint);
                 } else {
                     canvas.drawText("" + (int) maxValue, (float) (viewCenterX + knobRadius * 1.25), minY, textPaint);
                     canvas.drawText("" + (int) minValue, (float) (viewCenterX + knobRadius * 1.25), maxY, textPaint);
                 }
             }
        }
        else {

        }

        firstDraw = false;
        super.onDraw(canvas);
    }

    float previousX, previousY;

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        float x = event.getX();
        float y = event.getY();
        switch(event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                float distanceFromKnob = (float) Math.sqrt(Math.pow(x - posX, 2) + Math.pow(y - posY, 2));
                clicked = distanceFromKnob <= knobRadius;
                break;
            case MotionEvent.ACTION_MOVE:
                if(clicked) {
                    if(vertical) {
                        posX = x;
                        posY = y;

                        if(posX > maxX) posX = maxX;
                        else if(posX < minX) posX = minX;
                        if(posY > maxY) posY = maxY;
                        else if(posY < minY) posY = minY;

                        posY -= minY;

                        numberOfSteps = (float) Math.floor(posY/step);
                        numberOfSteps = resolution - numberOfSteps;

                        if(numberOfSteps != oldNumberOfSteps) {
                            // Call the listener (only if it exists)
                            if(listener != null) {
                                listener.onValueChanged(value(), value(oldNumberOfSteps));
                            }
                        }

                        oldNumberOfSteps = numberOfSteps;
                    }
                }
                break;
            case MotionEvent.ACTION_UP:
                clicked = false;
                break;

        }

        previousX = x;
        previousY = y;

        invalidate();
        requestLayout();
        return true;
    }

    // Create the interface
    public interface LeverListener {
        // Define the various events
        public void onValueChanged(float value, float oldValue);
    }

    // For creating the listeners
    public void setLeverListener(LeverListener listener_) {
        listener = listener_;
    }

    public float value() {
        deltaValue = maxValue - minValue;
        quantum = deltaValue/resolution;
        value = quantum*numberOfSteps;
        value += minValue;
        return value;
    }

    public float value(float numberOfSteps_) {
        deltaValue = maxValue - minValue;
        quantum = deltaValue/resolution;
        return quantum*numberOfSteps_ + minValue;
    }

    public void setKnobColor(int color) {
        knobColor = color;
        invalidate();
        requestLayout();
    }

    public void setSliderColor(int color) {
        sliderColor = color;
        invalidate();
        requestLayout();
    }

    public void setKnobSize(float size) {
        knobSize = size;
        invalidate();
        requestLayout();
    }

    public void setSliderWidth(float size) {
        sliderWidth = size;
        invalidate();
        requestLayout();
    }

    public void setSliderHeight(float size) {
        sliderHeight = size;
        invalidate();
        requestLayout();
    }

    public int getKnobColor() {
        return knobColor;
    }

    public int getSliderColor() {
        return sliderColor;
    }

}
