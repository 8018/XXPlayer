package me.xfly.boboplayer;

import android.graphics.Rect;

public class TimedText {
    private Rect mTextBounds = null;
    private String mTextChars = null;

    public TimedText(Rect bounds, String text) {
        mTextBounds = bounds;
        mTextChars = text;
    }

    public Rect getBounds() {
        return mTextBounds;
    }

    public String getText() {
        return mTextChars;
    }
}
