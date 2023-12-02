
#include "MetaSampler.hpp"

void MetaSampler::switchTo(int sample) {
  if (sample < 0) {
    sample = 0;
  } else if (sample > getNbSamples()-1) {
    sample = getNbSamples()-1;
  }
  selectedSample = sample;
  initProc();
}

void MetaSampler::setSamplerate(float sr) {
   switch(selectedSample) {
   case 1:
      synthSamplerCello_Voice_setSamplerate(processorCello, float_to_fix(sr));
      break;
   case 2:
      synthSamplerClarinet_Voice_setSamplerate(processorClarinet, float_to_fix(sr));
      break;
   case 3:
      synthSamplerPiano_Voice_setSamplerate(processorPiano, float_to_fix(sr));
      break;
   case 4:
      synthSamplerAmbientLead_Voice_setSamplerate(processorAmbientLead, float_to_fix(sr));
      break;
   case 5:
      synthSamplerAuthenticStrings_Voice_setSamplerate(processorAuthenticStrings, float_to_fix(sr));
      break;
   case 6:
      synthSamplerBoesendorferGrandPiano_Voice_setSamplerate(processorBoesendorferGrandPiano, float_to_fix(sr));
      break;
   case 7:
      synthSamplerCelesta_Voice_setSamplerate(processorCelesta, float_to_fix(sr));
      break;
   case 8:
      synthSamplerClarinets_Voice_setSamplerate(processorClarinets, float_to_fix(sr));
      break;
   case 9:
      synthSamplerClassicSuitcaseMk1_Voice_setSamplerate(processorClassicSuitcaseMk1, float_to_fix(sr));
      break;
   case 10:
      synthSamplerCosmos_Voice_setSamplerate(processorCosmos, float_to_fix(sr));
      break;
   case 11:
      synthSamplerDreamVoice_Voice_setSamplerate(processorDreamVoice, float_to_fix(sr));
      break;
   case 12:
      synthSamplerDrivenSynthLead_Voice_setSamplerate(processorDrivenSynthLead, float_to_fix(sr));
      break;
   case 13:
      synthSamplerGlassMarimbaSoft_Voice_setSamplerate(processorGlassMarimbaSoft, float_to_fix(sr));
      break;
   case 14:
      synthSamplerIndianSitar_Voice_setSamplerate(processorIndianSitar, float_to_fix(sr));
      break;
   case 15:
      synthSamplerJapaneseShakuhachiFlute_Voice_setSamplerate(processorJapaneseShakuhachiFlute, float_to_fix(sr));
      break;
   case 16:
      synthSamplerKoto_Voice_setSamplerate(processorKoto, float_to_fix(sr));
      break;
   case 17:
      synthSamplerMagnificentDrone_Voice_setSamplerate(processorMagnificentDrone, float_to_fix(sr));
      break;
   case 18:
      synthSamplerMysticVibes_Voice_setSamplerate(processorMysticVibes, float_to_fix(sr));
      break;
   case 19:
      synthSamplerPowerFifths_Voice_setSamplerate(processorPowerFifths, float_to_fix(sr));
      break;
   case 20:
      synthSamplerPulsingPatterns_Voice_setSamplerate(processorPulsingPatterns, float_to_fix(sr));
      break;
   case 21:
      synthSamplerShootingStar_Voice_setSamplerate(processorShootingStar, float_to_fix(sr));
      break;
   case 22:
      synthSamplerTimeWarp_Voice_setSamplerate(processorTimeWarp, float_to_fix(sr));
      break;
   case 23:
      synthSamplerTranquility_Voice_setSamplerate(processorTranquility, float_to_fix(sr));
      break;
   case 24:
      synthSamplerVocalCluster_Voice_setSamplerate(processorVocalCluster, float_to_fix(sr));
      break;
   case 25:
      synthSamplerZenGarden_Voice_setSamplerate(processorZenGarden, float_to_fix(sr));
      break;
   case 0:
   default:
      synthSampler_Voice_setSamplerate(processorOcarina, float_to_fix(sr));
      break;
   }
}

int MetaSampler::getSize() {
   switch(selectedSample) {
   case 1:
     return synthSamplerCello_Voice_synthGetSize(processorCello);
   case 2:
     return synthSamplerClarinet_Voice_synthGetSize(processorClarinet);
   case 3:
     return synthSamplerPiano_Voice_synthGetSize(processorPiano);
   case 4:
     return synthSamplerAmbientLead_Voice_synthGetSize(processorAmbientLead);
   case 5:
     return synthSamplerAuthenticStrings_Voice_synthGetSize(processorAuthenticStrings);
   case 6:
     return synthSamplerBoesendorferGrandPiano_Voice_synthGetSize(processorBoesendorferGrandPiano);
   case 7:
     return synthSamplerCelesta_Voice_synthGetSize(processorCelesta);
   case 8:
     return synthSamplerClarinets_Voice_synthGetSize(processorClarinets);
   case 9:
     return synthSamplerClassicSuitcaseMk1_Voice_synthGetSize(processorClassicSuitcaseMk1);
   case 10:
     return synthSamplerCosmos_Voice_synthGetSize(processorCosmos);
   case 11:
     return synthSamplerDreamVoice_Voice_synthGetSize(processorDreamVoice);
   case 12:
     return synthSamplerDrivenSynthLead_Voice_synthGetSize(processorDrivenSynthLead);
   case 13:
     return synthSamplerGlassMarimbaSoft_Voice_synthGetSize(processorGlassMarimbaSoft);
   case 14:
     return synthSamplerIndianSitar_Voice_synthGetSize(processorIndianSitar);
   case 15:
     return synthSamplerJapaneseShakuhachiFlute_Voice_synthGetSize(processorJapaneseShakuhachiFlute);
   case 16:
     return synthSamplerKoto_Voice_synthGetSize(processorKoto);
   case 17:
     return synthSamplerMagnificentDrone_Voice_synthGetSize(processorMagnificentDrone);
   case 18:
     return synthSamplerMysticVibes_Voice_synthGetSize(processorMysticVibes);
   case 19:
     return synthSamplerPowerFifths_Voice_synthGetSize(processorPowerFifths);
   case 20:
     return synthSamplerPulsingPatterns_Voice_synthGetSize(processorPulsingPatterns);
   case 21:
     return synthSamplerShootingStar_Voice_synthGetSize(processorShootingStar);
   case 22:
     return synthSamplerTimeWarp_Voice_synthGetSize(processorTimeWarp);
   case 23:
     return synthSamplerTranquility_Voice_synthGetSize(processorTranquility);
   case 24:
     return synthSamplerVocalCluster_Voice_synthGetSize(processorVocalCluster);
   case 25:
     return synthSamplerZenGarden_Voice_synthGetSize(processorZenGarden);
   case 0:
   default:
     return synthSampler_Voice_synthGetSize(processorOcarina);
   }
}

