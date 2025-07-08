
// how often no refresh on idle state, in Hz. 0 to disable animation during idle state
#define UI_REFRESH_RATE 30

#include "RayUI.hpp"
#include "PluginUtils.h"
#include <math.h>

// scenario: a planet orbiting around the sun
// dry/wet: stronger color on the sun for dry, wet on the planet, from semi-transparent to solid. Also dry increase sun rotation, while wet increase planet rotation
// decay: size of the planet (hence gravity) -- bigger the decay, the longer we "bounce" on a small planet
// delay: distance between planet and sun, also orbit speed
// activity: increase sun and planet size + brightness related to wet/dry, also overall bloom effect

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

    // load assets
    obtuseLogo = LoadTexture(resourcesLocation + "obtuse.png");
    // smooth logo for this low resolution display
    SetTextureFilter(obtuseLogo, TEXTURE_FILTER_BILINEAR);
    model = LoadModel(resourcesLocation + "patatoide.obj");

    // camera in the diagonal of origin, high enough to fit the whole scene with selected fov
    camera.position = (Vector3){ 0.0, 6.0, 6.0 };
    // target origin
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    // camera pointing down
    camera.up = (Vector3){ 0.0, 0.0, -1.0};
    // field-of-view Y
    camera.fovy = 45.0f;
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
  
  ~AllpassUI() {
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
    ClearBackground(Color({(unsigned char)(backgroundColor.r/2), (unsigned char)(backgroundColor.g/2), (unsigned char)(backgroundColor.b/2), (unsigned char)(backgroundColor.a/2)}));

    Vector3 positionSun = { 0.0f, 0.0f, 0.0f };

    // compute a ratio from delay, taking into account effective max delay
    float maxDelay = dspParams[kMaxDelay] > 0 ? dspParams[kMaxDelay] : params[kDelay].max;
    float delayRatio = 1.0;
    if (maxDelay > params[kDelay].min) {
      delayRatio = (dspParams[kDelay] - params[kDelay].min) / (maxDelay - params[kDelay].min);
    }

    // vary distance with delay (Pythagora should be here)
    float planetDistance = 1.3 + 1.9 * delayRatio;
    Vector3 positionPlanet = { planetDistance, 0.0f, planetDistance};

    // space occupy by scene depends on delay (distance planet-sun) and decay (planet's size). We will manipulate camera to compensate
    float sceneRatio = delayRatio * 0.8 + 0.2 * (1 - dspParams[kDecay]);
    // adapt camera fov to keep in frame
    camera.fovy = 60.0 - 20 * (1.0 - sceneRatio);
    // move camera target toward front as planet get further away and fov increase to better occupy space
    camera.target = (Vector3){ 0.0f, 0.0f, sceneRatio * 1.75f};

    // with qualya palette should be near magenta for sun and cyan for planet
    // will adapt color (alpha) depending on dry (sun) / wet (planet)
    Color colorSun = GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_PRESSED));
    Color colorPlanet = GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_FOCUSED));
    // max ratio for dry below 0.5, then fade
    float dryRatio = dspParams[kDryWet] < 0.5 ? 1.0 : 1 - (dspParams[kDryWet] - 0.5) * 2;
    colorSun.a =  63 + 192 * dryRatio;
    // reciprocate wet
    float wetRatio = dspParams[kDryWet] > 0.5 ? 1.0 : dspParams[kDryWet] * 2;
    colorPlanet.a = 63 + 192 * wetRatio;

    // tune brightness with activity (from same color to white)
    colorSun = ColorBrightness(colorSun, 0.50 * activity * dryRatio);
    colorPlanet = ColorBrightness(colorPlanet, 0.50 * activity * wetRatio);

    BeginMode3D(camera);

    // base rotation speed 360 degrees per second
    float rotationInc = GetFrameTime() * 360.0;
    // rotation for day mediated by wet ratio
    rotationDay = fmod(rotationDay + rotationInc * (0.1 + 0.4 * wetRatio ), 360.0);
    // for sun by dry ratio -- note that in real-life it takes around 25 days for one rotation
    rotationSun = fmod(rotationSun + rotationInc * (0.05 + 0.15 * dryRatio), 360.0);
    // a year around sun sun, likewise speed-up the 365 days, speed up if delay decrease
    // sepecial case, 0 delay : no more delay
    float rotationYearCoeff = delayRatio > 0.0 ? 0.1 * (20.0 - 19.0 * delayRatio) : 0.0;
    rotationYear = fmod(rotationYear + rotationInc * rotationYearCoeff, 360.0);

    rlPushMatrix();
    // rotation sun
    rlRotatef(rotationSun, 0, 1, 0);
    // Draw sun
    float sunSize = 0.8 + 0.5 * activity  * dryRatio;
    DrawSphereWires(positionSun, sunSize, 4, 8, colorSun);
    rlPopMatrix();

    rlPushMatrix();
    // rotation around sun
    rlRotatef(rotationYear, 0, 1, 0);
    // scale-down planet to match desired proportion
    // Note: wireframe does not work with GLES2 it seems, requires project with raylib compiled set to OPENGL 3
    // size related to decay -- model is scale 10 in original file compared to unit
    float planetSize = 0.03 + 0.04 * (1 - dspParams[kDecay]) + 0.04 * activity * wetRatio;
    // set day rotation
    DrawModelWiresEx(model, positionPlanet, {0, 1, 0}, rotationDay, {planetSize, planetSize, planetSize}, colorPlanet);
    rlPopMatrix();

    EndMode3D();
    EndTextureMode();

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
    GuiLabel(layoutRecs[3], "Allpass");

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
  
  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AllpassUI)
};

// --------------------------------------------------------------------------------------------------------------------

UI* createUI()
{
    return new AllpassUI();
}
 
// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
 
