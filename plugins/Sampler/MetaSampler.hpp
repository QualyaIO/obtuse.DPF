
#pragma once

#include "synthSampler.h"
#include "synthSamplerCello.h"
#include "synthSamplerClarinet.h"
#include "synthSamplerPiano.h"
#include "synthSamplerAmbientLead.h"
#include "synthSamplerAuthenticStrings.h"
#include "synthSamplerBoesendorferGrandPiano.h"
#include "synthSamplerCelesta.h"
#include "synthSamplerSoftClarinet.h"
#include "synthSamplerClassicSuitcaseMk1.h"
#include "synthSamplerCosmos.h"
#include "synthSamplerDreamVoice.h"
#include "synthSamplerDrivenSynthLead.h"
#include "synthSamplerGlassMarimbaSoft.h"
#include "synthSamplerIndianSitar.h"
#include "synthSamplerJapaneseShakuhachiFlute.h"
#include "synthSamplerKoto.h"
#include "synthSamplerMagnificentDrone.h"
#include "synthSamplerMysticVibes.h"
#include "synthSamplerPowerFifths.h"
#include "synthSamplerPulsingPatterns.h"
#include "synthSamplerShootingStar.h"
#include "synthSamplerTimeWarp.h"
#include "synthSamplerTranquility.h"
#include "synthSamplerVocalCluster.h"
#include "synthSamplerZenGarden.h"

// handle the various sampler
// adapted from VCV version
struct MetaSampler {
   
   // to sync with the actual number of samples...
   static int getNbSamples() { return 26;};
   // to sync with biggest wave file
   static int getMaxLoopSize() { return 100000;};
   static const char* getSampleName(int sample) {
      switch(sample) {
      case 1:
         return "Cello";
      case 2:
         return "Clarinet";
      case 3:
         return "Piano";
      case 4:
         return "AmbientLead";
      case 5:
         return "AuthenticStrings";
      case 6:
         return "BoesendorferGrandPiano";
      case 7:
         return "Celesta";
      case 8:
         return "SoftClarinet";
      case 9:
         return "ClassicSuitcaseMk1";
      case 10:
         return "Cosmos";
      case 11:
         return "DreamVoice";
      case 12:
         return "DrivenSynthLead";
      case 13:
         return "GlassMarimbaSoft";
      case 14:
         return "IndianSitar";
      case 15:
         return "JapaneseShakuhachiFlute";
      case 16:
         return "Koto";
      case 17:
         return "MagnificentDrone";
      case 18:
         return "MysticVibes";
      case 19:
         return "PowerFifths";
      case 20:
         return "PulsingPatterns";
      case 21:
         return "ShootingStar";
      case 22:
         return "TimeWarp";
      case 23:
         return "Tranquility";
      case 24:
         return "VocalCluster";
      case 25:
         return "ZenGarden";
      case 0:
      default:
         return "Ocarina";
      }
   };

   // will (re)init corresponding processor -- not checking if we actually change sample
   void switchTo(int sample);
   // wrapping functions
   void setSamplerate(float sr);
   void noteOn(int note, int velocity, int channel);
   void noteOff(int note, int channel);
   bool getLoop();
   int getLoopStart();
   int getLoopEnd();
   int getSize();
   void setLoop(bool loop);
   void setLoopStart(int loopS);
   void setLoopEnd(int loopE);
   void setPitchBend(float semitones);
   void setSustain(bool flag);
   void setReuse(bool flag);
   void process(int nb, fix16_t (&oBuffer)[128]);

private:
   // all variants of the processor
   synthSampler_Voice_process_type processorOcarina;
   synthSamplerCello_Voice_process_type processorCello;
   synthSamplerClarinet_Voice_process_type processorClarinet;
   synthSamplerPiano_Voice_process_type processorPiano;
   synthSamplerAmbientLead_Voice_process_type processorAmbientLead;
   synthSamplerAuthenticStrings_Voice_process_type processorAuthenticStrings;
   synthSamplerBoesendorferGrandPiano_Voice_process_type processorBoesendorferGrandPiano;
   synthSamplerCelesta_Voice_process_type processorCelesta;
   synthSamplerSoftClarinet_Voice_process_type processorSoftClarinet;
   synthSamplerClassicSuitcaseMk1_Voice_process_type processorClassicSuitcaseMk1;
   synthSamplerCosmos_Voice_process_type processorCosmos;
   synthSamplerDreamVoice_Voice_process_type processorDreamVoice;
   synthSamplerDrivenSynthLead_Voice_process_type processorDrivenSynthLead;
   synthSamplerGlassMarimbaSoft_Voice_process_type processorGlassMarimbaSoft;
   synthSamplerIndianSitar_Voice_process_type processorIndianSitar;
   synthSamplerJapaneseShakuhachiFlute_Voice_process_type processorJapaneseShakuhachiFlute;
   synthSamplerKoto_Voice_process_type processorKoto;
   synthSamplerMagnificentDrone_Voice_process_type processorMagnificentDrone;
   synthSamplerMysticVibes_Voice_process_type processorMysticVibes;
   synthSamplerPowerFifths_Voice_process_type processorPowerFifths;
   synthSamplerPulsingPatterns_Voice_process_type processorPulsingPatterns;
   synthSamplerShootingStar_Voice_process_type processorShootingStar;
   synthSamplerTimeWarp_Voice_process_type processorTimeWarp;
   synthSamplerTranquility_Voice_process_type processorTranquility;
   synthSamplerVocalCluster_Voice_process_type processorVocalCluster;
   synthSamplerZenGarden_Voice_process_type processorZenGarden;
   // currently selected sample
   int selectedSample = -1;

   // initialize current processor
   void initProc();
};
