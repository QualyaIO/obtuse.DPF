
// how often no refresh on idle state, in Hz. 0 to disable animation during idle state
#define UI_REFRESH_RATE 30

#include "RayUI.hpp"
#include "PluginUtils.h"
#include <math.h>

// scenario: tow balls going down a ramp, one straight (dry signal), one oscillating behind (wet signal)
// dry/wet: stronger color for the corresponding ball
// decay: speed of the ball going down 
// delay: amplitude of the oscillation
// activity: brightness related to wet/dry, also overall bloom effect

START_NAMESPACE_DISTRHO

// made-up function to boost lower values
// x expected be 0..1
// a should be between 0 and 1
// clamp output to 0..1
float normie(float x, float a) {
  float val = 0;
  float denom = pow(x, a);
  if (denom != 0) {
    val = x / denom;
  }
  val = val > 1.0 ? 1.0 : val;
  val = val < 0.0 ? 0.0 : val;
  return val;
}

class CombFBUI : public RayUI
{
public:
   /**
      UI class constructor.
      The UI should be initialized to a default state that matches the plugin side.
    */

  CombFBUI() : RayUI(DISTRHO_UI_DEFAULT_WIDTH, DISTRHO_UI_DEFAULT_HEIGHT, UI_REFRESH_RATE, TEXTURE_FILTER_POINT) {
    String resourcesLocation = getResourcesLocation();
    d_stdout("resources location: %s", resourcesLocation.buffer());

    // load assets
    obtuseLogo = LoadTexture(resourcesLocation + "obtuse.png");
    // smooth logo for this low resolution display
    SetTextureFilter(obtuseLogo, TEXTURE_FILTER_BILINEAR);
    model = LoadModel(resourcesLocation + "ramp.obj");

    // camera in the diagonal of origin, high enough to fit the whole scene with selected fov
    camera.position = (Vector3){ 0.0, 6.0, 4.0 };
    // target origin
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    // camera pointing down (...really, this time)
    camera.up = (Vector3){ 0.0, 0.0, 1.0};
    // field-of-view Y
    camera.fovy = 60.0f;
    // camera projection type
    camera.projection = CAMERA_PERSPECTIVE;
    
    // init canvas for 3D scene with place holder size
    canvasScene = LoadRenderTexture(layoutRecs[5].width, layoutRecs[5].height);
    SetTextureFilter(canvasScene.texture, TEXTURE_FILTER_POINT);

    // setup shader and pointer to its parameter
    switch(rlGetVersion()) {
    case RL_OPENGL_21:
      bloom = LoadShader(0, resourcesLocation + "bloom120.fs");
      d_stdout("loading bloom shader, glsl version 120");
      break;
    case  RL_OPENGL_ES_20:
      bloom = LoadShader(0, resourcesLocation + "bloom100.fs");
      d_stdout("loading bloom shader, glsl version 100");
      break;
    case  RL_OPENGL_ES_30:
      bloom = LoadShader(0, resourcesLocation + "bloom300es.fs");
      d_stdout("loading bloom shader, glsl version 300 es");
      break;
      // as per rlgl, 4.3 use 3.3 core functionality
    case RL_OPENGL_43:
    case RL_OPENGL_33:
      bloom = LoadShader(0, resourcesLocation + "bloom330.fs");
      d_stdout("loading bloom shader, glsl version 330");
      break;
      // might not be supported but we try nevertheless
    default:
      bloom = LoadShader(0, resourcesLocation + "bloom330.fs");
      d_stdout("loading bloom shader, glsl version 330 as failsafe");
      break;
    }
    bloomIntensityLoc = GetShaderLocation(bloom, "intensity");

    // init canvas for background
    canvasBackground = LoadRenderTexture(128, 128);
    SetTextureFilter(canvasBackground.texture, TEXTURE_FILTER_POINT);
  }
  