bool MetaSampler::getLoop() {
   switch(selectedSample) {
   case 1:
     return synthSamplerCello_Voice_synthGetLoop(processorCello);
   case 2:
     return synthSamplerClarinet_Voice_synthGetLoop(processorClarinet);
   case 3:
     return synthSamplerPiano_Voice_synthGetLoop(processorPiano);
   case 4:
     return synthSamplerAmbientLead_Voice_synthGetLoop(processorAmbientLead);
   case 5:
     return synthSamplerAuthenticStrings_Voice_synthGetLoop(processorAuthenticStrings);
   case 6:
     return synthSamplerBoesendorferGrandPiano_Voice_synthGetLoop(processorBoesendorferGrandPiano);
   case 7:
     return synthSamplerCelesta_Voice_synthGetLoop(processorCelesta);
   case 8:
     return synthSamplerClarinets_Voice_synthGetLoop(processorClarinets);
   case 9:
     return synthSamplerClassicSuitcaseMk1_Voice_synthGetLoop(processorClassicSuitcaseMk1);
   case 10:
     return synthSamplerCosmos_Voice_synthGetLoop(processorCosmos);
   case 11:
     return synthSamplerDreamVoice_Voice_synthGetLoop(processorDreamVoice);
   case 12:
     return synthSamplerDrivenSynthLead_Voice_synthGetLoop(processorDrivenSynthLead);
   case 13:
     return synthSamplerGlassMarimbaSoft_Voice_synthGetLoop(processorGlassMarimbaSoft);
   case 14:
     return synthSamplerIndianSitar_Voice_synthGetLoop(processorIndianSitar);
   case 15:
     return synthSamplerJapaneseShakuhachiFlute_Voice_synthGetLoop(processorJapaneseShakuhachiFlute);
   case 16:
     return synthSamplerKoto_Voice_synthGetLoop(processorKoto);
   case 17:
     return synthSamplerMagnificentDrone_Voice_synthGetLoop(processorMagnificentDrone);
   case 18:
     return synthSamplerMysticVibes_Voice_synthGetLoop(processorMysticVibes);
   case 19:
     return synthSamplerPowerFifths_Voice_synthGetLoop(processorPowerFifths);
   case 20:
     return synthSamplerPulsingPatterns_Voice_synthGetLoop(processorPulsingPatterns);
   case 21:
     return synthSamplerShootingStar_Voice_synthGetLoop(processorShootingStar);
   case 22:
     return synthSamplerTimeWarp_Voice_synthGetLoop(processorTimeWarp);
   case 23:
     return synthSamplerTranquility_Voice_synthGetLoop(processorTranquility);
   case 24:
     return synthSamplerVocalCluster_Voice_synthGetLoop(processorVocalCluster);
   case 25:
     return synthSamplerZenGarden_Voice_synthGetLoop(processorZenGarden);
   case 0:
   default:
     return synthSampler_Voice_synthGetLoop(processorOcarina);
   }
}

int MetaSampler::getLoopStart() {
   switch(selectedSample) {
   case 1:
     return synthSamplerCello_Voice_synthGetLoopStart(processorCello);
   case 2:
     return synthSamplerClarinet_Voice_synthGetLoopStart(processorClarinet);
   case 3:
     return synthSamplerPiano_Voice_synthGetLoopStart(processorPiano);
   case 4:
     return synthSamplerAmbientLead_Voice_synthGetLoopStart(processorAmbientLead);
   case 5:
     return synthSamplerAuthenticStrings_Voice_synthGetLoopStart(processorAuthenticStrings);
   case 6:
     return synthSamplerBoesendorferGrandPiano_Voice_synthGetLoopStart(processorBoesendorferGrandPiano);
   case 7:
     return synthSamplerCelesta_Voice_synthGetLoopStart(processorCelesta);
   case 8:
     return synthSamplerClarinets_Voice_synthGetLoopStart(processorClarinets);
   case 9:
     return synthSamplerClassicSuitcaseMk1_Voice_synthGetLoopStart(processorClassicSuitcaseMk1);
   case 10:
     return synthSamplerCosmos_Voice_synthGetLoopStart(processorCosmos);
   case 11:
     return synthSamplerDreamVoice_Voice_synthGetLoopStart(processorDreamVoice);
   case 12:
     return synthSamplerDrivenSynthLead_Voice_synthGetLoopStart(processorDrivenSynthLead);
   case 13:
     return synthSamplerGlassMarimbaSoft_Voice_synthGetLoopStart(processorGlassMarimbaSoft);
   case 14:
     return synthSamplerIndianSitar_Voice_synthGetLoopStart(processorIndianSitar);
   case 15:
     return synthSamplerJapaneseShakuhachiFlute_Voice_synthGetLoopStart(processorJapaneseShakuhachiFlute);
   case 16:
     return synthSamplerKoto_Voice_synthGetLoopStart(processorKoto);
   case 17:
     return synthSamplerMagnificentDrone_Voice_synthGetLoopStart(processorMagnificentDrone);
   case 18:
     return synthSamplerMysticVibes_Voice_synthGetLoopStart(processorMysticVibes);
   case 19:
     return synthSamplerPowerFifths_Voice_synthGetLoopStart(processorPowerFifths);
   case 20:
     return synthSamplerPulsingPatterns_Voice_synthGetLoopStart(processorPulsingPatterns);
   case 21:
     return synthSamplerShootingStar_Voice_synthGetLoopStart(processorShootingStar);
   case 22:
     return synthSamplerTimeWarp_Voice_synthGetLoopStart(processorTimeWarp);
   case 23:
     return synthSamplerTranquility_Voice_synthGetLoopStart(processorTranquility);
   case 24:
     return synthSamplerVocalCluster_Voice_synthGetLoopStart(processorVocalCluster);
   case 25:
     return synthSamplerZenGarden_Voice_synthGetLoopStart(processorZenGarden);
   case 0:
   default:
     return synthSampler_Voice_synthGetLoopStart(processorOcarina);
   }
}

