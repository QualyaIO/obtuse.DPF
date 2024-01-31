
#include "MetaDrummer.hpp"

void MetaDrummer::switchTo(int kit) {
  if (kit < 0) {
    kit = 0;
  } else if (kit > getNbKits()-1) {
    kit = getNbKits()-1;
  }
  selectedKit = kit;
  initProc();
}

void MetaDrummer::setSamplerate(float sr) {
   switch(selectedKit) {
   case 1:
      synthDrummerBbox_Voice_setSamplerate(processorBbox, float_to_fix(sr));
      break;
   case 2:
      synthDrummerFoleyType_Voice_setSamplerate(processorFoleyType, float_to_fix(sr));
      break;
   case 3:
      synthDrummerNes_Voice_setSamplerate(processorNes, float_to_fix(sr));
      break;
   case 4:
      synthDrummerTamaRockstar_Voice_setSamplerate(processorTamaRockstar, float_to_fix(sr));
      break;
   case 0:
   default:
      synthDrummer_Voice_setSamplerate(processor808, float_to_fix(sr));
      break;
   }
}

void MetaDrummer::setPitchBend(float semitones) {
   switch(selectedKit) {
   case 1:
      synthDrummerBbox_Voice_synthPitchBend(processorBbox, float_to_fix(semitones));
      break;
   case 2:
      synthDrummerFoleyType_Voice_synthPitchBend(processorFoleyType, float_to_fix(semitones));
      break;
   case 3:
      synthDrummerNes_Voice_synthPitchBend(processorNes, float_to_fix(semitones));
      break;
   case 4:
      synthDrummerTamaRockstar_Voice_synthPitchBend(processorTamaRockstar, float_to_fix(semitones));
      break;
   case 0:
   default:
      synthDrummer_Voice_synthPitchBend(processor808, float_to_fix(semitones));
      break;
   }
}

void MetaDrummer::setReuse(bool flag) {
   switch(selectedKit) {
   case 1:
      synthDrummerBbox_Voice_setReuse(processorBbox, flag);
      break;
   case 2:
      synthDrummerFoleyType_Voice_setReuse(processorFoleyType, flag);
      break;
   case 3:
      synthDrummerNes_Voice_setReuse(processorNes, flag);
      break;
   case 4:
      synthDrummerTamaRockstar_Voice_setReuse(processorTamaRockstar, flag);
      break;
   case 0:
   default:
      synthDrummer_Voice_setReuse(processor808, flag);
      break;
   }
}

void MetaDrummer::noteOn(int note, int velocity, int channel) {
   switch(selectedKit) {
   case 1:
     synthDrummerBbox_Voice_noteOn(processorBbox, note, velocity, channel);
     break;
   case 2:
     synthDrummerFoleyType_Voice_noteOn(processorFoleyType, note, velocity, channel);
     break;
   case 3:
     synthDrummerNes_Voice_noteOn(processorNes, note, velocity, channel);
     break;
   case 4:
     synthDrummerTamaRockstar_Voice_noteOn(processorTamaRockstar, note, velocity, channel);
     break;
   case 0:
   default:
     synthDrummer_Voice_noteOn(processor808, note, velocity, channel);
     break;
   }
}

void MetaDrummer::noteOff(int note, int channel) {
   switch(selectedKit) {
   case 1:
     synthDrummerBbox_Voice_noteOff(processorBbox, note, channel);
     break;
   case 2:
     synthDrummerFoleyType_Voice_noteOff(processorFoleyType, note, channel);
     break;
   case 3:
     synthDrummerNes_Voice_noteOff(processorNes, note, channel);
     break;
   case 4:
     synthDrummerTamaRockstar_Voice_noteOff(processorTamaRockstar, note, channel);
     break;
   case 0:
   default:
     synthDrummer_Voice_noteOff(processor808, note, channel);
     break;
   }
}

void MetaDrummer::process(int nb, fix16_t (&oBuffer)[128]) {
   switch(selectedKit) {
   case 1:
     synthDrummerBbox_Voice_process_bufferTo(processorBbox, nb, oBuffer);
     break;
   case 2:
     synthDrummerFoleyType_Voice_process_bufferTo(processorFoleyType, nb, oBuffer);
     break;
   case 3:
     synthDrummerNes_Voice_process_bufferTo(processorNes, nb, oBuffer);
     break;
   case 4:
     synthDrummerTamaRockstar_Voice_process_bufferTo(processorTamaRockstar, nb, oBuffer);
     break;
   case 0:
   default:
     synthDrummer_Voice_process_bufferTo(processor808, nb, oBuffer);
     break;
   }
}

void MetaDrummer::initProc() {
   switch(selectedKit) {
   case 1:
      synthDrummerBbox_Voice_process_init(processorBbox);
      break;
   case 2:
      synthDrummerFoleyType_Voice_process_init(processorFoleyType);
      break;
   case 3:
      synthDrummerNes_Voice_process_init(processorNes);
      break;
   case 4:
      synthDrummerTamaRockstar_Voice_process_init(processorTamaRockstar);
      break;
   case 0:
   default:
      synthDrummer_Voice_process_init(processor808);
      break;
   }
}
