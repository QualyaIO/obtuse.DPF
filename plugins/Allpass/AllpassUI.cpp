
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
    String resourcesLocation = getResourcesLocation();
    d_stdout("resources location: %s", resourcesLocation.buffer());
    // load model
    model = LoadModel(resourcesLocation + "patatoide.obj");

    // camera in the diagonal of origin, high enough to fit the whole scene with selected fov
    camera.position = (Vector3){ 0.0, 5.0, 5.0 };
    // toward origin
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    // camera pointing down
    camera.up = (Vector3){ 0.0, 0.0, -1.0};
    // field-of-view Y
    camera.fovy = 45.0f;
    // camera projection type
    camera.projection = CAMERA_PERSPECTIVE;

    // init canvas for 3D scene with place holder size
    canvasScene = LoadRenderTexture(layoutRecs[3].width, layoutRecs[3].height);
    // alternate route, power of 2, assumption that the placeholder for the rendering is 16:9 format
    //canvasScene = LoadRenderTexture(1024, 768);
    SetTextureFilter(canvasScene.texture, TEXTURE_FILTER_POINT);
    //SetTextureFilter(canvasScene.texture, TEXTURE_FILTER_BILINEAR);
  }
  
  ~AllpassUI() {
    // unload assets and canvas
    UnloadModel(model);
    UnloadRenderTexture(canvasScene);
  }
  
protected:
    // ----------------------------------------------------------------------------------------------------------------
    // DSP/Plugin Callbacks

   /**
      A parameter has changed on the *plugin side*.@n
      This is called by the host to inform the UI about parameter changes.
    */
    void parameterChanged(uint32_t index, float value) override {
      // backport change in maxDelay
      if (index == kMaxDelay && dspParams[kDelay] > value) {
	dspParams[kDelay] = value;
      }
      // special case with delay, clamp to effective max
      if (index == kDelay && dspParams[kMaxDelay] > 0 && value > dspParams[kMaxDelay]) {
	dspParams[index] = dspParams[kMaxDelay];
      }
      else if (index < kParameterCount) {
	dspParams[index] = value;
      }
    }

    // ----------------------------------------------------------------------------------------------------------------
    // Widget Callbacks
  void onMainDisplay() override
  {
    ClearBackground(BLUE);

    // 3D scene
    BeginTextureMode(canvasScene);
    // we have to clear background for anything to display
    ClearBackground(Color({0,0,0,0}));

    // playing with camera for now
    camera.position = (Vector3){0.0, 1.0 + dspParams[kDecay] * 5, 1.0 + dspParams[kDecay] * 5};
    camera.fovy = 100.0f  -  dspParams[kDecay] * 50;

    BeginMode3D(camera);

    rlPushMatrix();
    // se rotation speed to dry/wet for now
    static float r = 0;
    r = r + 1 + dspParams[kDryWet] * 5;
    // rotate along <1,0,0> x-axis
    rlRotatef(r, 0, 1, 0);

    // Draw sun
    DrawSphereWires(positionSun, 1.0, 5, 10, YELLOW);

    rlPopMatrix();

    rlPushMatrix();
    rlRotatef(r/2, 0, 1, 0);

    // scale-down planet to match desired proportion
    // Note: wireframe does not work with GLES2 it seems, requires project with raylib compiled set to OPENGL 3
    DrawModelWires(model, positionPlanet, 0.05f, RED);
    rlPopMatrix();

    EndMode3D();
    EndTextureMode();
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
    GuiSlider(layoutRecs[0], TextFormat("Dry/Wet: %.2f", uiParams[curParam]), NULL, &(uiParams[curParam]), params[curParam].min, params[curParam].max);

    curParam = kDecay;
    GuiSliderBar(layoutRecs[1], TextFormat("Decay: %.2f", uiParams[curParam]), NULL, &(uiParams[curParam]), params[curParam].min, params[curParam].max);

    curParam = kDelay;
    // use max delay if set
    GuiSliderBar(layoutRecs[2], TextFormat("Delay: %.2f ms", uiParams[curParam]), NULL, &(uiParams[curParam]), params[curParam].min, (dspParams[kMaxDelay] > 0.0 ? dspParams[kMaxDelay] : params[curParam].max));

    // only send value if updated
    for (int i=0; i < kParameterCount; i++) {
      if (uiParams[i] != dspParams[i]) {
	setParameterValue(i, uiParams[i]);
	// note: only output parameters, if any, will be fired back, hence sync also here
	dspParams[i] = uiParams[i];
      }
    }

    // render the 3D scene
    DrawTexturePro(
		   canvasScene.texture,
		   // flip Y so we get the right texture
		   (Rectangle){ 0.0f, 0.0f , (float)canvasScene.texture.width, -(float)canvasScene.texture.height },
		   (Rectangle){layoutRecs[3].x, layoutRecs[3].y, layoutRecs[3].width, layoutRecs[3].height },
		   (Vector2){ 0, 0 }, 0.0f, WHITE);

    DrawFPS(10, 10);
  }

    // -------------------------------------------------------------------------------------------------- --------------

private:
  // parameters sync with DSP
  // init to 0 so we can use effecively kMaxDelay
  float dspParams[kParameterCount] = {0.0};
  // those used in UI
  float uiParams[kParameterCount] = {0.0};
  // main asset and its positoin
  Model model;
  Vector3 positionSun = { 0.0f, 0.0f, 0.0f };
  Vector3 positionPlanet = { 1.1f, 0.0f, 1.1f, };
  // used for 3D rendering
  Camera camera;
  // drawing separately 3D scene
  RenderTexture2D canvasScene;

  // upper left reference point for UI
  static constexpr Vector2 anchor = { 10, 5 };
  // layout of the GUI
  Rectangle layoutRecs[4] = {
    (Rectangle){ anchor.x + 176, anchor.y + 0, 200, 32 },
    (Rectangle){ anchor.x + 176, anchor.y + 40, 200, 32 },
    (Rectangle){ anchor.x + 176, anchor.y + 80, 200, 32 },
    (Rectangle){ anchor.x + 0, anchor.y + 120, 376, 208 },
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
 