int MetaSampler::getLoopEnd() {
   switch(selectedSample) {
   case 1:
     return synthSamplerCello_Voice_synthGetLoopEnd(processorCello);
   case 2:
     return synthSamplerClarinet_Voice_synthGetLoopEnd(processorClarinet);
   case 3:
     return synthSamplerPiano_Voice_synthGetLoopEnd(processorPiano);
   case 4:
     return synthSamplerAmbientLead_Voice_synthGetLoopEnd(processorAmbientLead);
   case 5:
     return synthSamplerAuthenticStrings_Voice_synthGetLoopEnd(processorAuthenticStrings);
   case 6:
     return synthSamplerBoesendorferGrandPiano_Voice_synthGetLoopEnd(processorBoesendorferGrandPiano);
   case 7:
     return synthSamplerCelesta_Voice_synthGetLoopEnd(processorCelesta);
   case 8:
     return synthSamplerClarinets_Voice_synthGetLoopEnd(processorClarinets);
   case 9:
     return synthSamplerClassicSuitcaseMk1_Voice_synthGetLoopEnd(processorClassicSuitcaseMk1);
   case 10:
     return synthSamplerCosmos_Voice_synthGetLoopEnd(processorCosmos);
   case 11:
     return synthSamplerDreamVoice_Voice_synthGetLoopEnd(processorDreamVoice);
   case 12:
     return synthSamplerDrivenSynthLead_Voice_synthGetLoopEnd(processorDrivenSynthLead);
   case 13:
     return synthSamplerGlassMarimbaSoft_Voice_synthGetLoopEnd(processorGlassMarimbaSoft);
   case 14:
     return synthSamplerIndianSitar_Voice_synthGetLoopEnd(processorIndianSitar);
   case 15:
     return synthSamplerJapaneseShakuhachiFlute_Voice_synthGetLoopEnd(processorJapaneseShakuhachiFlute);
   case 16:
     return synthSamplerKoto_Voice_synthGetLoopEnd(processorKoto);
   case 17:
     return synthSamplerMagnificentDrone_Voice_synthGetLoopEnd(processorMagnificentDrone);
   case 18:
     return synthSamplerMysticVibes_Voice_synthGetLoopEnd(processorMysticVibes);
   case 19:
     return synthSamplerPowerFifths_Voice_synthGetLoopEnd(processorPowerFifths);
   case 20:
     return synthSamplerPulsingPatterns_Voice_synthGetLoopEnd(processorPulsingPatterns);
   case 21:
     return synthSamplerShootingStar_Voice_synthGetLoopEnd(processorShootingStar);
   case 22:
     return synthSamplerTimeWarp_Voice_synthGetLoopEnd(processorTimeWarp);
   case 23:
     return synthSamplerTranquility_Voice_synthGetLoopEnd(processorTranquility);
   case 24:
     return synthSamplerVocalCluster_Voice_synthGetLoopEnd(processorVocalCluster);
   case 25:
     return synthSamplerZenGarden_Voice_synthGetLoopEnd(processorZenGarden);
   case 0:
   default:
     return synthSampler_Voice_synthGetLoopEnd(processorOcarina);
   }
}

