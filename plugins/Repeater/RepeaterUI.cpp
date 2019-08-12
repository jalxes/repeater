/*
 * DISTRHO Plugin Framework (DPF)
 * Copyright (C) 2012-2014 Filipe Coelho <falktx@falktx.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include "DistrhoUI.hpp"

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------

class RepeaterUI : public UI
{
public:
    RepeaterUI()
        : UI(405, 256)
    {
        std::memset(fParams, 0, sizeof(float)*kParamsCount);
        std::memset(fStrBuf, 0, sizeof(char)*(0xff+1));

        fFont = createFontFromFile("sans", "/usr/share/fonts/TTF/DejaVuSans.ttf");
    }


protected:
  /* --------------------------------------------------------------------------------------------------------
   * DSP/Plugin Callbacks */

  /**
     This plugin has no parameters, so we can safely ignore this.
   */
  void parameterChanged(uint32_t index, float value) override
  {
    // update our grid state to match the plugin side
    fParams[index] = value;

    // trigger repaint
    repaint();
  }


  /* --------------------------------------------------------------------------------------------------------
   * Widget Callbacks */

  /**
     The OpenGL drawing function.
     This UI will draw a 3x3 grid, with on/off states according to plugin state.
   */
    void onNanoDisplay() override
  {
   static const float lineHeight = 20;

        fontSize(15.0f);
        textLineHeight(lineHeight);

        float x = 0;
        float y = 15;

        drawLeft(x, y, "kNumberLastBars:");
        drawRight(x, y, getTextBufFloat(fParams[kNumberLastBars]));
        y+=lineHeight;

        drawLeft(x, y, "kInitBar:");
        drawRight(x, y, getTextBufFloat(fParams[kInitBar]));
        y+=lineHeight;

        drawLeft(x, y, "kRepeat:");
        drawRight(x, y, getTextBufFloat(fParams[kRepeat]));
        y+=lineHeight;
        
        drawLeft(x, y, "kCurEventIndex:");
        drawRight(x, y, getTextBufFloat(fParams[kCurEventIndex]));
        y+=lineHeight;
        
        drawLeft(x, y, "kTimePosBar:");
        drawRight(x, y, getTextBufFloat(fParams[kTimePosBar]));
        y+=lineHeight;
  }

  // -------------------------------------------------------------------------------------------------------

private:
 float fParams[kParamsCount];

    FontId fFont;
    char fStrBuf[0xff+1];

    const char* getTextBufFloat(const float value)
    {
        std::snprintf(fStrBuf, 0xff, "%.1f", value);
        return fStrBuf;
    }
    
  // helpers for drawing text
    void drawLeft(const float x, const float y, const char* const text)
    {
        beginPath();
        fillColor(200, 200, 200);
        textAlign(ALIGN_RIGHT|ALIGN_TOP);
        textBox(x, y, 100, text);
        closePath();
    }

    void drawRight(const float x, const float y, const char* const text)
    {
        beginPath();
        fillColor(255, 255, 255);
        textAlign(ALIGN_LEFT|ALIGN_TOP);
        textBox(x+105, y, 100, text);
        closePath();
    }

  /**
     Set our UI class as non-copyable and add a leak detector just in case.
   */
  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RepeaterUI)
};

/* ------------------------------------------------------------------------------------------------------------
 * UI entry point, called by DPF to create a new UI instance. */

UI*
createUI()
{
  return new RepeaterUI();
}

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
