
// how often no refresh on idle state, in Hz. 0 to disable animation during idle state
#define UI_REFRESH_RATE 30

#include "RayUI.hpp"
#include "PluginUtils.h"

START_NAMESPACE_DISTRHO

class AllpassUI : public RayUI
{
public:
   /**
      UI class constructor.
      The UI should be initialized to a default state that matches the plugin side.
    */

  AllpassUI() : RayUI(DISTRHO_UI_DEFAULT_WIDTH, DISTRHO_UI_DEFAULT_HEIGHT, UI_REFRESH_RATE, TEXTURE_FILTER_POINT) {
  }
  
  ~AllpassUI() {
  }
  
protected:
    // ----------------------------------------------------------------------------------------------------------------
    // DSP/Plugin Callbacks

   /**
      A parameter has changed on the *plugin side*.@n
      This is called by the host to inform the UI about parameter changes.
    */
    void parameterChanged(uint32_t index, float value) override {
      if (index < kParameterCount) {
	dspParams[index] = value;
      }
    }

    // ----------------------------------------------------------------------------------------------------------------
    // Widget Callbacks
  void onMainDisplay() override
  {
    ClearBackground(BLUE);
  }
  
  void onCanvasDisplay() override
  {
    ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

    // sync ui and dsp
    for (int i=0; i < kParameterCount; i++) {
      uiParams[i] = dspParams[i];
    }
    // we'll ease copy and paste
    int curParam = 0;

    curParam = kDryWet;
    GuiSlider(layoutRecs[0], TextFormat("Dry/Wet: %f", uiParams[curParam]), NULL, &(uiParams[curParam]), params[curParam].min, params[curParam].max);

    curParam = kDecay;
    GuiSliderBar(layoutRecs[1], TextFormat("Decay: %f", uiParams[curParam]), NULL, &(uiParams[curParam]), params[curParam].min, params[curParam].max);

    curParam = kDelay;
    GuiSliderBar(layoutRecs[2], TextFormat("Delay: %f ms", uiParams[curParam]), NULL, &(uiParams[curParam]), params[curParam].min, params[curParam].max);

    // only send value if updated
    for (int i=0; i < kParameterCount; i++) {
      if (uiParams[i] != dspParams[i]) {
	setParameterValue(i, uiParams[i]);
	// note: only output parameters, if any, will be fired back, hence sync also here
	dspParams[i] = uiParams[i];
      }
    }

    DrawFPS(10, 10);
  }

    // -------------------------------------------------------------------------------------------------- --------------

private:
  // parameters sync with DSP
  float dspParams[kParameterCount];
  // those used in UI
  float uiParams[kParameterCount];

  // upper left reference point for UI
  static constexpr Vector2 anchor = { 10, 5 };
  // layout of the GUI
  Rectangle layoutRecs[4] = {
    (Rectangle){ anchor.x + 144, anchor.y + 0, 216, 32 },
    (Rectangle){ anchor.x + 144, anchor.y + 40, 216, 32 },
    (Rectangle){ anchor.x + 144, anchor.y + 80, 216, 32 },
    (Rectangle){ anchor.x + 0, anchor.y + 120, 360, 192 },
  };
  
  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AllpassUI)
};

// --------------------------------------------------------------------------------------------------------------------

UI* createUI()
{
    return new AllpassUI();
}
 
// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
 