void MetaSampler::setLoop(bool param) {
   switch(selectedSample) {
   case 1:
     synthSamplerCello_Voice_synthSetLoop(processorCello, param);
     break;
   case 2:
     synthSamplerClarinet_Voice_synthSetLoop(processorClarinet, param);
      break;
   case 3:
     synthSamplerPiano_Voice_synthSetLoop(processorPiano, param);
      break;
   case 4:
     synthSamplerAmbientLead_Voice_synthSetLoop(processorAmbientLead, param);
      break;
   case 5:
     synthSamplerAuthenticStrings_Voice_synthSetLoop(processorAuthenticStrings, param);
      break;
   case 6:
     synthSamplerBoesendorferGrandPiano_Voice_synthSetLoop(processorBoesendorferGrandPiano, param);
      break;
   case 7:
     synthSamplerCelesta_Voice_synthSetLoop(processorCelesta, param);
      break;
   case 8:
     synthSamplerClarinets_Voice_synthSetLoop(processorClarinets, param);
      break;
   case 9:
     synthSamplerClassicSuitcaseMk1_Voice_synthSetLoop(processorClassicSuitcaseMk1, param);
      break;
   case 10:
     synthSamplerCosmos_Voice_synthSetLoop(processorCosmos, param);
      break;
   case 11:
     synthSamplerDreamVoice_Voice_synthSetLoop(processorDreamVoice, param);
      break;
   case 12:
     synthSamplerDrivenSynthLead_Voice_synthSetLoop(processorDrivenSynthLead, param);
      break;
   case 13:
     synthSamplerGlassMarimbaSoft_Voice_synthSetLoop(processorGlassMarimbaSoft, param);
      break;
   case 14:
     synthSamplerIndianSitar_Voice_synthSetLoop(processorIndianSitar, param);
      break;
   case 15:
     synthSamplerJapaneseShakuhachiFlute_Voice_synthSetLoop(processorJapaneseShakuhachiFlute, param);
      break;
   case 16:
     synthSamplerKoto_Voice_synthSetLoop(processorKoto, param);
      break;
   case 17:
     synthSamplerMagnificentDrone_Voice_synthSetLoop(processorMagnificentDrone, param);
      break;
   case 18:
     synthSamplerMysticVibes_Voice_synthSetLoop(processorMysticVibes, param);
      break;
   case 19:
     synthSamplerPowerFifths_Voice_synthSetLoop(processorPowerFifths, param);
      break;
   case 20:
     synthSamplerPulsingPatterns_Voice_synthSetLoop(processorPulsingPatterns, param);
      break;
   case 21:
     synthSamplerShootingStar_Voice_synthSetLoop(processorShootingStar, param);
      break;
   case 22:
     synthSamplerTimeWarp_Voice_synthSetLoop(processorTimeWarp, param);
      break;
   case 23:
     synthSamplerTranquility_Voice_synthSetLoop(processorTranquility, param);
      break;
   case 24:
     synthSamplerVocalCluster_Voice_synthSetLoop(processorVocalCluster, param);
      break;
   case 25:
     synthSamplerZenGarden_Voice_synthSetLoop(processorZenGarden, param);
      break;
   case 0:
   default:
     synthSampler_Voice_synthSetLoop(processorOcarina, param);
      break;
   }
}

void MetaSampler::setLoopStart(int param) {
   switch(selectedSample) {
   case 1:
     synthSamplerCello_Voice_synthSetLoopStart(processorCello, param);
      break;
   case 2:
     synthSamplerClarinet_Voice_synthSetLoopStart(processorClarinet, param);
      break;
   case 3:
     synthSamplerPiano_Voice_synthSetLoopStart(processorPiano, param);
      break;
   case 4:
     synthSamplerAmbientLead_Voice_synthSetLoopStart(processorAmbientLead, param);
      break;
   case 5:
     synthSamplerAuthenticStrings_Voice_synthSetLoopStart(processorAuthenticStrings, param);
      break;
   case 6:
     synthSamplerBoesendorferGrandPiano_Voice_synthSetLoopStart(processorBoesendorferGrandPiano, param);
      break;
   case 7:
     synthSamplerCelesta_Voice_synthSetLoopStart(processorCelesta, param);
      break;
   case 8:
     synthSamplerClarinets_Voice_synthSetLoopStart(processorClarinets, param);
      break;
   case 9:
     synthSamplerClassicSuitcaseMk1_Voice_synthSetLoopStart(processorClassicSuitcaseMk1, param);
      break;
   case 10:
     synthSamplerCosmos_Voice_synthSetLoopStart(processorCosmos, param);
      break;
   case 11:
     synthSamplerDreamVoice_Voice_synthSetLoopStart(processorDreamVoice, param);
      break;
   case 12:
     synthSamplerDrivenSynthLead_Voice_synthSetLoopStart(processorDrivenSynthLead, param);
      break;
   case 13:
     synthSamplerGlassMarimbaSoft_Voice_synthSetLoopStart(processorGlassMarimbaSoft, param);
      break;
   case 14:
     synthSamplerIndianSitar_Voice_synthSetLoopStart(processorIndianSitar, param);
      break;
   case 15:
     synthSamplerJapaneseShakuhachiFlute_Voice_synthSetLoopStart(processorJapaneseShakuhachiFlute, param);
      break;
   case 16:
     synthSamplerKoto_Voice_synthSetLoopStart(processorKoto, param);
      break;
   case 17:
     synthSamplerMagnificentDrone_Voice_synthSetLoopStart(processorMagnificentDrone, param);
      break;
   case 18:
     synthSamplerMysticVibes_Voice_synthSetLoopStart(processorMysticVibes, param);
      break;
   case 19:
     synthSamplerPowerFifths_Voice_synthSetLoopStart(processorPowerFifths, param);
      break;
   case 20:
     synthSamplerPulsingPatterns_Voice_synthSetLoopStart(processorPulsingPatterns, param);
      break;
   case 21:
     synthSamplerShootingStar_Voice_synthSetLoopStart(processorShootingStar, param);
      break;
   case 22:
     synthSamplerTimeWarp_Voice_synthSetLoopStart(processorTimeWarp, param);
      break;
   case 23:
     synthSamplerTranquility_Voice_synthSetLoopStart(processorTranquility, param);
      break;
   case 24:
     synthSamplerVocalCluster_Voice_synthSetLoopStart(processorVocalCluster, param);
      break;
   case 25:
     synthSamplerZenGarden_Voice_synthSetLoopStart(processorZenGarden, param);
      break;
   case 0:
   default:
     synthSampler_Voice_synthSetLoopStart(processorOcarina, param);
      break;
   }
}

