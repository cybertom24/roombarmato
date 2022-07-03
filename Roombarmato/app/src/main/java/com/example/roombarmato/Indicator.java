package com.example.roombarmato;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.view.View;

public class Indicator extends View {
    // Drawing
    private float width, height, viewCentreX, viewCentreY;
    public int quantity;
    private float spacing, ledRadius, ledSize, strokeSize;
    private int backgroundColor, onColor, offColor, strokeColor;
    private Paint backgroundPaint, onPaint, offPaint, strokePaint;
    private boolean vertical;
    // Utility
    private boolean[] statusArray;

    public Indicator(Context context, AttributeSet attrs) {
        super(context, attrs);

        TypedArray values = context.getTheme().obtainStyledAttributes(attrs, R.styleable.Indicator, 0, 0);
        try {
            onColor = values.getInteger(R.styleable.Indicator_indicator_onColor, 0xFFFFFFFF);
            offColor = values.getInteger(R.styleable.Indicator_indicator_offColor, 0xFF000000);
            backgroundColor = values.getInteger(R.styleable.Indicator_indicator_backgroundColor, 0xFFBEBEBE);
            quantity = values.getInteger(R.styleable.Indicator_indicator_quantity, 3);
            vertical = values.getInteger(R.styleable.Indicator_indicator_orientation, context.getResources().getInteger(R.integer.vertical)) == context.getResources().getInteger(R.integer.vertical);
            strokeColor = values.getInteger(R.styleable.Indicator_indicator_strokeColor, 0xFF050505);
            ledSize = values.getFloat(R.styleable.Indicator_indicator_ledSize, 50);
            strokeSize = values.getFloat(R.styleable.Indicator_indicator_strokeSize, 5);
        } finally {
            values.recycle();
        }

        backgroundPaint = new Paint();
        onPaint = new Paint();
        offPaint = new Paint();
        strokePaint = new Paint();

        statusArray = new boolean[quantity];
    }

    @Override
    protected void onDraw(Canvas canvas) {
        width = getMeasuredWidth();
        height = getMeasuredHeight();
        viewCentreX = width / 2;
        viewCentreY = height / 2;
        if(vertical) spacing = height / quantity;
        else spacing = width / quantity;
        ledRadius = (spacing*(ledSize/100))/2;

        backgroundPaint.setColor(backgroundColor);
        backgroundPaint.setAntiAlias(true);
        backgroundPaint.setStyle(Paint.Style.FILL);

        onPaint.setColor(onColor);
        onPaint.setAntiAlias(true);
        onPaint.setStyle(Paint.Style.FILL);

        offPaint.setColor(offColor);
        offPaint.setAntiAlias(true);
        offPaint.setStyle(Paint.Style.FILL);

        strokePaint.setColor(strokeColor);
        strokePaint.setAntiAlias(true);
        strokePaint.setStyle(Paint.Style.STROKE);
        strokePaint.setStrokeWidth(ledRadius*(strokeSize/100));
        strokePaint.setStrokeCap(Paint.Cap.ROUND);

        if (vertical) {

        } else {
            canvas.drawRect(0, 0, width, height, backgroundPaint);
            float circleY = height / 2;
            for (int i = 0; i < quantity; i++) {
                float circleX = spacing * i + spacing / 2;
                if (statusArray[i]) canvas.drawCircle(circleX, circleY, ledRadius, onPaint);
                else canvas.drawCircle(circleX, circleY, ledRadius, offPaint);
                canvas.drawCircle(circleX, circleY, ledRadius, strokePaint);
            }
        }

        super.onDraw(canvas);
    }

    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) {
        super.onSizeChanged(w, h, oldw, oldh);
    }

    public void setStatusArray(boolean[] statusArray_) throws Exception {
        Exception e = new Exception("The new status array must have the same length of the old" + "\nOld: " + statusArray.length + " - New: " + statusArray_.length);
        if(statusArray_.length == statusArray.length) {
            statusArray = statusArray_.clone();
            invalidate();
            requestLayout();
        }
        else throw e;
    }

    public void setStatus(boolean status_, int i) throws Exception {
        Exception e = new Exception("ID too big");
        if(i < statusArray.length) {
            statusArray[i] = status_;
            invalidate();
            requestLayout();
        }
        else throw e;
    }

    public void setOnColor(int color) {
        onColor = color;
        invalidate();
        requestLayout();
    }

    public void setOffColor(int color) {
        offColor = color;
        invalidate();
        requestLayout();
    }

    public void setBackgroundColor_(int color) {
        backgroundColor = color;
        invalidate();
        requestLayout();
    }

    public void setStrokeColor(int color) {
        strokeColor = color;
        invalidate();
        requestLayout();
    }

    public void setLedSize(float size) {
        ledSize = size;
        invalidate();
        requestLayout();
    }

    public void setStrokeSize(float size) {
        strokeSize = size;
        invalidate();
        requestLayout();
    }

    public void setQuantity(int quantity_) {
        quantity = quantity_;
        statusArray = new boolean[quantity];
        invalidate();
        requestLayout();
    }

    public int getQuantity() {
        return quantity;
    }
}