  ~CombFBUI() {
    // unload assets and canvas
    UnloadModel(model);
    UnloadRenderTexture(canvasScene);
    UnloadRenderTexture(canvasBackground);
    UnloadShader(bloom);
    UnloadTexture(obtuseLogo);
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

    float activity = normie(dspParams[kActivity], 0.7);

    ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

    // background, two "stars" flickering at different pace with time and activity
    BeginTextureMode(canvasBackground);
    double bright1 =  -0.5 + 0.25 * sin(GetTime()) + 0.25 * activity;
    double bright2 =  -0.5 + 0.25 * sin(GetTime()/2 + 0.5) + 0.25 * activity;
    DrawRectangle(10, 10, 5, 5, (ColorBrightness(WHITE, bright1)));
    DrawRectangle(90, 100, 2, 2, (ColorBrightness(WHITE, bright2)));
    EndTextureMode();
    // repeat texture on background -- that as well requiring OpenGL > ES2
    int ratiox =  GetScreenWidth() / canvasBackground.texture.width + 1;
    int ratioy =  GetScreenHeight() / canvasBackground.texture.height + 1;
    DrawTexturePro(canvasBackground.texture, {0, 0, (float) canvasBackground.texture.width*ratiox, (float) canvasBackground.texture.height*ratioy}, {0, 0, (float)canvasBackground.texture.width*ratiox, (float)canvasBackground.texture.height*ratioy}, {0, 0}, 0, WHITE);

    // 3D scene
    BeginTextureMode(canvasScene);
    // we have to clear background for anything to display
    // bloom shader will add source and effect, hence half the color to result in the same
    Color backgroundColor = GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR));
    backgroundColor = Color({(unsigned char)(backgroundColor.r/2), (unsigned char)(backgroundColor.g/2), (unsigned char)(backgroundColor.b/2), backgroundColor.a});
    ClearBackground(backgroundColor);

    // compute a ratio from delay, taking into account effective max delay
    float maxDelay = dspParams[kMaxDelay] > 0 ? dspParams[kMaxDelay] : params[kDelay].max;
    float delayRatio = 1.0;
    if (maxDelay > params[kDelay].min) {
      delayRatio = (dspParams[kDelay] - params[kDelay].min) / (maxDelay - params[kDelay].min);
    }

    // vary distance with delay (Pythagora should be here)


    float planetDistance = 1.3 + 1.9 * delayRatio;
    Vector3 positionPlanet = { planetDistance, 0.0f, planetDistance};

    // with qualya palette should be near magenta for sun and cyan for planet
    // will adapt color (alpha) depending on dry (sun) / wet (planet)
    Color colorDry = GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_PRESSED));
    Color colorWet = GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_FOCUSED));
    // max ratio for dry below 0.5, then fade
    float dryRatio = dspParams[kDryWet] < 0.5 ? 1.0 : 1 - (dspParams[kDryWet] - 0.5) * 2;
    colorDry.a =  63 + 192 * dryRatio;
    // reciprocate wet
    float wetRatio = dspParams[kDryWet] > 0.5 ? 1.0 : dspParams[kDryWet] * 2;
    colorWet.a = 63 + 192 * wetRatio;

    // tune brightness with activity (from same color to white)
    colorDry = ColorBrightness(colorDry, 0.50 * activity * dryRatio);
    colorWet = ColorBrightness(colorWet, 0.50 * activity * wetRatio);

    BeginMode3D(camera);

    // ramp, 5mm long in freecad
    float rampLength = 5.0;
    // will serve as a clip plane
    Vector3 positionClip = {0, -3*rampLength, 0};
    DrawCube(positionClip, 10, 0.5, 10, backgroundColor);

    // advance ramp, speed related to decay, from 1 unit second (max decay) to 5 (one tile, at min decay)
    d_stdout("fractime: %f, pos: %f)", GetFrameTime(), positionRamp.y);
    positionRamp.y -=  GetFrameTime() * (5 - 4 * dspParams[kDecay]);
    if (positionRamp.y <= -rampLength) {
      positionRamp.y = 0;
    }

    // duplicate ramp tile
    for (int i=0; i<7; i++) {
      Vector3 positionRampTile = {positionRamp.x, positionClip.y + rampLength * i +  positionRamp.y, positionRamp.z};
      // tune color as depth goes
      Color colorTile = ColorContrast(colorDry, -0.20 + 0.05*i);

      DrawModelWiresEx(model, positionRampTile, {0, 0, 0},  0.0, {1, 1, 1}, colorTile);
    }

    EndMode3D();
    EndTextureMode();

    float intensity = activity;
    SetShaderValue(bloom, bloomIntensityLoc, &activity, SHADER_UNIFORM_FLOAT);
  }
  
  void onCanvasDisplay() override
  {

    // background for window, take the darkest color and some more
    Color backgroundWindow = ColorBrightness(GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_NORMAL)), -0.38);
    ClearBackground(backgroundWindow);

    // header
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_RIGHT);
    GuiLabel(layoutRecs[1], "Obtuse");
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
    DrawTexturePro(obtuseLogo, {0, 0, (float)obtuseLogo.width, (float)obtuseLogo.height}, layoutRecs[2], {0, 0}, 0, WHITE);
    GuiLabel(layoutRecs[3], "CombFB");

    // background for UI -- but nicer without?
    //DrawRectangle(layoutRecs[4].x,layoutRecs[4].y, layoutRecs[4].width, layoutRecs[4].height, GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

    // render the 3D scene, apply shader is correctly loaded
    if (IsShaderValid(bloom)) {
      BeginShaderMode(bloom);
    }
    DrawTexturePro(
		   canvasScene.texture,
		   // flip Y so we get the right texture
		   (Rectangle){ 0.0f, 0.0f , (float)canvasScene.texture.width, -(float)canvasScene.texture.height },
		   {layoutRecs[5].x, layoutRecs[5].y, layoutRecs[5].width, layoutRecs[5].height},
		   (Vector2){ 0, 0 }, 0.0f, WHITE);
    if (IsShaderValid(bloom)) {
      EndShaderMode();
    }

    // sync ui and dsp
    for (int i=0; i < kParameterCount; i++) {
      uiParams[i] = dspParams[i];
    }
    // we'll ease copy and paste
    int curParam = 0;

    curParam = kDryWet;
    GuiSlider(layoutRecs[6], TextFormat("Dry/Wet: %.2f", uiParams[curParam]), NULL, &(uiParams[curParam]), params[curParam].min, params[curParam].max);

    curParam = kDecay;
    GuiSliderBar(layoutRecs[7], TextFormat("Decay: %.2f", uiParams[curParam]), NULL, &(uiParams[curParam]), params[curParam].min, params[curParam].max);

    curParam = kDelay;
    // use max delay if set
    GuiSliderBar(layoutRecs[8], TextFormat("Delay: %.2f ms", uiParams[curParam]), NULL, &(uiParams[curParam]), params[curParam].min, (dspParams[kMaxDelay] > 0.0 ? dspParams[kMaxDelay] : params[curParam].max));

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
  // init to 0 so we can use effecively kMaxDelay
  float dspParams[kParameterCount] = {0.0};
  // those used in UI
  float uiParams[kParameterCount] = {0.0};
  // main asset and its positoin
  Model model;
    // used for 3D rendering
  Camera camera;
  // drawing separately 3D scene
  RenderTexture2D canvasScene;
  // background
  RenderTexture2D canvasBackground;
  // keep trace of the different rotation between calls
  float rotationDay = 0.0;
  float rotationSun = 0.0;
  float rotationYear = 0.0;
  // base position of ramp, will advance with time
  Vector3 positionRamp = { 0.0f, 0.0f, 0.0f };

  // we want a nice effect
  Shader bloom = {0, 0};
  int bloomIntensityLoc;
  // one nice logo
  Texture2D obtuseLogo;

  // upper left reference point for UI
  static constexpr Vector2 anchor = {0, 0};
  // layout of the GUI
  const Rectangle layoutRecs[9] = {
    (Rectangle){ anchor.x + 0, anchor.y + 0, 392, 384 },
    (Rectangle){ anchor.x + 8, anchor.y + 8, 160, 32 },
    (Rectangle){ anchor.x + 176, anchor.y + 0, 40, 40 },
    (Rectangle){ anchor.x + 224, anchor.y + 8, 160, 32 },
    (Rectangle){ anchor.x + 8, anchor.y + 48, 376, 328 },
    (Rectangle){ anchor.x + 8, anchor.y + 48, 376, 208 },
    (Rectangle){ anchor.x + 184, anchor.y + 264, 200, 32 },
    (Rectangle){ anchor.x + 184, anchor.y + 304, 200, 32 },
    (Rectangle){ anchor.x + 184, anchor.y + 344, 200, 32 },
  };
  
  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CombFBUI)
};

// --------------------------------------------------------------------------------------------------------------------

UI* createUI()
{
    return new CombFBUI();
}
 
// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
 
