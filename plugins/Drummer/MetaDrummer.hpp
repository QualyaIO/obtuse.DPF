
#pragma once

#include "synthDrummer.h"
#include "synthDrummerBbox.h"
#include "synthDrummerFoleyType.h"
#include "synthDrummerNes.h"
#include "synthDrummerTamaRockstar.h"

// handle the various drummer
// adapted from VCV version
struct MetaDrummer {

   // to sync with the actual number of drum kits...
   static int getNbKits() { return 5;};
   static const char* getKitName(int kit) {
      switch(kit) {
      case 1:
         return "Bbox";
      case 2:
         return "FoleyType";
      case 3:
         return "NES";
      case 4:
         return "TamaRockstar";
      case 0:
      default:
         return "808";
      }
   };

   // will (re)init corresponding processor upon change
   void switchTo(int kit);
   // wrapping functions
   void setSamplerate(float sr);
   void noteOn(int note, int velocity, int channel);
   void noteOff(int note, int channel);
   void setPitchBend(float semitones);
   void process(int nb, fix16_t (&oBuffer)[128]);

private:
   // all variants of the drummer
   synthDrummer_Voice_process_type processor808;
   synthDrummerBbox_Voice_process_type processorBbox;
   synthDrummerFoleyType_Voice_process_type processorFoleyType;
   synthDrummerNes_Voice_process_type processorNes;
   synthDrummerTamaRockstar_Voice_process_type processorTamaRockstar;
   // currently selected kit
   int selectedKit = -1;

   // initialize current processor
   void initProc();
};
