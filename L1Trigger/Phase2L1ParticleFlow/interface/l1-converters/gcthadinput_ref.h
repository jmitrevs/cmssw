#ifndef L1Trigger_Phase2L1ParticleFlow_newfirmware_gcthadinput_ref_h
#define L1Trigger_Phase2L1ParticleFlow_newfirmware_gcthadinput_ref_h

#include "DataFormats/L1TParticleFlow/interface/layer1_emulator.h"
#include "DataFormats/L1TCalorimeterPhase2/interface/GCTHadDigiCluster.h"

// TODO:  add calibration

namespace edm {
  class ParameterSet;
  class ParameterSetDescription;
}  // namespace edm

namespace l1ct {
  class GctHadClusterDecoderEmulator {

  public:
    GctHadClusterDecoderEmulator() {};
    GctHadClusterDecoderEmulator(const edm::ParameterSet &pset);

    ~GctHadClusterDecoderEmulator();

    double fracPart(const double total, const unsigned int hoe) const;

    static edm::ParameterSetDescription getParameterSetDescription();

    l1ct::HadCaloObjEmu decode(const l1ct::DetectorSector<l1ct::HadCaloObjEmu> &sec,
                               const l1tp2::GCTHadDigiCluster &digi) const;
  };
}  // namespace l1ct

#endif