void MetaSampler::setLoopEnd(int param) {
   switch(selectedSample) {
   case 1:
     synthSamplerCello_Voice_synthSetLoopEnd(processorCello, param);
      break;
   case 2:
     synthSamplerClarinet_Voice_synthSetLoopEnd(processorClarinet, param);
      break;
   case 3:
     synthSamplerPiano_Voice_synthSetLoopEnd(processorPiano, param);
      break;
   case 4:
     synthSamplerAmbientLead_Voice_synthSetLoopEnd(processorAmbientLead, param);
      break;
   case 5:
     synthSamplerAuthenticStrings_Voice_synthSetLoopEnd(processorAuthenticStrings, param);
      break;
   case 6:
     synthSamplerBoesendorferGrandPiano_Voice_synthSetLoopEnd(processorBoesendorferGrandPiano, param);
      break;
   case 7:
     synthSamplerCelesta_Voice_synthSetLoopEnd(processorCelesta, param);
      break;
   case 8:
     synthSamplerClarinets_Voice_synthSetLoopEnd(processorClarinets, param);
      break;
   case 9:
     synthSamplerClassicSuitcaseMk1_Voice_synthSetLoopEnd(processorClassicSuitcaseMk1, param);
      break;
   case 10:
     synthSamplerCosmos_Voice_synthSetLoopEnd(processorCosmos, param);
      break;
   case 11:
     synthSamplerDreamVoice_Voice_synthSetLoopEnd(processorDreamVoice, param);
      break;
   case 12:
     synthSamplerDrivenSynthLead_Voice_synthSetLoopEnd(processorDrivenSynthLead, param);
      break;
   case 13:
     synthSamplerGlassMarimbaSoft_Voice_synthSetLoopEnd(processorGlassMarimbaSoft, param);
      break;
   case 14:
     synthSamplerIndianSitar_Voice_synthSetLoopEnd(processorIndianSitar, param);
      break;
   case 15:
     synthSamplerJapaneseShakuhachiFlute_Voice_synthSetLoopEnd(processorJapaneseShakuhachiFlute, param);
      break;
   case 16:
     synthSamplerKoto_Voice_synthSetLoopEnd(processorKoto, param);
      break;
   case 17:
     synthSamplerMagnificentDrone_Voice_synthSetLoopEnd(processorMagnificentDrone, param);
      break;
   case 18:
     synthSamplerMysticVibes_Voice_synthSetLoopEnd(processorMysticVibes, param);
      break;
   case 19:
     synthSamplerPowerFifths_Voice_synthSetLoopEnd(processorPowerFifths, param);
      break;
   case 20:
     synthSamplerPulsingPatterns_Voice_synthSetLoopEnd(processorPulsingPatterns, param);
      break;
   case 21:
     synthSamplerShootingStar_Voice_synthSetLoopEnd(processorShootingStar, param);
      break;
   case 22:
     synthSamplerTimeWarp_Voice_synthSetLoopEnd(processorTimeWarp, param);
      break;
   case 23:
     synthSamplerTranquility_Voice_synthSetLoopEnd(processorTranquility, param);
      break;
   case 24:
     synthSamplerVocalCluster_Voice_synthSetLoopEnd(processorVocalCluster, param);
      break;
   case 25:
     synthSamplerZenGarden_Voice_synthSetLoopEnd(processorZenGarden, param);
      break;
   case 0:
   default:
     synthSampler_Voice_synthSetLoopEnd(processorOcarina, param);
      break;
   }
}

void MetaSampler::setPitchBend(float semitones) {
   switch(selectedSample) {
   case 1:
      synthSamplerCello_Voice_synthPitchBend(processorCello, float_to_fix(semitones));
      break;
   case 2:
      synthSamplerClarinet_Voice_synthPitchBend(processorClarinet, float_to_fix(semitones));
      break;
   case 3:
      synthSamplerPiano_Voice_synthPitchBend(processorPiano, float_to_fix(semitones));
      break;
   case 4:
      synthSamplerAmbientLead_Voice_synthPitchBend(processorAmbientLead, float_to_fix(semitones));
      break;
   case 5:
      synthSamplerAuthenticStrings_Voice_synthPitchBend(processorAuthenticStrings, float_to_fix(semitones));
      break;
   case 6:
      synthSamplerBoesendorferGrandPiano_Voice_synthPitchBend(processorBoesendorferGrandPiano, float_to_fix(semitones));
      break;
   case 7:
      synthSamplerCelesta_Voice_synthPitchBend(processorCelesta, float_to_fix(semitones));
      break;
   case 8:
      synthSamplerClarinets_Voice_synthPitchBend(processorClarinets, float_to_fix(semitones));
      break;
   case 9:
      synthSamplerClassicSuitcaseMk1_Voice_synthPitchBend(processorClassicSuitcaseMk1, float_to_fix(semitones));
      break;
   case 10:
      synthSamplerCosmos_Voice_synthPitchBend(processorCosmos, float_to_fix(semitones));
      break;
   case 11:
      synthSamplerDreamVoice_Voice_synthPitchBend(processorDreamVoice, float_to_fix(semitones));
      break;
   case 12:
      synthSamplerDrivenSynthLead_Voice_synthPitchBend(processorDrivenSynthLead, float_to_fix(semitones));
      break;
   case 13:
      synthSamplerGlassMarimbaSoft_Voice_synthPitchBend(processorGlassMarimbaSoft, float_to_fix(semitones));
      break;
   case 14:
      synthSamplerIndianSitar_Voice_synthPitchBend(processorIndianSitar, float_to_fix(semitones));
      break;
   case 15:
      synthSamplerJapaneseShakuhachiFlute_Voice_synthPitchBend(processorJapaneseShakuhachiFlute, float_to_fix(semitones));
      break;
   case 16:
      synthSamplerKoto_Voice_synthPitchBend(processorKoto, float_to_fix(semitones));
      break;
   case 17:
      synthSamplerMagnificentDrone_Voice_synthPitchBend(processorMagnificentDrone, float_to_fix(semitones));
      break;
   case 18:
      synthSamplerMysticVibes_Voice_synthPitchBend(processorMysticVibes, float_to_fix(semitones));
      break;
   case 19:
      synthSamplerPowerFifths_Voice_synthPitchBend(processorPowerFifths, float_to_fix(semitones));
      break;
   case 20:
      synthSamplerPulsingPatterns_Voice_synthPitchBend(processorPulsingPatterns, float_to_fix(semitones));
      break;
   case 21:
      synthSamplerShootingStar_Voice_synthPitchBend(processorShootingStar, float_to_fix(semitones));
      break;
   case 22:
      synthSamplerTimeWarp_Voice_synthPitchBend(processorTimeWarp, float_to_fix(semitones));
      break;
   case 23:
      synthSamplerTranquility_Voice_synthPitchBend(processorTranquility, float_to_fix(semitones));
      break;
   case 24:
      synthSamplerVocalCluster_Voice_synthPitchBend(processorVocalCluster, float_to_fix(semitones));
      break;
   case 25:
      synthSamplerZenGarden_Voice_synthPitchBend(processorZenGarden, float_to_fix(semitones));
      break;
   case 0:
   default:
      synthSampler_Voice_synthPitchBend(processorOcarina, float_to_fix(semitones));
      break;
   }
}

void MetaSampler::setSustain(bool flag) {
   switch(selectedSample) {
   case 1:
      synthSamplerCello_Voice_synthSetSustain(processorCello, flag);
      break;
   case 2:
      synthSamplerClarinet_Voice_synthSetSustain(processorClarinet, flag);
      break;
   case 3:
      synthSamplerPiano_Voice_synthSetSustain(processorPiano, flag);
      break;
   case 4:
      synthSamplerAmbientLead_Voice_synthSetSustain(processorAmbientLead, flag);
      break;
   case 5:
      synthSamplerAuthenticStrings_Voice_synthSetSustain(processorAuthenticStrings, flag);
      break;
   case 6:
      synthSamplerBoesendorferGrandPiano_Voice_synthSetSustain(processorBoesendorferGrandPiano, flag);
      break;
   case 7:
      synthSamplerCelesta_Voice_synthSetSustain(processorCelesta, flag);
      break;
   case 8:
      synthSamplerClarinets_Voice_synthSetSustain(processorClarinets, flag);
      break;
   case 9:
      synthSamplerClassicSuitcaseMk1_Voice_synthSetSustain(processorClassicSuitcaseMk1, flag);
      break;
   case 10:
      synthSamplerCosmos_Voice_synthSetSustain(processorCosmos, flag);
      break;
   case 11:
      synthSamplerDreamVoice_Voice_synthSetSustain(processorDreamVoice, flag);
      break;
   case 12:
      synthSamplerDrivenSynthLead_Voice_synthSetSustain(processorDrivenSynthLead, flag);
      break;
   case 13:
      synthSamplerGlassMarimbaSoft_Voice_synthSetSustain(processorGlassMarimbaSoft, flag);
      break;
   case 14:
      synthSamplerIndianSitar_Voice_synthSetSustain(processorIndianSitar, flag);
      break;
   case 15:
      synthSamplerJapaneseShakuhachiFlute_Voice_synthSetSustain(processorJapaneseShakuhachiFlute, flag);
      break;
   case 16:
      synthSamplerKoto_Voice_synthSetSustain(processorKoto, flag);
      break;
   case 17:
      synthSamplerMagnificentDrone_Voice_synthSetSustain(processorMagnificentDrone, flag);
      break;
   case 18:
      synthSamplerMysticVibes_Voice_synthSetSustain(processorMysticVibes, flag);
      break;
   case 19:
      synthSamplerPowerFifths_Voice_synthSetSustain(processorPowerFifths, flag);
      break;
   case 20:
      synthSamplerPulsingPatterns_Voice_synthSetSustain(processorPulsingPatterns, flag);
      break;
   case 21:
      synthSamplerShootingStar_Voice_synthSetSustain(processorShootingStar, flag);
      break;
   case 22:
      synthSamplerTimeWarp_Voice_synthSetSustain(processorTimeWarp, flag);
      break;
   case 23:
      synthSamplerTranquility_Voice_synthSetSustain(processorTranquility, flag);
      break;
   case 24:
      synthSamplerVocalCluster_Voice_synthSetSustain(processorVocalCluster, flag);
      break;
   case 25:
      synthSamplerZenGarden_Voice_synthSetSustain(processorZenGarden, flag);
      break;
   case 0:
   default:
      synthSampler_Voice_synthSetSustain(processorOcarina, flag);
      break;
   }
}


void MetaSampler::noteOn(int note, int velocity, int channel) {
   switch(selectedSample) {
   case 1:
     synthSamplerCello_Voice_noteOn(processorCello, note, velocity, channel);
     break;
   case 2:
     synthSamplerClarinet_Voice_noteOn(processorClarinet, note, velocity, channel);
     break;
   case 3:
     synthSamplerPiano_Voice_noteOn(processorPiano, note, velocity, channel);
     break;
   case 4:
     synthSamplerAmbientLead_Voice_noteOn(processorAmbientLead, note, velocity, channel);
     break;
   case 5:
     synthSamplerAuthenticStrings_Voice_noteOn(processorAuthenticStrings, note, velocity, channel);
     break;
   case 6:
     synthSamplerBoesendorferGrandPiano_Voice_noteOn(processorBoesendorferGrandPiano, note, velocity, channel);
     break;
   case 7:
     synthSamplerCelesta_Voice_noteOn(processorCelesta, note, velocity, channel);
     break;
   case 8:
     synthSamplerClarinets_Voice_noteOn(processorClarinets, note, velocity, channel);
     break;
   case 9:
     synthSamplerClassicSuitcaseMk1_Voice_noteOn(processorClassicSuitcaseMk1, note, velocity, channel);
     break;
   case 10:
     synthSamplerCosmos_Voice_noteOn(processorCosmos, note, velocity, channel);
     break;
   case 11:
     synthSamplerDreamVoice_Voice_noteOn(processorDreamVoice, note, velocity, channel);
     break;
   case 12:
     synthSamplerDrivenSynthLead_Voice_noteOn(processorDrivenSynthLead, note, velocity, channel);
     break;
   case 13:
     synthSamplerGlassMarimbaSoft_Voice_noteOn(processorGlassMarimbaSoft, note, velocity, channel);
     break;
   case 14:
     synthSamplerIndianSitar_Voice_noteOn(processorIndianSitar, note, velocity, channel);
     break;
   case 15:
     synthSamplerJapaneseShakuhachiFlute_Voice_noteOn(processorJapaneseShakuhachiFlute, note, velocity, channel);
     break;
   case 16:
     synthSamplerKoto_Voice_noteOn(processorKoto, note, velocity, channel);
     break;
   case 17:
     synthSamplerMagnificentDrone_Voice_noteOn(processorMagnificentDrone, note, velocity, channel);
     break;
   case 18:
     synthSamplerMysticVibes_Voice_noteOn(processorMysticVibes, note, velocity, channel);
     break;
   case 19:
     synthSamplerPowerFifths_Voice_noteOn(processorPowerFifths, note, velocity, channel);
     break;
   case 20:
     synthSamplerPulsingPatterns_Voice_noteOn(processorPulsingPatterns, note, velocity, channel);
     break;
   case 21:
     synthSamplerShootingStar_Voice_noteOn(processorShootingStar, note, velocity, channel);
     break;
   case 22:
     synthSamplerTimeWarp_Voice_noteOn(processorTimeWarp, note, velocity, channel);
     break;
   case 23:
     synthSamplerTranquility_Voice_noteOn(processorTranquility, note, velocity, channel);
     break;
   case 24:
     synthSamplerVocalCluster_Voice_noteOn(processorVocalCluster, note, velocity, channel);
     break;
   case 25:
     synthSamplerZenGarden_Voice_noteOn(processorZenGarden, note, velocity, channel);
     break;
   case 0:
   default:
     synthSampler_Voice_noteOn(processorOcarina, note, velocity, channel);
     break;
   }
}

void MetaSampler::noteOff(int note, int channel) {
   switch(selectedSample) {
   case 1:
     synthSamplerCello_Voice_noteOff(processorCello, note, channel);
     break;
   case 2:
     synthSamplerClarinet_Voice_noteOff(processorClarinet, note, channel);
     break;
   case 3:
     synthSamplerPiano_Voice_noteOff(processorPiano, note, channel);
     break;
   case 4:
     synthSamplerAmbientLead_Voice_noteOff(processorAmbientLead, note, channel);
     break;
   case 5:
     synthSamplerAuthenticStrings_Voice_noteOff(processorAuthenticStrings, note, channel);
     break;
   case 6:
     synthSamplerBoesendorferGrandPiano_Voice_noteOff(processorBoesendorferGrandPiano, note, channel);
     break;
   case 7:
     synthSamplerCelesta_Voice_noteOff(processorCelesta, note, channel);
     break;
   case 8:
     synthSamplerClarinets_Voice_noteOff(processorClarinets, note, channel);
     break;
   case 9:
     synthSamplerClassicSuitcaseMk1_Voice_noteOff(processorClassicSuitcaseMk1, note, channel);
     break;
   case 10:
     synthSamplerCosmos_Voice_noteOff(processorCosmos, note, channel);
     break;
   case 11:
     synthSamplerDreamVoice_Voice_noteOff(processorDreamVoice, note, channel);
     break;
   case 12:
     synthSamplerDrivenSynthLead_Voice_noteOff(processorDrivenSynthLead, note, channel);
     break;
   case 13:
     synthSamplerGlassMarimbaSoft_Voice_noteOff(processorGlassMarimbaSoft, note, channel);
     break;
   case 14:
     synthSamplerIndianSitar_Voice_noteOff(processorIndianSitar, note, channel);
     break;
   case 15:
     synthSamplerJapaneseShakuhachiFlute_Voice_noteOff(processorJapaneseShakuhachiFlute, note, channel);
     break;
   case 16:
     synthSamplerKoto_Voice_noteOff(processorKoto, note, channel);
     break;
   case 17:
     synthSamplerMagnificentDrone_Voice_noteOff(processorMagnificentDrone, note, channel);
     break;
   case 18:
     synthSamplerMysticVibes_Voice_noteOff(processorMysticVibes, note, channel);
     break;
   case 19:
     synthSamplerPowerFifths_Voice_noteOff(processorPowerFifths, note, channel);
     break;
   case 20:
     synthSamplerPulsingPatterns_Voice_noteOff(processorPulsingPatterns, note, channel);
     break;
   case 21:
     synthSamplerShootingStar_Voice_noteOff(processorShootingStar, note, channel);
     break;
   case 22:
     synthSamplerTimeWarp_Voice_noteOff(processorTimeWarp, note, channel);
     break;
   case 23:
     synthSamplerTranquility_Voice_noteOff(processorTranquility, note, channel);
     break;
   case 24:
     synthSamplerVocalCluster_Voice_noteOff(processorVocalCluster, note, channel);
     break;
   case 25:
     synthSamplerZenGarden_Voice_noteOff(processorZenGarden, note, channel);
     break;
   case 0:
   default:
     synthSampler_Voice_noteOff(processorOcarina, note, channel);
     break;
   }
}

float MetaSampler::process(int nb, fix16_t (&oBuffer)[128]) {
   switch(selectedSample) {
   case 1:
     synthSamplerCello_Voice_process_bufferTo(processorCello, nb, oBuffer);
     break;
   case 2:
     synthSamplerClarinet_Voice_process_bufferTo(processorClarinet, nb, oBuffer);
     break;
   case 3:
     synthSamplerPiano_Voice_process_bufferTo(processorPiano, nb, oBuffer);
     break;
   case 4:
     synthSamplerAmbientLead_Voice_process_bufferTo(processorAmbientLead, nb, oBuffer);
     break;
   case 5:
     synthSamplerAuthenticStrings_Voice_process_bufferTo(processorAuthenticStrings, nb, oBuffer);
     break;
   case 6:
     synthSamplerBoesendorferGrandPiano_Voice_process_bufferTo(processorBoesendorferGrandPiano, nb, oBuffer);
     break;
   case 7:
     synthSamplerCelesta_Voice_process_bufferTo(processorCelesta, nb, oBuffer);
     break;
   case 8:
     synthSamplerClarinets_Voice_process_bufferTo(processorClarinets, nb, oBuffer);
     break;
   case 9:
     synthSamplerClassicSuitcaseMk1_Voice_process_bufferTo(processorClassicSuitcaseMk1, nb, oBuffer);
     break;
   case 10:
     synthSamplerCosmos_Voice_process_bufferTo(processorCosmos, nb, oBuffer);
     break;
   case 11:
     synthSamplerDreamVoice_Voice_process_bufferTo(processorDreamVoice, nb, oBuffer);
     break;
   case 12:
     synthSamplerDrivenSynthLead_Voice_process_bufferTo(processorDrivenSynthLead, nb, oBuffer);
     break;
   case 13:
     synthSamplerGlassMarimbaSoft_Voice_process_bufferTo(processorGlassMarimbaSoft, nb, oBuffer);
     break;
   case 14:
     synthSamplerIndianSitar_Voice_process_bufferTo(processorIndianSitar, nb, oBuffer);
     break;
   case 15:
     synthSamplerJapaneseShakuhachiFlute_Voice_process_bufferTo(processorJapaneseShakuhachiFlute, nb, oBuffer);
     break;
   case 16:
     synthSamplerKoto_Voice_process_bufferTo(processorKoto, nb, oBuffer);
     break;
   case 17:
     synthSamplerMagnificentDrone_Voice_process_bufferTo(processorMagnificentDrone, nb, oBuffer);
     break;
   case 18:
     synthSamplerMysticVibes_Voice_process_bufferTo(processorMysticVibes, nb, oBuffer);
     break;
   case 19:
     synthSamplerPowerFifths_Voice_process_bufferTo(processorPowerFifths, nb, oBuffer);
     break;
   case 20:
     synthSamplerPulsingPatterns_Voice_process_bufferTo(processorPulsingPatterns, nb, oBuffer);
     break;
   case 21:
     synthSamplerShootingStar_Voice_process_bufferTo(processorShootingStar, nb, oBuffer);
     break;
   case 22:
     synthSamplerTimeWarp_Voice_process_bufferTo(processorTimeWarp, nb, oBuffer);
     break;
   case 23:
     synthSamplerTranquility_Voice_process_bufferTo(processorTranquility, nb, oBuffer);
     break;
   case 24:
     synthSamplerVocalCluster_Voice_process_bufferTo(processorVocalCluster, nb, oBuffer);
     break;
   case 25:
     synthSamplerZenGarden_Voice_process_bufferTo(processorZenGarden, nb, oBuffer);
     break;
   case 0:
   default:
     synthSampler_Voice_process_bufferTo(processorOcarina, nb, oBuffer);
     break;
   }
}

void MetaSampler::initProc() {
   switch(selectedSample) {
   case 1:
      synthSamplerCello_Voice_process_init(processorCello);
      break;
   case 2:
      synthSamplerClarinet_Voice_process_init(processorClarinet);
      break;
   case 3:
      synthSamplerPiano_Voice_process_init(processorPiano);
      break;
   case 4:
      synthSamplerAmbientLead_Voice_process_init(processorAmbientLead);
      break;
   case 5:
      synthSamplerAuthenticStrings_Voice_process_init(processorAuthenticStrings);
      break;
   case 6:
      synthSamplerBoesendorferGrandPiano_Voice_process_init(processorBoesendorferGrandPiano);
      break;
   case 7:
      synthSamplerCelesta_Voice_process_init(processorCelesta);
      break;
   case 8:
      synthSamplerClarinets_Voice_process_init(processorClarinets);
      break;
   case 9:
      synthSamplerClassicSuitcaseMk1_Voice_process_init(processorClassicSuitcaseMk1);
      break;
   case 10:
      synthSamplerCosmos_Voice_process_init(processorCosmos);
      break;
   case 11:
      synthSamplerDreamVoice_Voice_process_init(processorDreamVoice);
      break;
   case 12:
      synthSamplerDrivenSynthLead_Voice_process_init(processorDrivenSynthLead);
      break;
   case 13:
      synthSamplerGlassMarimbaSoft_Voice_process_init(processorGlassMarimbaSoft);
      break;
   case 14:
      synthSamplerIndianSitar_Voice_process_init(processorIndianSitar);
      break;
   case 15:
      synthSamplerJapaneseShakuhachiFlute_Voice_process_init(processorJapaneseShakuhachiFlute);
      break;
   case 16:
      synthSamplerKoto_Voice_process_init(processorKoto);
      break;
   case 17:
      synthSamplerMagnificentDrone_Voice_process_init(processorMagnificentDrone);
      break;
   case 18:
      synthSamplerMysticVibes_Voice_process_init(processorMysticVibes);
      break;
   case 19:
      synthSamplerPowerFifths_Voice_process_init(processorPowerFifths);
      break;
   case 20:
      synthSamplerPulsingPatterns_Voice_process_init(processorPulsingPatterns);
      break;
   case 21:
      synthSamplerShootingStar_Voice_process_init(processorShootingStar);
      break;
   case 22:
      synthSamplerTimeWarp_Voice_process_init(processorTimeWarp);
      break;
   case 23:
      synthSamplerTranquility_Voice_process_init(processorTranquility);
      break;
   case 24:
      synthSamplerVocalCluster_Voice_process_init(processorVocalCluster);
      break;
   case 25:
      synthSamplerZenGarden_Voice_process_init(processorZenGarden);
      break;
   case 0:
   default:
      synthSampler_Voice_process_init(processorOcarina);
      break;
   }
}
